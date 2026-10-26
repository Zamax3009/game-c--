#include "GameEngine.h"
#include "raylib.h"
#include "raymath.h"
#include "imgui.h"

void GameEngine::render() {
    BeginDrawing();
    ClearBackground(DARKGRAY);
    
    BeginMode2D(m_camera);
    if (m_currentState == GameState::RegionHub) {
        renderRegion();
    }
    else if (m_currentState == GameState::WorldMap) {
        renderMap();
    }
    EndMode2D();

    if (m_currentState == GameState::RegionHub) {
        if (m_isMapWindowOpen){
            renderMapWindow();
        } else {
            renderMinimap();
        }
        
    }
    else if (m_currentState == GameState::Dialogue) {
        renderUI();
    }
    else if (m_currentState == GameState::Inventory) {
        renderInventory();
    }
    else if (m_currentState == GameState::Combat) {
        renderCombat();
    }

    EndDrawing();
}

void GameEngine::renderCombat() {
    // BACKGROUND

    DrawRectangle(0, 0, 1280, 720, Fade(BLACK, 0.75f));

    if (!m_activeEnemy) return;

    float arenaWidth = 700.0f;
    float arenaHeight = 450.0f;
    float arenaX = (1280.0f - arenaWidth) / 2.0f;
    float arenaY = (720.0f - arenaHeight) / 2.0f;

    DrawRectangle(arenaX, arenaY, arenaWidth, arenaHeight, DARKGRAY);
    DrawRectangleLinesEx({arenaX, arenaY, arenaWidth, arenaHeight}, 2, RED);
    DrawText("COMBAT", arenaX + 30, arenaY + 25, 20, RED);

    DrawText(m_activeEnemy->getName().c_str(), arenaX + 400, arenaY + 40, 20, RED);
    DrawText(m_player.getName().c_str(), arenaX + 50, arenaY + 40, 20, GOLD);

    // ENEMY (RIGHT SIDE)
    if (m_currentPhase == CombatPhase::ActionSelect) {
        std::string enemyHP = "HP: " + std::to_string(m_activeEnemy->getHealth()) + " / " + std::to_string(m_activeEnemy->getMaxHealth());
        DrawText(enemyHP.c_str(), arenaX + 400, arenaY + 120, 16, LIGHTGRAY);

        float barWidth = 220.0f;
        float hpPercent = static_cast<float>(m_activeEnemy->getHealth()) / static_cast<float>(m_activeEnemy->getMaxHealth());

        DrawRectangle(arenaX + 400, arenaY + 150, barWidth, 18, BLACK);
        DrawRectangle(arenaX + 400, arenaY + 150, barWidth * hpPercent, 18, RED);
        DrawRectangleLines(arenaX + 400, arenaY + 150, barWidth, 18, WHITE);

        // PLAYER (LEFT SIDE)
        DrawText(m_player.getName().c_str(), arenaX + 50, arenaY + 80, 22, GREEN);

        std::string playerHPText = "HP: " + std::to_string(m_player.getHP()) + " / " + std::to_string(m_player.getMaxHP());
        DrawText(playerHPText.c_str(), arenaX + 50, arenaY + 120, 16, LIGHTGRAY);

        float playerBarWidth = 220.0f;
        float playerHpPercent = static_cast<float>(m_player.getHP()) / static_cast<float>(m_player.getMaxHP());
        DrawRectangle(arenaX + 50, arenaY + 150, playerBarWidth, 18, BLACK);
        DrawRectangle(arenaX + 50, arenaY + 150, playerBarWidth * playerHpPercent, 18, GREEN);
        DrawRectangleLines(arenaX + 50, arenaY + 150, playerBarWidth, 18, WHITE);

        // CONTROLS
        const auto& attacks = m_player.getAttacks();
        int attackCount = static_cast<int>(attacks.size());
        
        float controlBoxHeight = std::max(120.0f, static_cast<float>(attackCount * 25 + 50));
        DrawRectangle(arenaX + 50, arenaY + 260, 600, 120, Fade(BLACK, 0.5f));
        DrawRectangleLines(arenaX + 50, arenaY + 260, 600, 120, WHITE);

        int offsetY = 275;
        int index = 1;
        for (const auto& atk : m_player.getAttacks()) {
            std::string label = "[" + std::to_string(index) + "]" + atk.name + "(Damage: " + std::to_string(atk.damage) + " | Healing: " + std::to_string(atk.healing) + ")";
            DrawText(label.c_str(), arenaX + 70, arenaY + offsetY, 14, GRAY);
            offsetY += 25;
            index++;
        }

        DrawText("Press [R] to Run", arenaX + 370, arenaY + 300, 16, YELLOW);
    }
    else if (m_currentPhase == CombatPhase::Victory) {
        DrawRectangle(arenaX + 50, arenaY + 120, 600, 130, Fade(DARKGREEN, 0.9f));
        DrawRectangleLines(arenaX+ 50, arenaY + 120, 600, 130, GREEN);

        DrawText("VICTORY!", arenaX + 250, arenaY + 140, 24, GOLD);
        std::string lootMsg = "You obtained " + std::to_string(m_lastLootGold) + " gold coins!";
        DrawText(lootMsg.c_str(), arenaX + 190, arenaY + 175, 16, WHITE);
        DrawText("Press [ENTER] to continue...", arenaX + 180, arenaY + 210, 14, LIGHTGRAY);
    }
    else if (m_currentPhase == CombatPhase::Defeat) {
        DrawRectangle(arenaX + 50, arenaY + 120, 600, 130, Fade(MAROON, 0.9f));
        DrawRectangleLines(arenaX + 50, arenaY + 120, 600, 130, RED);

        DrawText("DEFEAT!", arenaX + 180, arenaY + 140, 24, RED);
        DrawText("Your journey doesn't end here! Resume!", arenaX + 150, arenaY + 175, 16, WHITE);
        DrawText("Press [ENTER] to wake up...", arenaX + 180, arenaY + 210, 14, LIGHTGRAY);
    }
}

