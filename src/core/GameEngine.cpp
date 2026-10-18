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

    m_camera.target = m_player.getPosition();
    m_camera.offset = { 1280.0f / 2.0f, 720.0f / 2.0f },
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
            "Aer",  Realm::Aer, {
                {"Gwen", "Tavern owner", "assets/AerDialogues.json", "gwen", {50.0f, 55.0f}, {{10.0f, 15.0f}, {100.0f, 350.0f}, BROWN}},
                {"Aer Wizard", "Wise old Wizard, keeper of secrets", "assets/AerDialogues.json", "aer_wizard", {50.0f, 55.0f}, {{10.0f, 15.0f}, {1000.0f, 350.0f}, WHITE}}
            },
            {0, 100, 1280, 620}
        },
        {
            "Ignis",  Realm::Ignis, {
                {"Gwen", "Tavern owner", "assets/IgnisDialogues.json", "gwen", {50.0f, 55.0f}, {{10.0f, 15.0f}, {100.0f, 350.0f}, BROWN}},
                {"Ignis Wizard", "Wise old Wizard, keeper of secrets", "assets/IgnisDialogues.json", "ignis_wizard", {50.0f, 55.0f}, {{10.0f, 15.0f}, {1000.0f, 350.0f}, WHITE}}
            },
            {0, 100, 1280, 620}
        },
        {
            "Aqua",  Realm::Aqua, {
                {"Gwen", "Tavern owner", "assets/AquaDialogues.json", "gwen", {50.0f, 55.0f}, {{10.0f, 15.0f}, {100.0f, 350.0f}, BROWN}},
                {"Aqua Wizard", "Wise old Wizard, keeper of secrets", "assets/AquaDialogues.json", "aqua_wizard", {50.0f, 55.0f}, {{10.0f, 15.0f}, {1000.0f, 350.0f}, WHITE}}
            },
            {0, 100, 1280, 620}
        },
        {
            "Terra",  Realm::Terra, {
                {"Gwen", "Tavern owner", "assets/TerraDialogues.json", "gwen", {50.0f, 55.0f}, {{10.0f, 15.0f}, {100.0f, 350.0f}, BROWN}},
                {"Terra Wizard", "Wise old Wizard, keeper of secrets", "assets/TerraDialogues.json", "terra_wizard", {50.0f, 55.0f}, {{10.0f, 15.0f}, {1000.0f, 350.0f}, WHITE}}
            },
            {0, 100, 1280, 620}
        }
    };

    m_buildings = {
        {Realm::Aer, {
            // npcid, name, building, roof, interior, floor, entrance, door
            {"gwen", "Tavern", {50, 200, 150, 250}, DARKBROWN, {60, 210, 130, 230}, BEIGE, {110, 440, 30, 20}, BROWN},
            {"aer_wizard", "Library", {900, 200, 150, 250}, LIGHTGRAY, {910, 210, 130, 230}, WHITE, {960, 440, 30, 20}, BROWN},
        }},
        {Realm::Aqua, {
            {"gwen", "Tavern", {50, 200, 150, 250}, DARKBROWN, {60, 210, 130, 230}, BEIGE, {110, 440, 30, 20}, BROWN},
            {"aqua_wizard", "Library", {900, 200, 150, 250}, WHITE, {910, 210, 130, 230}, LIGHTGRAY, {960, 440, 30, 20}, BROWN},
        }},
        {Realm::Terra, {
            {"gwen", "Tavern", {50, 200, 150, 250}, DARKBROWN, {60, 210, 130, 230}, BEIGE, {110, 440, 30, 20}, BROWN},
            {"terra_wizard", "Library", {900, 200, 150, 250}, WHITE, {910, 210, 130, 230}, LIGHTGRAY, {960, 440, 30, 20}, BROWN},
        }},
        {Realm::Ignis, {
            {"gwen", "Tavern", {50, 200, 150, 250}, DARKBROWN, {60, 210, 130, 230}, BEIGE, {110, 440, 30, 20}, BROWN},
            {"ignis_wizard", "Library", {900, 200, 150, 250}, WHITE, {910, 210, 130, 230}, LIGHTGRAY, {960, 440, 30, 20}, BROWN},
        }}
    };

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


DialogueManager::DialogueManager(){}



