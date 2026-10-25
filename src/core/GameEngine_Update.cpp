#include "GameEngine.h"
#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "rlImGui.h"

void GameEngine::update() {
    // 1. AUDIO STREAM MANAGEMENT
    if (m_selectRealm == Realm::Ignis && m_currentState == GameState::Dialogue) {
        UpdateMusicStream(m_ignisSound);
    }
    else if (m_selectRealm == Realm::Aqua && m_currentState == GameState::Dialogue) {
        UpdateMusicStream(m_aquaSound);
    }
    else if (m_currentState == GameState::WorldMap) {
        if (!m_dialogueManager.isDialogueActive()) {
            StopMusicStream(m_ignisSound);
            StopMusicStream(m_aquaSound);
        }
    }
    // 2. MOVEMENT (WorldMap & RegionHub)

    if (m_currentState == GameState::WorldMap || m_currentState == GameState::RegionHub) {
        
        
    }

    // 3. WORLD MAP REGION SELECTION LOGIC
    if (m_currentState == GameState::WorldMap) {
        Vector2 playerPos = m_player.getPosition();
        float velocity = 4.0f;
        Vector2 movement = { 0.0f, 0.0f };
        m_camera.target = m_player.getPosition();
        
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) movement.y -= 1.0f;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) movement.y += 1.0f;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) movement.x -= 1.0f;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) movement.x += 1.0f;
        
        if (movement.x != 0.0f || movement.y != 0.0f) {
            m_player.move({ movement.x * velocity, movement.y * velocity });
        }
        if (IsKeyPressed(KEY_F)) {
            for (const auto& region : m_regions) {
                if (CheckCollisionPointRec(playerPos, region.bounds)) {
                    m_selectRealm = region.realm;
                    
                    if (m_selectRealm == Realm::Aqua) PlayMusicStream(m_aquaSound);
                    else if (m_selectRealm == Realm::Ignis) PlayMusicStream(m_ignisSound);

                    for (const auto& hub : m_regionHub) {
                        if (hub.realm == m_selectRealm) {
                            m_player.setPosition({
                                hub.rectangle.x + (hub.rectangle.width / 2),
                                hub.rectangle.y + (hub.rectangle.height / 2)
                            });
                            break;
                        }
                    }

                    m_currentState = GameState::RegionHub;
                    break;
                }
            }
        }
    }
    // 4. REGION HUB LOGIC
    else if (m_currentState == GameState::RegionHub) {
        RegionHubG* currentHub = nullptr;
        for (auto& hub : m_regionHub) {
            if (hub.realm == m_selectRealm) {
                currentHub = &hub;
                break;
            }
        }
        if (!currentHub) return;
        if (IsKeyPressed(KEY_Q)) {
            m_isMapWindowOpen = !m_isMapWindowOpen;
        }
        if (!m_isMapWindowOpen) {
            Vector2 playerPos = m_player.getPosition();
            float velocity = 4.0f;
            Vector2 movement = { 0.0f, 0.0f };
            
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) movement.y -= 1.0f;
            if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) movement.y += 1.0f;
            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) movement.x -= 1.0f;
            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) movement.x += 1.0f;

            // Player bounding boxes matching top-left render origin
            Rectangle playerRectX = { playerPos.x + (movement.x * velocity), playerPos.y, 10.0f, 15.0f };
            Rectangle playerRectY = { playerPos.x, playerPos.y + (movement.y * velocity), 10.0f, 15.0f };

            // Axis-independent wall collision checks
            for (auto& regionBldg : m_buildings) {
                if (regionBldg.realm == m_selectRealm) {
                    for (auto& bldg : regionBldg.buildings) {
                        bldg.updateStatus(playerPos);

                        for (const auto& wall : bldg.walls) {
                            if (CheckCollisionRecs(playerRectX, wall.bounds)) {
                                movement.x = 0.0f;
                            }
                            if (CheckCollisionRecs(playerRectY, wall.bounds)) {
                                movement.y = 0.0f;
                            }
                        }
                    }
                }
            }

            Rectangle playerFutureX = { playerPos.x + (movement.x * velocity) - 5.0f, playerPos.y - 7.5f, 10.0f, 15.0f };
            Rectangle playerFutureY = { playerPos.x - 5.0f, playerPos.y + (movement.x * velocity) - 7.5f, 10.0f, 15.0f };

            if (playerFutureX.x < currentHub->rectangle.x || (playerFutureX.x + playerFutureX.width) > (currentHub->rectangle.x + currentHub->rectangle.width)) {
                movement.x = 0.0f;
            }
            if (playerFutureY.y < currentHub->rectangle.y || (playerFutureY.y + playerFutureY.height) > (currentHub->rectangle.y + currentHub->rectangle.height)) {
                movement.y = 0.0f;
            }
            if (movement.x != 0.0f || movement.y != 0.0f) {
                m_player.move({ movement.x * velocity, movement.y * velocity });
                playerPos = m_player.getPosition();
            }
            if (IsKeyPressed(KEY_I)){
                m_currentState =  GameState::Inventory;
            }
            if (IsKeyPressed(KEY_F)) {
                RegionHubG* currentHub = nullptr;
                for (auto& hub : m_regionHub) {
                    if (hub.realm == m_selectRealm) {
                        currentHub = &hub;
                        break;
                    }
                }
                if (currentHub) {
                    for (const auto& npc : currentHub->npcs) {
                        Rectangle npcRec = {
                            npc.npcGraphics.npcPos.x - (npc.npcCollision.x / 2.0f),
                            npc.npcGraphics.npcPos.y - (npc.npcCollision.y / 2.0f),
                            npc.npcCollision.x,
                            npc.npcCollision.y
                        };

                        if (CheckCollisionPointRec(playerPos, npcRec)) {
                            if (m_dialogueManager.loadFromFile(npc.dialogueFile, npc.npcId)){
                                m_dialogueManager.startDialogue(0);
                                m_currentState = GameState::Dialogue;
                            }
                            break;
                        }
                    }
                }
            }

            if (IsKeyPressed(KEY_SPACE)) {
                for (auto& enemy : currentHub->enemies) {
                    if (!enemy.isDefeated()) {
                        Rectangle enemyRec = {
                            enemy.getPosition().x - (enemy.getCollisionBox().x / 2.0f),
                            enemy.getPosition().y - (enemy.getCollisionBox().y / 2.0f),
                            enemy.getCollisionBox().x,
                            enemy.getCollisionBox().y
                        };

                        if (CheckCollisionPointRec(playerPos, enemyRec)) {
                            m_activeEnemy = &enemy;
                            m_currentState = GameState::Combat;
                            break;
                        }
                    }
                }
            }

            if (IsKeyPressed(KEY_E)){
                for (auto& wItem : m_worldItems) {
                    if (!wItem.isCollected && wItem.realm == m_selectRealm) {
                        float dist = Vector2Distance(playerPos, wItem.position);
                        if (dist < 35.0f) {
                            wItem.isCollected = true;
                            bool found = false;
                            for (auto& invItem : m_inventory) {
                                if (invItem.name == wItem.name) {
                                    invItem.quantity += wItem.quantity;
                                    found = true;
                                    break;
                                }
                            }
                            if(!found) {
                                m_inventory.push_back({wItem.name, wItem.description, wItem.quantity});
                            }
                            break;
                        }
                    }
                }
            }

            Vector2 cameraTarget = m_player.getPosition();

            float halfScreenWidth = (1280.0f / 2.0f) / m_camera.zoom;
            float halfScreenHeight = (720.0f / 2.0f) / m_camera.zoom;

            float minCameraX = currentHub->rectangle.x + halfScreenWidth;
            float maxCameraX = currentHub->rectangle.x + currentHub->rectangle.width - halfScreenWidth; 
            if (cameraTarget.x < minCameraX) cameraTarget.x = minCameraX;
            if (cameraTarget.x > maxCameraX) cameraTarget.x = maxCameraX;
            
            float minCameraY = currentHub->rectangle.y + halfScreenHeight;
            float maxCameraY = currentHub->rectangle.y + currentHub->rectangle.height - halfScreenHeight; 
            if (cameraTarget.y < minCameraY) cameraTarget.y = minCameraY;
            if (cameraTarget.y > maxCameraY) cameraTarget.y = maxCameraY;

            m_camera.target = cameraTarget;
        }
        else {
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)){
                m_mapRow = (m_mapRow - 1 + 2) % 2;
            }
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)){
                m_mapRow = (m_mapRow + 1) % 2;
            }
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)){
                m_mapCol = (m_mapCol - 1 + 2) % 2;
            }
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)){
                m_mapCol = (m_mapCol + 1) % 2;
            }

            int currentIndex = m_mapRow * 2 + m_mapCol;

            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_F)){
                const auto& selectedRegion = m_regions[currentIndex];
                m_selectRealm = selectedRegion.realm;

                if (m_selectRealm == Realm::Aqua) PlayMusicStream(m_aquaSound);
                else if (m_selectRealm == Realm::Ignis) PlayMusicStream(m_ignisSound);

                for (const auto& hub : m_regionHub) {
                    if (hub.realm == m_selectRealm) {
                        m_player.setPosition({
                            hub.rectangle.x + (hub.rectangle.width / 2),
                            hub.rectangle.y + (hub.rectangle.height / 2)
                        });
                        break;
                    }
                }
                m_isMapWindowOpen = false;
            }
        }
    }

    // 5. DIALOGUE STATE MANAGEMENT
    else if (m_currentState == GameState::Dialogue) {
        if (!m_dialogueManager.isDialogueActive()){
            m_currentState = GameState::RegionHub;
        }
    }
    else if (m_currentState == GameState::Inventory) {
        if (IsKeyPressed(KEY_I)){
            m_currentState = GameState::RegionHub;
        }
    }
    else if (m_currentState == GameState::Combat) {
        if (m_activeEnemy) {
            if (IsKeyPressed(KEY_A)) {
                int playerDamage = 10;
                m_activeEnemy->takeDamage(playerDamage);
            
                if (m_activeEnemy->getHealth() <= 0) {
                    m_activeEnemy->setDefeated(true);
                    m_player.addGold(25);
                    m_activeEnemy = nullptr;
                    m_currentState = GameState::RegionHub;
                } else {
                    m_player.takeDamage(m_activeEnemy->getAttackPower());
                    if (m_player.getHP() <= 0) {
                        m_activeEnemy = nullptr;
                        m_currentState = GameState::RegionHub;
                    }
                }
            }
            if (IsKeyPressed(KEY_R)) {
                m_activeEnemy = nullptr;
                m_currentState = GameState::RegionHub;
            }
        }
    }
}