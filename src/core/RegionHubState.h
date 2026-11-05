#ifndef REGION_HUB_STATE_H
#define REGION_HUB_STATE_H

#include "GameState.h"
#include "GameEngine.h"
#include "InventoryState.h"
#include "CombatState.h"
#include "ShopState.h"
#include "QuestLogState.h"
#include "DialogueState.h"
#include "raylib.h"
#include "raymath.h"

class RegionHubState : public GameState {
public:
    void enter(GameEngine& engine) override {
        if (engine.m_selectRealm == Realm::Ignis) {
            PlayMusicStream(engine.m_ignisSound);
        } else if (engine.m_selectRealm == Realm::Aqua) {
            PlayMusicStream(engine.m_aquaSound);
        }
    }

    void update(GameEngine& engine) override {
        if (engine.m_selectRealm == Realm::Ignis) {
            UpdateMusicStream(engine.m_ignisSound);
        } else if (engine.m_selectRealm == Realm::Aqua) {
            UpdateMusicStream(engine.m_aquaSound);
        }

        RegionHubG* currentHub = nullptr;
        for (auto& hub : engine.m_regionHub) {
            if (hub.realm == engine.m_selectRealm) {
                currentHub = &hub;
                break;
            }
        }
        if (!currentHub) return;

        if (IsKeyPressed(KEY_Q)) {
            engine.m_isMapWindowOpen = !engine.m_isMapWindowOpen;
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            engine.goToPause();
            return;
        }

        if (!engine.m_isMapWindowOpen) {
            updateActiveHub(engine, currentHub);
        } else {
            updateMapWindow(engine);
        }
    }