void GameEngine::renderInventory() {
    DrawRectangle(0, 0, 1280, 720, Fade(BLACK, 0.75f));

    float invWidth = 600.0f;
    float invHeight = 450.0f;
    float invX = (1280.0f - invWidth) / 2.0f;
    float invY = (720.0f - invHeight) / 2.0f;

    DrawRectangle(invX, invY, invWidth, invHeight, DARKGRAY);
    DrawRectangleLines(invX, invY, invWidth, invHeight, WHITE);
    DrawText("INVENTORY", invX + 30, invY + 25, 22, GOLD);
    DrawText("Press [I] or [ESC] to close", invX + 30, invY + 55, 12, LIGHTGRAY);

    float startY = invY + 100.0f;
    if (m_inventory.empty()) {
        DrawText("Inventory is empity", invX + 30, startY, 16, WHITE);
    } else {
        for (size_t i = 0; i < m_inventory.size(); i++) {
            const auto& item = m_inventory[i];

            float itemY = startY + (i * 40.0f);
            DrawRectangle(invX + 25, itemY, invWidth - 50, 32, Fade(BLACK, 0.3f));

            std::string itemText = item.name + "  x" + std::to_string(item.quantity);
            DrawText(itemText.c_str(), invX + 40, itemY + 8, 14, WHITE);
            DrawText(item.description.c_str(), invX + 250, itemY + 8, 12, LIGHTGRAY);
        }
    }
}

void GameEngine::renderMap() {
    Vector2 playerPos = m_player.getPosition();
    DrawText("World map: choose a destination", 450, 40, 20, WHITE);
    for (const auto& region : m_regions){
        bool isPlayerInside = CheckCollisionPointRec(playerPos, region.bounds);

        renderColor = region.color;
        if (isPlayerInside){
            renderColor = ColorAlpha(region.color, 0.6f);
        }

        DrawRectangleRec(region.bounds, renderColor);
        
        Color borderColor = isPlayerInside ? WHITE : BLACK;
        int borderWidth = isPlayerInside ? 3 : 1;
        
        DrawRectangleLinesEx(region.bounds, borderWidth, borderColor);

        DrawText(region.name.c_str(), region.bounds.x + 15, region.bounds.y + 15, 14, WHITE);

                if (isPlayerInside) {
            DrawText("Press [F] to enter...", region.bounds.x + 20, region.bounds.y + 60, 14, LIGHTGRAY);
        }
    }

    DrawRectangleV(playerPos, {10.0, 15.0}, YELLOW);
}

