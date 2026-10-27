#include "GameEngine.h"
#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "rlImGui.h"

void GameEngine::update() {
    // 1. AUDIO STREAM MANAGEMENT
    
    // 2. WORLD MAP REGION SELECTION LOGIC
    if (m_currentState == GameState::WorldMap) {
        updateWorldMap();
    }
    // 3. REGION HUB LOGIC
    else if (m_currentState == GameState::RegionHub) {
        updateRegionHub();
    }
    // 4. DIALOGUE STATE MANAGEMENT
    else if (m_currentState == GameState::Dialogue) {
        updateDialogue();
    }
    // 5. INVENTORY STATE MANAGEMENT
    else if (m_currentState == GameState::Inventory) {
        updateInventory();
    }
    // 5. COMBAT STATE MANAGEMENT
    else if (m_currentState == GameState::Combat) {
        updateCombat();
    }
}

void GameEngine::updateWorldMap() {
    playerMovement(4.0f);
    Vector2 playerPos = m_player.getPosition();
    m_camera.target = m_player.getPosition();
    
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

void GameEngine::updateRegionHub() {
    if (m_selectRealm == Realm::Ignis) {
        UpdateMusicStream(m_ignisSound);
    }
    else if (m_selectRealm == Realm::Aqua) {
        UpdateMusicStream(m_aquaSound);
    }
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
        updateRegionHubNotMap(currentHub);
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

void GameEngine::updateCombat() {
    if (!m_activeEnemy) return;
    if (m_currentPhase == CombatPhase::ActionSelect) {
        int selectedAction = getSelectedAttack();

        if (selectedAction != -1) {
            const auto& attacks = m_player.getAttacks();
            const Attack& chosenAttack = attacks[selectedAction];

            if (chosenAttack.damage > 0) {
                m_activeEnemy->takeDamage(chosenAttack.damage);
            }
            if (chosenAttack.healing > 0) {
                m_player.heal(chosenAttack.healing);
            }
            if (m_activeEnemy->getHealth() <= 0) {
                    m_activeEnemy->setDefeated(true);
                    m_lastLootGold = 25;
                    m_player.addGold(m_lastLootGold);
                    m_currentPhase = CombatPhase::Victory;
                }
            else {
                m_player.takeDamage(m_activeEnemy->getAttackPower());

                if (m_player.getHP() <= 0) {
                    m_currentPhase = CombatPhase::Defeat;
                }
            }
        }
        if (IsKeyPressed(KEY_R)) {
            m_activeEnemy = nullptr;
            m_currentState = GameState::RegionHub;
        }
    }
    else if (m_currentPhase == CombatPhase::Victory) {
        if (IsKeyPressed(KEY_ENTER)) {
            m_activeEnemy = nullptr;
            m_currentPhase = CombatPhase::ActionSelect;
            m_currentState = GameState::RegionHub;
        }
    }
    else {
        if (IsKeyPressed(KEY_ENTER)) {
            m_activeEnemy = nullptr;
            m_player.resetHP();
            m_currentPhase = CombatPhase::ActionSelect;
            m_currentState = GameState::RegionHub;
        }
    }
}

void GameEngine::updateInventory() {
    if (IsKeyPressed(KEY_I)){
        m_currentState = GameState::RegionHub;
        return;
    }

    if (m_inventory.empty()) return;

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        m_inventoryIndex = (m_inventoryIndex - 1 + m_inventory.size()) % m_inventory.size();
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        m_inventoryIndex - (m_inventoryIndex + 1) % m_inventory.size();
    }

    if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER)) {
        auto& item = m_inventory[m_inventoryIndex];
        bool itemUsed = false;

        switch (item.type)
        {
        case ItemType::Heal:
            m_player.heal(item.attribute);
            itemUsed = true;
            break;
        
        // MANA NOT ADDED YET :P

        // case ItemType::RestoreMana:
        //     m_player.modifyMastery(Realm::Aqua, item.attribute);
        //     itemUsed = true;
        //     break;
        // case ItemType::None:
        default:
            break;
        }

        if (itemUsed) {
            item.quantity--;

            if (item.quantity <= 0) {
                m_inventory.erase(m_inventory.begin() + m_inventoryIndex);
                if (m_inventoryIndex >= m_inventory.size() && !m_inventory.empty()) {
                    m_inventoryIndex = m_inventory.size() - 1;
                }
            }
        }
        
    }
}

void GameEngine::updateDialogue() {
    if (!m_dialogueManager.isDialogueActive()){
        m_currentState = GameState::RegionHub;
    }   
}

void GameEngine::updateCamera(const RegionHubG* currentHub) {
    if (!currentHub) return;

    Vector2 cameraTarget = m_player.getPosition();
    float halfScreenWidth = (1280.0f / 2.0f) / m_camera.zoom;
    float halfScrenHeight = (720.0f / 2.0f) / m_camera.zoom;

    float minCameraX = currentHub->rectangle.x + halfScreenWidth;
    float maxCameraX = currentHub->rectangle.y + currentHub->rectangle.width - halfScreenWidth;
    if (cameraTarget.x < minCameraX) cameraTarget.x = minCameraX;
    if (cameraTarget.x > maxCameraX) cameraTarget.x = maxCameraX;

    float minCameraY = currentHub->rectangle.y + halfScrenHeight;
    float maxCameraY = currentHub->rectangle.y + currentHub->rectangle.height - halfScrenHeight;
    if (cameraTarget.y < minCameraY) cameraTarget.y = minCameraY;
    if (cameraTarget.y > maxCameraY) cameraTarget.y = maxCameraY;

    m_camera.target = cameraTarget;
}

void GameEngine::updateRegionHubNotMap(RegionHubG* currentHub) {
    playerMovement(4.0f);
    Vector2 playerPos = m_player.getPosition();
    
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

    updateCamera(currentHub);
}