void GameEngine::update() {
    // ==========================================
    // 1. AUDIO STREAM MANAGEMENT
    // ==========================================
    // Handle music playback and stopping based on the current realm and game state
    if (m_selectRealm == Realm::Ignis && m_currentState == GameState::Dialogue) {
        UpdateMusicStream(m_ignisSound);
    }
    else if (m_selectRealm == Realm::Aqua && m_currentState == GameState::Dialogue) {
        UpdateMusicStream(m_aquaSound);
    }
    else if (m_currentState == GameState::WorldMap) {
        // Stop music streams if no dialogue is active while on the world map
        if (!m_dialogueManager.isDialogueActive()) {
            StopMusicStream(m_ignisSound);
            StopMusicStream(m_aquaSound);
        }
    }

    // ==========================================
    // 2. WORLD MAP MOVEMENT
    // ==========================================
    // Process player directional input when roaming the world map
    if (m_currentState == GameState::WorldMap) {
        float velocity = 4.0f;
        Vector2 movement = { 0.0f, 0.0f };
        
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) movement.y -= 1.0f;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) movement.y += 1.0f;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) movement.x -= 1.0f;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) movement.x += 1.0f;

        if (movement.x != 0.0f || movement.y != 0.0f) {
            m_player.move({ movement.x * velocity, movement.y * velocity });
        }
    }

    // ==========================================
    // 3. WORLD MAP REGION SELECTION LOGIC
    // ==========================================
    if (m_currentState == GameState::WorldMap) {
        // Check if the player presses [F] to enter a region boundary
        if (IsKeyPressed(KEY_F)) {
            Vector2 playerPos = m_player.getPosition();
            
            for (const auto& region : m_regions) {
                if (CheckCollisionPointRec(playerPos, region.bounds)) {
                    m_selectRealm = region.realm;
                    
                    // Play realm-specific theme music
                    if (m_selectRealm == Realm::Aqua) PlayMusicStream(m_aquaSound);
                    else if (m_selectRealm == Realm::Ignis) PlayMusicStream(m_ignisSound);

                    // Reposition the player to the center of the selected hub
                    for (const auto& hub : m_regionHub) {
                        if (hub.realm == m_selectRealm) {
                            m_player.setPosition({
                                hub.rectangle.x + (hub.rectangle.width / 2),
                                hub.rectangle.y + (hub.rectangle.height / 2)
                            });
                            break;
                        }
                    }

                    // Switch state to RegionHub
                    m_currentState = GameState::RegionHub;
                    break;
                }
            }
        }
    }
    // ==========================================
    // 4. REGION HUB LOGIC (Movement & Collisions)
    // ==========================================
    else if (m_currentState == GameState::RegionHub) {
        Vector2 playerPos = m_player.getPosition();
        float velocity = 4.0f;
        Vector2 movement = { 0.0f, 0.0f };
        
        // Capture input for movement inside the hub
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) movement.y -= 1.0f;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) movement.y += 1.0f;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) movement.x -= 1.0f;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) movement.x += 1.0f;

        // --- BUILDING COLLISION AND STATUS CHECK ---
        for (auto& regionBldg : m_buildings) {
            if (regionBldg.realm == m_selectRealm) {
                for (auto& bldg : regionBldg.buildings) {
                    
                    // Update whether the player is inside the building (interior or entrance)
                    bldg.updateStatus(playerPos);

                    // If the player is outside, exterior walls act as solid obstacles
                    if (!bldg.isPlayerInside) {
                        // Project future positions separately on X and Y axes to allow smooth wall sliding
                        Rectangle playerRectX = { playerPos.x + (movement.x * velocity) - 5.0f, playerPos.y - 7.5f, 10.0f, 15.0f };
                        Rectangle playerRectY = { playerPos.x - 5.0f, playerPos.y + (movement.y * velocity) - 7.5f, 10.0f, 15.0f };

                        // Stop movement along the respective axis if a collision occurs
                        if (CheckCollisionRecs(playerRectX, bldg.building)) {
                            movement.x = 0.0f;
                        }
                        if (CheckCollisionRecs(playerRectY, bldg.building)) {
                            movement.y = 0.0f;
                        }
                    }
                }
            }
        }

        // Apply movement if no collision blocks it
        if (movement.x != 0.0f || movement.y != 0.0f) {
            m_player.move({ movement.x * velocity, movement.y * velocity });
            playerPos = m_player.getPosition(); // Refresh local position reference
        }

        // --- NPC INTERACTION WITH [F] ---
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
                    // Define NPC collision bounding box
                    Rectangle npcRec = {
                        npc.npcGraphics.npcPos.x - (npc.npcCollision.x / 2.0f),
                        npc.npcGraphics.npcPos.y - (npc.npcCollision.y / 2.0f),
                        npc.npcCollision.x,
                        npc.npcCollision.y
                    };

                    // Trigger dialogue if the player interacts within range
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

        // --- RETURN TO WORLD MAP WITH [Q] ---
        if (IsKeyPressed(KEY_Q)) {
            m_currentState = GameState::WorldMap;
        }
    }
    // ==========================================
    // 5. DIALOGUE STATE MANAGEMENT
    // ==========================================
    else if (m_currentState == GameState::Dialogue) {
        // Return to the RegionHub automatically when dialogue ends
        if (!m_dialogueManager.isDialogueActive()){
            m_currentState = GameState::RegionHub;
        }
    }
    
    // Update camera target to smoothly follow the player's position
    m_camera.target = m_player.getPosition();
}