void GameEngine::renderMapWindow() {
    DrawRectangle(0, 0, 1280, 720, Fade(BLACK, 0.6f));

    float boxWidth = 500.0f;
    float boxHeight = 400.0f;
    float boxX = (1280.0f - boxWidth) / 2.0f;
    float boxY = (720.0f - boxHeight) / 2.0f;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, DARKGRAY);
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, WHITE);

    DrawText("Fast Travel - Select a Region", boxX + 40, boxY + 20, 20, WHITE);
    DrawText("Use WASD to move, Press [F] to travel", boxX + 40, boxY + 50, 12, LIGHTGRAY);

    float cellWidth = 200.0f;
    float cellHeight = 120.0f;
    float startX = boxX + 45.0f;
    float startY = boxY + 85.0f;

    for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 2; col++) {
            int index = row * 2 + col;
            if (index >= m_regions.size()) continue;

            const auto& region = m_regions[index];
            bool isSelected = (m_mapRow == row && m_mapCol == col);

            float x = startX + col * (cellWidth + 10.0f);
            float y = startY + row * (cellHeight + 10.0f);

            Color cellColor = isSelected ? GOLD : LIGHTGRAY;
            Color textcolor = isSelected ? BLACK : DARKGRAY;

            DrawRectangle(x, y, cellWidth, cellHeight, cellColor);
            DrawRectangleLines(x, y, cellWidth, cellHeight, BLACK);

            DrawText(region.name.c_str(), x + 15, y + 20, 18, textcolor);

            if (isSelected) {
                DrawText("[ SELECTED ]", x + 15, y + 80, 14, RED);
            }
        }
    }
}

void GameEngine::renderMinimap() {
    float mmWidth = 200.0f;
    float mmHeight = 150.0f;
    float mmX = 1280.0f - mmWidth - 20.0f;
    float mmY = 20.0f;

    DrawRectangle(mmX, mmY, mmWidth, mmHeight, Fade(BLACK, 0.7f));
    DrawRectangleLines(mmX, mmY, mmWidth, mmHeight, WHITE);
    DrawText("MINIMAP", mmX + 10, mmY + 10, 10, LIGHTGRAY);

    BeginScissorMode(static_cast<int>(mmX), static_cast<int>(mmY), static_cast<int>(mmWidth), static_cast<int>(mmHeight));

    RegionHubG* currentHub = nullptr;
    for (auto& hub : m_regionHub) {
        if (hub.realm == m_selectRealm) {
            currentHub = &hub;
            break;
        }
    }
    if (currentHub && currentHub->rectangle.width > 0 && currentHub->rectangle.height > 0) {
        const RegionBuildings* currentRegionBuildings = nullptr;
        for (const auto& rb : m_buildings) {
            if (rb.realm == m_selectRealm) {
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

                float bDrawX = mmX + (normBX * mmWidth);
                float bDrawY = mmY + (normBY * mmHeight);
                float bDrawW = normBW * mmWidth;
                float bDrawH = normBH * mmHeight;

                DrawRectangle(bDrawX, bDrawY, bDrawW, bDrawH, b.roof);

                float normEntX = (b.entrance.x - currentHub->rectangle.x) / currentHub->rectangle.width;
                float normEntY = (b.entrance.y - currentHub->rectangle.y) / currentHub->rectangle.height;
                float normEntW = b.entrance.width / currentHub->rectangle.width;
                float normEntH = b.entrance.height / currentHub->rectangle.height;

                float drawEntX = mmX + (normEntX * mmWidth);
                float drawEntY = mmY + (normEntY * mmHeight);
                float drawEntW = normEntW * mmWidth;
                float drawEntH = normEntH * mmHeight;

                DrawRectangle(drawEntX, drawEntY, drawEntW, drawEntH, b.door);
            }
        }

        for (const auto& npc : currentHub->npcs) {
            float nNormX = (npc.npcGraphics.npcPos.x - currentHub->rectangle.x) / currentHub->rectangle.width;
            float nNormY = (npc.npcGraphics.npcPos.y - currentHub->rectangle.y) / currentHub->rectangle.height;
            float nDotX = mmX + (nNormX * mmWidth);
            float nDotY = mmY + (nNormY * mmWidth) - 21;

            DrawCircle(nDotX, nDotY, 3.0f, BLUE);
        }

        for (const auto& enemy : currentHub->enemies) {
            if (!enemy.isDefeated()) {
                float eNormX = (enemy.getPosition().x - currentHub->rectangle.x) / currentHub->rectangle.width;
                float eNormY = (enemy.getPosition().y - currentHub->rectangle.y) / currentHub->rectangle.height;
                float eDotX = mmX + (eNormX * mmWidth);
                float eDotY = mmY + (eNormY * mmHeight);

                DrawCircle(eDotX, eDotY, 3.0f, ORANGE);
            }
        }

        Vector2 playerPos = m_player.getPosition();
        float normX = (playerPos.x - currentHub->rectangle.x) / currentHub->rectangle.width;
        float normY = (playerPos.y - currentHub->rectangle.y) / currentHub->rectangle.height;
        float dotX = mmX + (normX * mmWidth);
        float dotY = mmY + (normY * mmHeight);

        DrawCircle(dotX, dotY, 4.0f, RED);
    }
    EndScissorMode();
}

