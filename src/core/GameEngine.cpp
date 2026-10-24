#include "GameEngine.h"
#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

GameEngine::GameEngine() : m_player("Pilgrim") {
    InitWindow(1280, 720, "Realms of Elementa - Engine");
    InitAudioDevice();
    SetTargetFPS(60);
    rlImGuiSetup(true);

    m_player.setPosition({ 1280.0f / 2.0f, 720.0f / 2.0f});
    
    m_aquaSound = LoadMusicStream("assets/Aqua_Main_Theme.mp3");
    m_ignisSound = LoadMusicStream("assets/Ignis_Main_Theme.mp3");
    m_currentState = GameState::WorldMap;

    m_camera.offset = { 1280.0f / 2.0f, 720.0f / 2.0f };
    m_camera.rotation = 0.0f;
    m_camera.zoom = 1.5f;

    m_regions = {
        {"Aer", Realm::Aer, { 0, 100, 640, 310 }, SKYBLUE},
        {"Aqua", Realm::Aqua, { 640, 410, 640, 310 }, BLUE},
        {"Terra", Realm::Terra, { 640, 100, 640, 310 }, GREEN},
        {"Ignis", Realm::Ignis, { 0, 410, 640, 310 }, ORANGE}
    };

    m_regionHub = {
        {
            "Aer", Realm::Aer, {
                {"Gwen", "Tavern owner", "assets/AerDialogues.json", "gwen", {50.0f, 55.0f}, {{10.0f, 15.0f}, {100.0f, 350.0f}, BROWN}},
                {"Aer Wizard", "Wise old Wizard, keeper of secrets", "assets/AerDialogues.json", "aer_wizard", {50.0f, 55.0f}, {{10.0f, 15.0f}, {1000.0f, 350.0f}, WHITE}}
            },
            {0, 100, 1280, 620}
        },
        {
            "Ignis", Realm::Ignis, {
                {"Gwen", "Tavern owner", "assets/IgnisDialogues.json", "gwen", {50.0f, 55.0f}, {{10.0f, 15.0f}, {100.0f, 350.0f}, BROWN}},
                {"Ignis Wizard", "Wise old Wizard, keeper of secrets", "assets/IgnisDialogues.json", "ignis_wizard", {50.0f, 55.0f}, {{10.0f, 15.0f}, {1000.0f, 350.0f}, WHITE}}
            },
            {0, 100, 1280, 620}
        },
        {
            "Aqua", Realm::Aqua, {
                {"Gwen", "Tavern owner", "assets/AquaDialogues.json", "gwen", {50.0f, 55.0f}, {{10.0f, 15.0f}, {100.0f, 350.0f}, BROWN}},
                {"Aqua Wizard", "Wise old Wizard, keeper of secrets", "assets/AquaDialogues.json", "aqua_wizard", {50.0f, 55.0f}, {{10.0f, 15.0f}, {1000.0f, 350.0f}, WHITE}}
            },
            {0, 100, 1280, 620}
        },
        {
            "Terra", Realm::Terra, {
                {"Gwen", "Tavern owner", "assets/TerraDialogues.json", "gwen", {50.0f, 55.0f}, {{10.0f, 15.0f}, {100.0f, 350.0f}, BROWN}},
                {"Terra Wizard", "Wise old Wizard, keeper of secrets", "assets/TerraDialogues.json", "terra_wizard", {50.0f, 55.0f}, {{10.0f, 15.0f}, {1000.0f, 350.0f}, WHITE}}
            },
            {0, 100, 1280, 620}
        }
    };

    m_buildings = {
        {Realm::Aer, {
            {"gwen", "Tavern", {50, 200, 150, 250}, DARKBROWN, {51, 201, 148, 248}, BEIGE, {110, 440, 30, 20}, BROWN},
            {"aer_wizard", "Library", {900, 200, 150, 250}, LIGHTGRAY, {901, 201, 148, 248}, WHITE, {960, 440, 30, 20}, BROWN},
        }},
        {Realm::Aqua, {
            {"gwen", "Tavern", {50, 200, 150, 250}, DARKBROWN, {51, 201, 148, 248}, BEIGE, {110, 440, 30, 20}, BROWN},
            {"aqua_wizard", "Library", {900, 200, 150, 250}, WHITE, {901, 201, 148, 248}, LIGHTGRAY, {960, 440, 30, 20}, BROWN},
        }},
        {Realm::Terra, {
            {"gwen", "Tavern", {50, 200, 150, 250}, DARKBROWN, {51, 201, 148, 248}, BEIGE, {110, 440, 30, 20}, BROWN},
            {"terra_wizard", "Library", {900, 200, 150, 250}, WHITE, {901, 201, 148, 248}, LIGHTGRAY, {960, 440, 30, 20}, BROWN},
        }},
        {Realm::Ignis, {
            {"gwen", "Tavern", {50, 200, 150, 250}, DARKBROWN, {51, 201, 148, 248}, BEIGE, {110, 440, 30, 20}, BROWN},
            {"ignis_wizard", "Library", {900, 200, 150, 250}, WHITE, {901, 201, 148, 248}, LIGHTGRAY, {960, 440, 30, 20}, BROWN},
        }}
    };

    for(auto& regionBldg : m_buildings) {
        for(auto& bldg : regionBldg.buildings) {
            bldg.generateWalls();
        }
    }

    GameEngine::update();
}

GameEngine::~GameEngine() {
    rlImGuiShutdown();
    UnloadMusicStream(m_aquaSound);
    UnloadMusicStream(m_ignisSound);
    CloseAudioDevice();
    CloseWindow();
}

void GameEngine::run() {
    while (!WindowShouldClose()) {
        update();
        render();
    }
}

DialogueManager::DialogueManager() {}

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
                m_mapCol = (m_mapRow + 1) % 2;
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
    
}

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

    EndDrawing();
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
    
    DrawRectangleV(playerPos, {10.0f, 15.0f}, YELLOW);
}

void GameEngine::renderUI() {
    //Statistics
    ImGui::Begin("Statistics");
    ImGui::Text("Name: %s", m_player.getName().c_str());
    ImGui::Text("Gold: %d", m_player.getGold());
    ImGui::Separator();
    ImGui::Text("Factions Reputation:");
    ImGui::Text("Aer: %d | Aqua: %d | Terra: %d | Ignis: %d",
        m_player.getReputation(Realm::Aer),
        m_player.getReputation(Realm::Aqua),
        m_player.getReputation(Realm::Terra),
        m_player.getReputation(Realm::Ignis));
    ImGui::Separator();
    ImGui::Text("Elemental Mastery:");
    ImGui::Text("Aer: %d | Aqua: %d | Terra: %d | Ignis: %d",
        m_player.getMastery(Realm::Aer),
        m_player.getMastery(Realm::Aqua),
        m_player.getMastery(Realm::Terra),
        m_player.getMastery(Realm::Ignis));
    ImGui::End();
    
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