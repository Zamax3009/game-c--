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

        if (IsKeyPressed(KEY_I)) {
            engine.goToInventory();
            return;
        }

        if (IsKeyPressed(KEY_J)) {
            engine.goToQuestLog();
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
        renderStaminaBar(engine);
        EndMode2D();

        if (engine.m_isMapWindowOpen) {
            renderFastTravelWindow(engine);
        } else {
            renderMinimapContent(engine, currentHub);
            renderHubControlsContent(engine);
            renderHotbar(engine);
        }
    }

private:
    void updateActiveHub(GameEngine& engine, RegionHubG* currentHub) {
        engine.playerMovement(4.0f);

        SubArea* currentSubArea = nullptr;
        for (auto& sa : currentHub->areas) {
            if (sa.id == currentHub->currenSubAreaId) {
                currentSubArea = &sa;
                break;
            }
        }
        if (!currentSubArea) return;

        engine.updateCamera(currentSubArea);

        engine.updateRealtimeCombat(currentSubArea);

        Vector2 playerPos = engine.m_player.getPosition();

        std::vector<WallCollider> allWalls;
        for (const auto& rb : engine.m_buildings) {
            if (rb.realm == engine.m_selectRealm) {
                for (const auto& bldg : rb.buildings) {
                    if (bldg.areaID != currentSubArea->id) continue;
                    for (const auto& w : bldg.walls) {
                        allWalls.push_back(w);
                    }
                }
            }
        }
        for (auto& enemy : currentSubArea->enemies) {
            enemy.updateAI(playerPos, allWalls, currentSubArea->bounds, engine.m_player);
        }

        if (IsKeyPressed(KEY_F)) {
            for (const auto& rb : engine.m_buildings) {
                if (rb.realm == engine.m_selectRealm) {
                    for ( const auto& bldg : rb.buildings) {
                        if (bldg.areaID != currentSubArea->id) continue;
                        if (!bldg.isPlayerInside) continue;
                        if (bldg.npc == "gwen") {
                            engine.goToShop();
                        } else {
                            std::string realName = "Aer";
                            if (engine.m_selectRealm == Realm::Aqua) realName = "Aqua";
                            else if (engine.m_selectRealm == Realm::Terra) realName = "Terra";
                            else if (engine.m_selectRealm == Realm::Ignis) realName = "Ignis";

                            std::string dialogueFile = "assets/" + realName + "Dialogues.json";
                            engine.m_dialogueManager.loadFromFile(dialogueFile, bldg.npc);
                            engine.m_dialogueManager.startDialogue(0);

                            if (engine.m_dialogueManager.isDialogueActive()) {
                                engine.goToDialogue();
                            }
                            return;
                        }
                        return;
                    }
                }
            }
        }

        if (IsKeyPressed(KEY_E)) {
            for (auto& wItem : currentSubArea->items) {
                if (!wItem.isCollected() && Vector2Distance(playerPos, wItem.getPosition()) < 35.0f) {
                    wItem.setCollected(true);
                    engine.m_inventory.addItem(engine.createItem(wItem.getID(), wItem.getQuantity()));
                    engine.showNotification("Picked up: " + wItem.getName());
                    break;
                }
            }
        }

        switch (currentHub->currenSubAreaId)
        {
        case AreaID::Village:
            if (playerPos.x > currentSubArea->bounds.x + currentSubArea->bounds.width - 20.0f) {
                currentHub->currenSubAreaId = AreaID::Capital;
                engine.m_player.setPosition({ currentSubArea->bounds.x + 40.0f, playerPos.y });
            }
            break;
        case AreaID::Capital:
            if (playerPos.x < currentSubArea->bounds.x + 20.0f) {
                currentHub->currenSubAreaId = AreaID::Village;
                engine.m_player.setPosition({ currentSubArea->bounds.x + currentSubArea->bounds.width - 40.0f, playerPos.y });
            } else if (playerPos.x > currentSubArea->bounds.x + currentSubArea->bounds.width - 20) {
                currentHub->currenSubAreaId = AreaID::ScorchedSteppes;
                engine.m_player.setPosition({ currentSubArea->bounds.x + 40.0f, playerPos.y });
                engine.showNotification("Challenge Started: Clear the Scorched Steppes!");
            }
            break;
        case AreaID::ScorchedSteppes: {
            bool allEnemiesDefeated = true;
            for (const auto& enemy : currentSubArea->enemies) {
                if (!enemy.isDefeated()) {
                    allEnemiesDefeated = false;
                    break;
                }
            }
            if (playerPos.x < currentSubArea->bounds.x + 20.0f) {
                currentHub->currenSubAreaId = AreaID::Capital;
                engine.m_player.setPosition({ currentSubArea->bounds.x + currentSubArea->bounds.width - 40.0f, playerPos.y });
            } else if (playerPos.x > currentSubArea->bounds.x + currentSubArea->bounds.width - 20.0f) {
                if (allEnemiesDefeated) {
                    currentHub->currenSubAreaId = AreaID::AncientTitanForge;
                    engine.m_player.setPosition({ currentSubArea->bounds.x + 40.0f, playerPos.y });
                    engine.showNotification("The path to the Ancient Titan Forge is open!");
                } else {
                    engine.m_player.setPosition({ currentSubArea->bounds.x + currentSubArea->bounds.width - 40.0f, playerPos.y });
                    engine.showNotification("Defeat all enemies in the arena first!");
                }
            }
            break;
        }
        case AreaID::AncientTitanForge:
            if (playerPos.x < currentSubArea->bounds.x + 20.0f) {
                currentHub->currenSubAreaId = AreaID::ScorchedSteppes;
                engine.m_player.setPosition({ currentSubArea->bounds.x + currentSubArea->bounds.width - 40.0f, playerPos.y });
            }
            break;
        default:
            break;
        }

        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
            engine.useHotbarItem(ItemType::Heal);
        }
        if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
            engine.useHotbarItem(ItemType::RestoreMana);
        }
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

            for (auto& hub : engine.m_regionHub) {
                if (hub.realm == engine.m_selectRealm) {

                    hub.currenSubAreaId = AreaID::Capital;

                    for (const auto& subArea : hub.areas) {
                       if (subArea.id == AreaID::Capital) {
                            engine.m_player.setPosition({
                                subArea.bounds.x + (subArea.bounds.width / 2),
                                subArea.bounds.y + (subArea.bounds.height / 2)
                            });
                            break;
                        }
                    }
                    break;
                }
            }
            engine.m_isMapWindowOpen = false;
        }
    }

    void renderRegionContent(GameEngine& engine, RegionHubG* currentHub) {
        SubArea* currentSubArea = nullptr;
        for (auto& sa : currentHub->areas) {
            if (sa.id == currentHub->currenSubAreaId) {
                currentSubArea = &sa;
            }
        }
        if (!currentSubArea) return;
        
        DrawRectangleRec(currentSubArea->bounds, ColorAlpha(currentSubArea->backgroundColor, 0.2f));
        DrawRectangleLinesEx(currentSubArea->bounds, 2, currentSubArea->backgroundColor);
        DrawText(currentHub->name.c_str(), currentSubArea->bounds.x + 20, currentSubArea->bounds.y + 20, 20, WHITE);
        
        for (const auto& regionBldg : engine.m_buildings) {
            if (regionBldg.realm == engine.m_selectRealm) {
                for (auto& bldg : regionBldg.buildings) {
                    if (bldg.areaID != currentSubArea->id) continue;

                    DrawRectangleRec(bldg.interior, bldg.floor);
                    DrawRectangleLinesEx(bldg.interior, 1, DARKGRAY);

                    if (!bldg.isPlayerInside) {
                        DrawRectangleRec(bldg.building, bldg.roof);
                        DrawRectangleLinesEx(bldg.building, 2, BLACK);
                        DrawRectangleRec(bldg.entrance, bldg.door);
                        DrawRectangleLinesEx(bldg.entrance, 1, BLACK);
                        DrawText(bldg.name.c_str(), bldg.interior.x + 10, bldg.interior.y + 10, 14, WHITE);
                    } else {
                        DrawText(bldg.name.c_str(), bldg.interior.x + 10, bldg.interior.y + 10, 14, DARKGRAY);
                    }
                }
            }
        }

        Vector2 playerPos = engine.m_player.getPosition();

        for (const auto& wItem : currentSubArea->items) {
            if (!wItem.isCollected()) {
                DrawCircleV(wItem.getPosition(), 6.0f, GOLD);
                DrawCircleLines(wItem.getPosition().x, wItem.getPosition().y, 8.0f, WHITE);
                DrawText(wItem.getName().c_str(), wItem.getPosition().x - 25, wItem.getPosition().y - 20, 10, YELLOW);
                if (Vector2Distance(playerPos, wItem.getPosition()) < 35.0f) {
                    DrawText("Press [E] to pick up", wItem.getPosition().x - 45, wItem.getPosition().y - 35, 10, GREEN);
                }
            }
        }
        if (currentSubArea->id == AreaID::Capital) {
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
        }
        for (const auto& enemy : currentSubArea->enemies) {
            if (!enemy.isDefeated()){
                Rectangle enemyRec = enemy.getCollisionBox();
                DrawRectangleRec(enemyRec, enemy.getColor());
                DrawRectangleLinesEx(enemyRec, 1, RED);
                DrawText(enemy.getName().c_str(), enemy.getPosition().x - (MeasureText(enemy.getName().c_str(), 10) / 2), enemyRec.y - 15, 10, WHITE);
                if (CheckCollisionPointRec(playerPos, enemyRec)) {
                    DrawText("Press [K] to fight", enemy.getPosition().x - 55, enemyRec.y - 30, 10, RED);
                }
            }
        }

        float rightEdge = currentSubArea->bounds.x + currentSubArea->bounds.width;
        float leftEdge = currentSubArea->bounds.x;
        float centerY = currentSubArea->bounds.y + (currentSubArea->bounds.height / 2.0f);
        
        if (currentSubArea->id == AreaID::Village) {
            DrawText("-> Capital", rightEdge - 110, centerY - 10, 14, RAYWHITE);
        }
        else if (currentSubArea->id == AreaID::Capital) {
            DrawText("<- Village", leftEdge + 40, centerY - 10, 14, RAYWHITE);

            DrawText("-> Scorched Steppes (Arena)", rightEdge - 210, centerY - 10, 14, RAYWHITE);
        }
        else if (currentSubArea->id == AreaID::ScorchedSteppes) {
            DrawText("<- Capital", leftEdge + 40, centerY - 10, 14, RAYWHITE);

            bool allCleared = true;
            for (const auto& enemy : currentSubArea->enemies) {
                if (!enemy.isDefeated()) {
                    allCleared = false;
                    break;
                }
            }

            Color forgeColor = allCleared ? GOLD : RED;
            std::string forgeText = allCleared ? "-> Ancient Titan Forge [OPEN]" : "-> Forge [LOCKED: Clear Arena]";
            DrawText(forgeText.c_str(), rightEdge - (allCleared ? 230 : 250), centerY - 10, 14, forgeColor);
        }
        else if (currentSubArea->id == AreaID::AncientTitanForge) {
            DrawText("<- Scorched Steppes", leftEdge + 40, centerY - 10, 14, RAYWHITE);
        }

        DrawRectangleRec({playerPos.x, playerPos.y, 10.0f, 15.0f}, YELLOW);

        if (engine.m_player.getIsAttacking()) {
            Rectangle attackBox = engine.m_player.getAttackHitbox();
            DrawRectangleRec(attackBox, Fade(YELLOW, 0.5f));
            DrawRectangleLinesEx(attackBox, 1, RED);
        }

        for (const auto& ft : engine.m_floatingTexts) {
            Color fadeColor = Fade(ft.color, ft.lifeTimer); 
            DrawText(ft.text.c_str(), ft.position.x - 10, ft.position.y - 20, 16, fadeColor);
        }
    }

    void renderMinimapContent(GameEngine& engine, RegionHubG* currentHub) {
        SubArea* currentSubArea = nullptr;
        for (auto& sa : currentHub->areas) {
            if (sa.id == currentHub->currenSubAreaId) {
                currentSubArea = &sa;
            }
        }
        if (!currentSubArea) return;

        float mmWidth = 200.0f;
        float mmHeight = 150.0f;
        float mmX = engine.SCREENSIZEX - mmWidth - 20.0f;
        float mmY = 20.0f;

        DrawRectangle(mmX, mmY, mmWidth, mmHeight, Fade(BLACK, 0.7f));
        DrawRectangleLines(mmX, mmY, mmWidth, mmHeight, WHITE);
        DrawText("MINIMAP", mmX + 10, mmY + 10, 10, LIGHTGRAY);

        BeginScissorMode(static_cast<int>(mmX), static_cast<int>(mmY), static_cast<int>(mmWidth), static_cast<int>(mmHeight));
        if (currentSubArea && currentSubArea->bounds.width > 0 && currentSubArea->bounds.height > 0) {
            const RegionBuildings* currentRegionBuildings = nullptr;
            for (const auto& rb : engine.m_buildings) {
                if (rb.realm == engine.m_selectRealm) {
                    currentRegionBuildings = &rb;
                    break;
                }
            }

            if (currentRegionBuildings) {
                for (const auto& b : currentRegionBuildings->buildings) {
                    if (b.areaID != currentSubArea->id) continue;
                    Rectangle bRec = b.building;
                    float normBX = (bRec.x - currentSubArea->bounds.x) / currentSubArea->bounds.width;
                    float normBY = (bRec.y - currentSubArea->bounds.y) / currentSubArea->bounds.height;
                    float normBW = bRec.width / currentSubArea->bounds.width;
                    float normBH = bRec.height / currentSubArea->bounds.height;

                    DrawRectangle(mmX + (normBX * mmWidth), mmY + (normBY * mmHeight), normBW * mmWidth, normBH * mmHeight, b.roof);
                }
            }

            for (const auto& enemy : currentSubArea->enemies) {
                if (!enemy.isDefeated()) {
                    float eNormX = (enemy.getPosition().x - currentSubArea->bounds.x) / currentSubArea->bounds.width;
                    float eNormY = (enemy.getPosition().y - currentSubArea->bounds.y) / currentSubArea->bounds.height;
                    DrawCircle(mmX + (eNormX * mmWidth), mmY + (eNormY * mmHeight), 3.0f, ORANGE);
                }
            }

            Vector2 playerPos = engine.m_player.getPosition();
            float normX = (playerPos.x - currentSubArea->bounds.x) / currentSubArea->bounds.width;
            float normY = (playerPos.y - currentSubArea->bounds.y) / currentSubArea->bounds.height;
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
    void renderStaminaBar(GameEngine& engine) {
        float visibilityTimer = engine.m_player.getStaminaVisibilityTimer();
        if (visibilityTimer <= 0.0f) return;

        Vector2 playerPos = engine.m_player.getPosition();
        
        float barWidth = 30.0f;
        float barHeight = 4.0f;
        float barX = playerPos.x + (10.0f / 2.0f) - (barWidth / 2.0f);
        float barY = playerPos.y - 10.0f;

        float staminaPct = engine.m_player.getStamina() / engine.m_player.getMaxStamina();
        
        float alpha = (visibilityTimer < 0.3f) ? (visibilityTimer / 0.3f) : 1.0f;

        DrawRectangle(barX, barY, barWidth, barHeight, Fade(BLACK, 0.6f * alpha));
        DrawRectangle(barX, barY, barWidth * staminaPct, barHeight, Fade(GREEN, alpha));
        DrawRectangleLinesEx({ barX, barY, barWidth, barHeight }, 1, Fade(DARKGREEN, alpha));
    }
    void renderHotbar(GameEngine& engine) {
        float slotSize = 44.0f;
        float spacing = 10.0f;
        int totalSlots = 2;
        float barWidth = (totalSlots * slotSize) + ((totalSlots - 1) * spacing);
        
        float startX = (engine.SCREENSIZEX - barWidth) / 2.0f;
        float startY = engine.SCREENSIZEY - 95.0f;

        int healthPotionQty = 0;
        int manaPotionQty = 0;

        for (const auto& item : engine.m_inventory.getItems()) {
            if (item.type == ItemType::Heal) healthPotionQty += item.quantity;
            if (item.type == ItemType::RestoreMana) manaPotionQty += item.quantity;
        }

        DrawRectangle(startX, startY, slotSize, slotSize, Fade(BLACK, 0.75f));
        DrawRectangleLinesEx({startX, startY, slotSize, slotSize}, 2, healthPotionQty > 0 ? RED : DARKGRAY);
        DrawText("[1]", startX + 4, startY + 4, 10, LIGHTGRAY);
        DrawText("HP", startX + 13, startY + 16, 14, RED);
        if (healthPotionQty > 0) {
            DrawText(std::to_string(healthPotionQty).c_str(), startX + slotSize - 14, startY + slotSize - 16, 12, WHITE);
        }

        float slot2X = startX + slotSize + spacing;
        DrawRectangle(slot2X, startY, slotSize, slotSize, Fade(BLACK, 0.75f));
        DrawRectangleLinesEx({slot2X, startY, slotSize, slotSize}, 2, manaPotionQty > 0 ? BLUE : DARKGRAY);
        DrawText("[2]", slot2X + 4, startY + 4, 10, LIGHTGRAY);
        DrawText("MP", slot2X + 11, startY + 16, 14, BLUE);
        if (manaPotionQty > 0) {
            DrawText(std::to_string(manaPotionQty).c_str(), slot2X + slotSize - 14, startY + slotSize - 16, 12, WHITE);
        }
    }
};

#endif