void GameEngine::renderRegion() {
    Color currentRegionColor = WHITE;
    for (const auto& region : m_regions) {
        if (region.realm == m_selectRealm){
            currentRegionColor = region.color;
            break;
        }
    }

    RegionHubG* currentHub = nullptr;
    for (auto& hub : m_regionHub) {
        if (hub.realm == m_selectRealm) {
            currentHub = &hub;
            break;
        }
    }
    if (!currentHub) return;

    // RENDER BUILDINGS
    for (const auto& regionBldg : m_buildings){
        if (regionBldg.realm == m_selectRealm){
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
    
    Vector2 playerPos = m_player.getPosition();

    for (const auto& wItem : m_worldItems) {
        if (!wItem.isCollected && wItem.realm == m_selectRealm) {
            DrawCircleV(wItem.position, 6.0f, GOLD);
            DrawCircleLines(wItem.position.x, wItem.position.y, 8.0f, WHITE);
            DrawText(wItem.name.c_str(), wItem.position.x -25, wItem.position.y - 20, 10, YELLOW);
            if (Vector2Distance(playerPos, wItem.position) < 35.0f) {
                DrawText("Press [E] to pick up", wItem.position.x -45, wItem.position.y - 35, 10, GREEN);
            }
        }
    }

    // RENDER NPCs ONLY INSIDE BUILDINGS
    for (const auto& regionBldg : m_buildings) {
        if (regionBldg.realm == m_selectRealm) {
            for (const auto& bldg : regionBldg.buildings) {
                if (bldg.isPlayerInside) {
                    for (const auto& npcs : currentHub->npcs) {
                        if (npcs.npcId == bldg.npc) {
                            Vector2 pos = npcs.npcGraphics.npcPos;
                            Vector2 size = npcs.npcGraphics.npcRec;
                            Color col = npcs.npcGraphics.npcColor;
                            
                            Rectangle visualRec = {
                                pos.x - (size.x / 2.0f),
                                pos.y - (size.y / 2.0f),
                                size.x,
                                size.y
                            };
                            Rectangle collisionRec = {
                                pos.x - (npcs.npcCollision.x / 2.0f),
                                pos.y - (npcs.npcCollision.y / 2.0f),
                                npcs.npcCollision.x,
                                npcs.npcCollision.y
                            };

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
            Rectangle enemyRec = {
                enemy.getPosition().x - (enemy.getCollisionBox().x / 2.0f),
                enemy.getPosition().y - (enemy.getCollisionBox().y / 2.0f),
                enemy.getCollisionBox().x,
                enemy.getCollisionBox().y
            };

            DrawRectangleRec(enemyRec, enemy.getColor());
            DrawRectangleLinesEx(enemyRec, 1, RED);
            DrawText(enemy.getName().c_str(), enemy.getPosition().x - (MeasureText(enemy.getName().c_str(), 10) / 2), enemyRec.y - 15, 10, WHITE);
            if (CheckCollisionPointRec(playerPos, enemyRec)) {
                DrawText("Press [SPACE] to fight", enemy.getPosition().x - 55, enemyRec.y - 30, 10, RED);
            }
        }
    }
    
    DrawRectangleV(playerPos, {10.0f, 15.0f}, YELLOW);
}

void GameEngine::renderUI() {
    //Dialogue
    const DialogueNode* node = m_dialogueManager.getCurrentNode();
    if (node) {
        ImGui::Begin("Dialogue", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::TextDisabled("[%s]", node->speaker.c_str());
        ImGui::TextWrapped("%s", node->text.c_str());
        ImGui::Separator();

        for (size_t i = 0; i < node->options.size(); i++) {
            if (ImGui::Button(node->options[i].text.c_str())) {
                m_dialogueManager.selectOption(static_cast<int>(i), m_player);
            }
        }
        ImGui::End();
    }
}