    void render(GameEngine& engine) override {
        RegionHubG* currentHub = nullptr;
        for (auto& hub : engine.m_regionHub) {
            if (hub.realm == engine.m_selectRealm) {
                currentHub = &hub;
                break;
            }
        }
        if (!currentHub) return;

        BeginMode2D(engine.m_camera);
        renderRegionContent(engine, currentHub);
        EndMode2D();

        if (engine.m_isMapWindowOpen) {
            renderFastTravelWindow(engine);
        } else {
            renderMinimapContent(engine, currentHub);
            renderHubControlsContent(engine);
        }
    }

private:
    void updateActiveHub(GameEngine& engine, RegionHubG* currentHub) {
        engine.playerMovement(4.0f);
        Vector2 playerPos = engine.m_player.getPosition();
        
        if (IsKeyPressed(KEY_J)) {
            engine.changeState(std::make_unique<QuestLogState>());
            return;
        }

        std::vector<WallCollider> activeWalls;
        for (const auto& regionBldg : engine.m_buildings) {
            if (regionBldg.realm == engine.m_selectRealm) {
                for (const auto& bldg : regionBldg.buildings) {
                    activeWalls.insert(activeWalls.end(), bldg.walls.begin(), bldg.walls.end());
                }
            }
        }

        for (auto& enemy : currentHub->enemies) {
            if (!enemy.isDefeated()) {
                enemy.updateAI(playerPos, activeWalls, currentHub->rectangle);

                Rectangle enemyRec = enemy.getCollisionBox();
                Rectangle playerRec = { playerPos.x, playerPos.y, 10.0f, 15.0f };

                if (CheckCollisionRecs(playerRec, enemyRec)) {
                    engine.m_activeEnemy = &enemy;
                    engine.changeState(std::make_unique<CombatState>());
                    return;
                }
            }
        }

        if (IsKeyPressed(KEY_I)) {
            engine.changeState(std::make_unique<InventoryState>());
            return;
        }

        if (IsKeyPressed(KEY_F)) {
            RegionHubG* hubToCheck = nullptr;
            for (auto& hub : engine.m_regionHub) {
                if (hub.realm == engine.m_selectRealm) {
                    hubToCheck = &hub;
                    break;
                }
            }
            if (hubToCheck) {
                for (const auto& npc : hubToCheck->npcs) {
                    Rectangle npcRec = {
                        npc.npcGraphics.npcPos.x - (npc.npcCollision.x / 2.0f),
                        npc.npcGraphics.npcPos.y - (npc.npcCollision.y / 2.0f),
                        npc.npcCollision.x,
                        npc.npcCollision.y
                    };

                    if (CheckCollisionPointRec(playerPos, npcRec)) {
                        if (npc.npcId == "gwen") {
                            engine.changeState(std::make_unique<ShopState>());
                        } else {
                            if (engine.m_dialogueManager.loadFromFile(npc.dialogueFile, npc.npcId)) {
                                engine.m_dialogueManager.startDialogue(0);
                                engine.changeState(std::make_unique<DialogueState>());
                            }
                        }
                        return;
                    }
                }
            }
        }

        if (IsKeyPressed(KEY_E)) {
            for (auto& wItem : engine.m_worldItems) {
                if (!wItem.isCollected() && wItem.getRealm() == engine.m_selectRealm) {
                    float dist = Vector2Distance(playerPos, wItem.getPosition());
                    if (dist < 35.0f) {
                        wItem.setCollected(true);
                        engine.m_inventory.addItem(engine.createItem(wItem.getID(), wItem.getQuantity()));

                        if (wItem.getID() == ItemID::ElementalCrystal) {
                            for (auto& quest : engine.m_quests) {
                                if (quest.id == QuestID::CrystalGathering && quest.isAccepted && !quest.isCompleted) {
                                    quest.currentProgress++;
                                    if (quest.currentProgress >= quest.targetProgress) {
                                        engine.completeQuest(quest);
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }

        engine.updateCamera(currentHub);
    }

    void updateMapWindow(GameEngine& engine) {
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) engine.m_mapRow = (engine.m_mapRow - 1 + 2) % 2;
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) engine.m_mapRow = (engine.m_mapRow + 1) % 2;
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) engine.m_mapCol = (engine.m_mapCol - 1 + 2) % 2;
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) engine.m_mapCol = (engine.m_mapCol + 1) % 2;

        int currentIndex = engine.m_mapRow * 2 + engine.m_mapCol;

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_F)) {
            const auto& selectedRegion = engine.m_regions[currentIndex];
            engine.m_selectRealm = selectedRegion.realm;

            if (engine.m_selectRealm == Realm::Aqua) PlayMusicStream(engine.m_aquaSound);
            else if (engine.m_selectRealm == Realm::Ignis) PlayMusicStream(engine.m_ignisSound);

            for (const auto& hub : engine.m_regionHub) {
                if (hub.realm == engine.m_selectRealm) {
                    engine.m_player.setPosition({
                        hub.rectangle.x + (hub.rectangle.width / 2),
                        hub.rectangle.y + (hub.rectangle.height / 2)
                    });
                    break;
                }
            }
            engine.m_isMapWindowOpen = false;
        }
    }

    void renderRegionContent(GameEngine& engine, RegionHubG* currentHub) {
        Color currentRegionColor = WHITE;
        for (const auto& region : engine.m_regions) {
            if (region.realm == engine.m_selectRealm){
                currentRegionColor = region.color;
                break;
            }
        }

        for (const auto& zone : currentHub->zones) {
            DrawRectangleRec(zone.bounds, zone.color);
            DrawText(zone.name.c_str(), zone.bounds.x + 10, zone.bounds.y + 10, 12, WHITE);
        }

        for (const auto& regionBldg : engine.m_buildings){
            if (regionBldg.realm == engine.m_selectRealm){
                for (auto& bldg : regionBldg.buildings){
                    DrawRectangleRec(bldg.interior, bldg.floor);
                    DrawRectangleLinesEx(bldg.interior, 1, DARKGRAY);

                    if (!bldg.isPlayerInside) {
                        DrawRectangleRec(bldg.building, bldg.roof);
                        DrawRectangleLinesEx(bldg.building, 2, BLACK);
                        DrawRectangleRec(bldg.entrance, bldg.door);
                        DrawRectangleLinesEx(bldg.entrance, 1, BLACK);
                        DrawText(bldg.name.c_str(), bldg.building.x + 10, bldg.building.y + 10, 14, WHITE);
                    } else {
                        DrawText(bldg.name.c_str(), bldg.interior.x + 10, bldg.interior.y + 10, 14, DARKGRAY);
                    }
                    for (const auto& wall : bldg.walls) {
                        DrawRectangleRec(wall.bounds, RED);
                    }
                }
            }
        }

        DrawRectangleRec(currentHub->rectangle, ColorAlpha(currentRegionColor, 0.2f));
        DrawRectangleLinesEx(currentHub->rectangle, 2, currentRegionColor);
        DrawText(currentHub->name.c_str(), currentHub->rectangle.x + 20, currentHub->rectangle.y + 20, 20, WHITE);
        
        Vector2 playerPos = engine.m_player.getPosition();
        
        for (const auto& wItem : engine.m_worldItems) {
            if (!wItem.isCollected() && wItem.getRealm() == engine.m_selectRealm) {
                DrawCircleV(wItem.getPosition(), 6.0f, GOLD);
                DrawCircleLines(wItem.getPosition().x, wItem.getPosition().y, 8.0f, WHITE);
                DrawText(wItem.getName().c_str(), wItem.getPosition().x - 25, wItem.getPosition().y - 20, 10, YELLOW);
                if (Vector2Distance(playerPos, wItem.getPosition()) < 35.0f) {
                    DrawText("Press [E] to pick up", wItem.getPosition().x - 45, wItem.getPosition().y - 35, 10, GREEN);
                }
            }
        }

        for (const auto& regionBldg : engine.m_buildings) {
            if (regionBldg.realm == engine.m_selectRealm) {
                for (const auto& bldg : regionBldg.buildings) {
                    if (bldg.isPlayerInside) {
                        for (const auto& npcs : currentHub->npcs) {
                            if (npcs.npcId == bldg.npc) {
                                Vector2 pos = npcs.npcGraphics.npcPos;
                                Vector2 size = npcs.npcGraphics.npcRec;
                                Color col = npcs.npcGraphics.npcColor;
                                
                                Rectangle visualRec = { pos.x - (size.x / 2.0f), pos.y - (size.y / 2.0f), size.x, size.y };
                                Rectangle collisionRec = { pos.x - (npcs.npcCollision.x / 2.0f), pos.y - (npcs.npcCollision.y / 2.0f), npcs.npcCollision.x, npcs.npcCollision.y };

                                bool isPlayerInRange = CheckCollisionPointRec(playerPos, collisionRec);

                                DrawRectangleRec(visualRec, col);
                                DrawRectangleLinesEx(visualRec, 1, BLACK);
                                DrawText(npcs.name.c_str(), pos.x - (MeasureText(npcs.name.c_str(), 12) / 2), visualRec.y - 18, 12, WHITE);

                                if (isPlayerInRange) {
                                    DrawText("Press [F] to talk", pos.x - 45, visualRec.y - 35, 10, YELLOW);
                                }
                            }
                        }
                    }
                }
            }
        }

        for (const auto& enemy : currentHub->enemies) {
            if (!enemy.isDefeated()){
                Rectangle enemyRec = enemy.getCollisionBox();
                DrawRectangleRec(enemyRec, enemy.getColor());
                DrawRectangleLinesEx(enemyRec, 1, RED);
                DrawText(enemy.getName().c_str(), enemy.getPosition().x - (MeasureText(enemy.getName().c_str(), 10) / 2), enemyRec.y - 15, 10, WHITE);
                if (CheckCollisionPointRec(playerPos, enemyRec)) {
                    DrawText("Press [SPACE] to fight", enemy.getPosition().x - 55, enemyRec.y - 30, 10, RED);
                }
            }
        }
        
        DrawRectangleRec({playerPos.x, playerPos.y, 10.0f, 15.0f}, YELLOW);
    }

    void renderMinimapContent(GameEngine& engine, RegionHubG* currentHub) {
        float mmWidth = 200.0f;
        float mmHeight = 150.0f;
        float mmX = engine.SCREENSIZEX - mmWidth - 20.0f;
        float mmY = 20.0f;

        DrawRectangle(mmX, mmY, mmWidth, mmHeight, Fade(BLACK, 0.7f));
        DrawRectangleLines(mmX, mmY, mmWidth, mmHeight, WHITE);
        DrawText("MINIMAP", mmX + 10, mmY + 10, 10, LIGHTGRAY);

        BeginScissorMode(static_cast<int>(mmX), static_cast<int>(mmY), static_cast<int>(mmWidth), static_cast<int>(mmHeight));
        if (currentHub && currentHub->rectangle.width > 0 && currentHub->rectangle.height > 0) {
            const RegionBuildings* currentRegionBuildings = nullptr;
            for (const auto& rb : engine.m_buildings) {
                if (rb.realm == engine.m_selectRealm) {
                    currentRegionBuildings = &rb;
                    break;
                }
            }

            if (currentRegionBuildings) {
                for (const auto& b : currentRegionBuildings->buildings) {
                    Rectangle bRec = b.building;
                    float normBX = (bRec.x - currentHub->rectangle.x) / currentHub->rectangle.width;
                    float normBY = (bRec.y - currentHub->rectangle.y) / currentHub->rectangle.height;
                    float normBW = bRec.width / currentHub->rectangle.width;
                    float normBH = bRec.height / currentHub->rectangle.height;

                    DrawRectangle(mmX + (normBX * mmWidth), mmY + (normBY * mmHeight), normBW * mmWidth, normBH * mmHeight, b.roof);
                }
            }

            for (const auto& enemy : currentHub->enemies) {
                if (!enemy.isDefeated()) {
                    float eNormX = (enemy.getPosition().x - currentHub->rectangle.x) / currentHub->rectangle.width;
                    float eNormY = (enemy.getPosition().y - currentHub->rectangle.y) / currentHub->rectangle.height;
                    DrawCircle(mmX + (eNormX * mmWidth), mmY + (eNormY * mmHeight), 3.0f, ORANGE);
                }
            }

            Vector2 playerPos = engine.m_player.getPosition();
            float normX = (playerPos.x - currentHub->rectangle.x) / currentHub->rectangle.width;
            float normY = (playerPos.y - currentHub->rectangle.y) / currentHub->rectangle.height;
            DrawCircle(mmX + (normX * mmWidth), mmY + (normY * mmHeight), 4.0f, RED);
        }
        EndScissorMode();
    }

    void renderHubControlsContent(GameEngine& engine) {
        float boxWidth = 600.0f, boxHeight = 35.0f;
        float boxX = (engine.SCREENSIZEX - boxWidth) / 2.0f, boxY = engine.SCREENSIZEY - 45.0f;
        DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(BLACK, 0.3f));
        DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 1, DARKGRAY);
        DrawText("[WASD] Move | [E] Collect | [F] Interact | [I] Inventory | [J] Quests | [Q] Map | [F5] Save", boxX + 15, boxY + 11, 12, WHITE);
    }

    void renderFastTravelWindow(GameEngine& engine) {
        DrawRectangle(0, 0, engine.SCREENSIZEX, engine.SCREENSIZEY, Fade(BLACK, 0.6f));
        float boxWidth = 500.0f, boxHeight = 400.0f;
        float boxX = (engine.SCREENSIZEX - boxWidth) / 2.0f, boxY = (engine.SCREENSIZEY - boxHeight) / 2.0f;

        DrawRectangle(boxX, boxY, boxWidth, boxHeight, DARKGRAY);
        DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, WHITE);
        DrawText("Fast Travel - Select a Region", boxX + 40, boxY + 20, 20, WHITE);

        float cellWidth = 200.0f, cellHeight = 120.0f;
        float startX = boxX + 45.0f, startY = boxY + 85.0f;

        for (int row = 0; row < 2; row++) {
            for (int col = 0; col < 2; col++) {
                int index = row * 2 + col;
                if (index >= engine.m_regions.size()) continue;

                const auto& region = engine.m_regions[index];
                bool isSelected = (engine.m_mapRow == row && engine.m_mapCol == col);
                float x = startX + col * (cellWidth + 10.0f);
                float y = startY + row * (cellHeight + 10.0f);

                DrawRectangle(x, y, cellWidth, cellHeight, isSelected ? GOLD : LIGHTGRAY);
                DrawRectangleLines(x, y, cellWidth, cellHeight, BLACK);
                DrawText(region.name.c_str(), x + 15, y + 20, 18, isSelected ? BLACK : DARKGRAY);
                if (isSelected) DrawText("[ SELECTED ]", x + 15, y + 80, 14, RED);
            }
        }
    }
};

#endif