void GameEngine::render() {
    BeginDrawing();
    ClearBackground(DARKGRAY);
    BeginMode2D(m_camera);
    if (m_currentState == GameState::WorldMap) {
        rlImGuiBegin();
        renderMap();
        rlImGuiEnd();
    }
    
    else if (m_currentState == GameState::RegionHub) {
        rlImGuiBegin();
        renderRegion();
        rlImGuiEnd();
    }

    EndMode2D();

    if (m_currentState == GameState::Dialogue) {
        rlImGuiBegin();
        renderUI();
        rlImGuiEnd();
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
        
        Color borderColor = isPlayerInside ?  WHITE : BLACK;
        int borderWidth = isPlayerInside ? 3 : 1;
        
        DrawRectangleLinesEx(region.bounds, borderWidth, borderColor);

        DrawText(region.name.c_str(), region.bounds.x + 15, region.bounds.y + 15, 14, WHITE);

        if (isPlayerInside) {
            DrawText("Press [F] to enter...", region.bounds.x + 20, region.bounds.y + 60, 14, LIGHTGRAY);
        }
    }

    DrawRectangleV(playerPos, {10.0, 15.0}, YELLOW);

}

void GameEngine::renderRegion() {
    // Find the color of the current selected region
    Color currentRegionColor = WHITE;
    for (const auto& region : m_regions) {
        if (region.realm == m_selectRealm){
            currentRegionColor = region.color;
            break;
        }
    }

    // Find the current region hub data
    RegionHubG* currentHub = nullptr;
    for (auto& hub : m_regionHub) {
        if (hub.realm == m_selectRealm) {
            currentHub = &hub;
            break;
        }
    }
    if (!currentHub) return;

    // ==========================================
    // 1. RENDER BUILDINGS (Interiors, Roofs & Doors)
    // ==========================================
    for (const auto& regionBldg : m_buildings){
        if (regionBldg.realm == m_selectRealm){
            for (auto& bldg : regionBldg.buildings){
                
                // Always draw the interior floor underneath
                DrawRectangleRec(bldg.interior, bldg.floor);
                DrawRectangleLinesEx(bldg.interior, 1, DARKGRAY);

                // If the player is OUTSIDE, draw the roof/exterior walls and entrance door over it
                if (!bldg.isPlayerInside) {
                    DrawRectangleRec(bldg.building, bldg.roof);
                    DrawRectangleLinesEx(bldg.building, 2, BLACK);
                    DrawRectangleRec(bldg.entrance, bldg.door);
                    DrawRectangleLinesEx(bldg.entrance, 1, BLACK);
                    
                    DrawText(bldg.name.c_str(), bldg.building.x + 10, bldg.building.y + 10, 14, WHITE);
                } 
                else {
                    // If the player is INSIDE, the roof disappears (cutaway effect), show building name inside
                    DrawText(bldg.name.c_str(), bldg.interior.x + 10, bldg.interior.y + 10, 14, DARKGRAY);
                }
            }
        }
    }
    
    // Draw the main region hub background and boundaries
    DrawRectangleRec(currentHub->rectangle, ColorAlpha(currentRegionColor, 0.2f));
    DrawRectangleLinesEx(currentHub->rectangle, 2, currentRegionColor);
    DrawText(currentHub->name.c_str(), currentHub->rectangle.x + 20, currentHub->rectangle.y + 20, 20, WHITE);
    
    Vector2 playerPos = m_player.getPosition();
    
    // ==========================================
    // 2. RENDER NPCs INSIDE BUILDINGS
    // ==========================================
    // Render NPCs only if the player is currently inside the building containing them
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

                            // Draw NPC visual body and outline
                            DrawRectangleRec(visualRec, col);
                            DrawRectangleLinesEx(visualRec, 1, BLACK);

                            // Draw NPC name tag above them
                            DrawText(npcs.name.c_str(), pos.x - (MeasureText(npcs.name.c_str(), 12) / 2), visualRec.y - 18, 12, WHITE);

                            // Display interaction prompt if player is close enough
                            if (isPlayerInRange) {
                                DrawText("Press [F] to talk", pos.x - 45, visualRec.y - 35, 10, YELLOW);
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Draw the player character
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