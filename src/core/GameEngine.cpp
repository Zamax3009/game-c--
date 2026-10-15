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
        {"Tavern", {50, 275, 150, 250}, DARKBROWN, BEIGE, {195, 390, 10, 30}, BROWN},
        {"Library", {900, 275, 150, 250}, LIGHTGRAY, WHITE, {897, 390, 10, 30}, BROWN},
        }},
        {Realm::Aqua, {
        {"Tavern", {50, 275, 150, 250}, DARKBROWN, BEIGE, {195, 390, 10, 30}, BROWN},
        {"Library", {900, 275, 150, 250}, WHITE, LIGHTGRAY, {897, 390, 10, 30}, BROWN},
        }},
        {Realm::Terra, {
        {"Tavern", {50, 275, 150, 250}, DARKBROWN, BEIGE, {195, 390, 10, 30}, BROWN},
        {"Library", {900, 275, 150, 250}, WHITE, LIGHTGRAY, {897, 390, 10, 30}, BROWN},
        }},
        {Realm::Ignis, {
        {"Tavern", {50, 275, 150, 250}, DARKBROWN, BEIGE, {195, 390, 10, 30}, BROWN},
        {"Library", {900, 275, 150, 250}, WHITE, LIGHTGRAY, {897, 390, 10, 30}, BROWN},
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
    // Audio update
    if (m_selectRealm == Realm::Ignis && m_currentState == GameState::Dialogue) UpdateMusicStream(m_ignisSound);
    else if (m_selectRealm == Realm::Aqua && m_currentState == GameState::Dialogue) UpdateMusicStream(m_aquaSound);
    else if (m_currentState == GameState::WorldMap){
        if (!m_dialogueManager.isDialogueActive()){
            StopMusicStream(m_ignisSound);
            StopMusicStream(m_aquaSound);
        }
    }

    if (m_currentState == GameState::WorldMap || m_currentState == GameState::RegionHub) {
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

    if (m_currentState == GameState::WorldMap) {
        if (IsKeyPressed(KEY_F)) {
            Vector2 playerPos = m_player.getPosition();
            
            for (const auto& region : m_regions) {
                if (CheckCollisionPointRec(playerPos, region.bounds)) {
                    m_selectRealm = region.realm;
                    
                    if (m_selectRealm == Realm::Aqua) PlayMusicStream(m_aquaSound);
                    else if (m_selectRealm == Realm::Ignis) PlayMusicStream(m_ignisSound);

                    for (const auto& hub : m_regionHub) {
                        if (hub.realm == m_selectRealm) {
                            m_player.setPosition({
                                hub.rectangle.x + (hub.rectangle.width / 2),
                                hub.rectangle.y + (hub.rectangle.height /2)
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
    else if (m_currentState == GameState::RegionHub) {
        Vector2 playerPos = m_player.getPosition();

        for (auto& regionBldg : m_buildings) {
                    if (regionBldg.realm == m_selectRealm) {
                        for (auto& bldg : regionBldg.buildings) {
                            bldg.updateStatus(playerPos);
                        }
                    }
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

        if (IsKeyPressed(KEY_Q)) {
            m_currentState = GameState::WorldMap;
        }
    }
    else if (m_currentState == GameState::Dialogue) {
        if (!m_dialogueManager.isDialogueActive()){
            m_currentState = GameState::RegionHub;
        }
    }
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
    Color bldgColor = WHITE;
    for (const auto& regionBldg : m_buildings){
        if (regionBldg.realm == m_selectRealm){
            for (auto& bldg : regionBldg.buildings){
                if (bldg.isPlayerInside) {
                    bldgColor = bldg.floor;
                }
                else {
                    bldgColor = bldg.roof;
                }
                DrawRectangleRec(bldg.building, bldgColor);
                DrawRectangleLinesEx(bldg.building, 2, BLACK);
                DrawRectangleRec(bldg.entrance, bldg.door);
                DrawRectangleLinesEx(bldg.entrance, 1, BLACK);

                DrawText(bldg.name.c_str(), bldg.building.x + 10, bldg.building.y - 40, 14, WHITE);
            }
        }
    }
    
    DrawRectangleRec(currentHub->rectangle, ColorAlpha(currentRegionColor, 0.2f));
    DrawRectangleLinesEx(currentHub->rectangle, 2, currentRegionColor);
    DrawText(currentHub->name.c_str(), currentHub->rectangle.x + 20, currentHub->rectangle.y + 20, 20, WHITE);
    
    Vector2 playerPos = m_player.getPosition();
    
    for (const auto& RegionBldg : m_buildings){
        if (RegionBldg.realm == m_selectRealm){
            for (const auto& bldg : RegionBldg.buildings){

            }
        }
    }

    for (const auto& npcs : currentHub->npcs) {
        Vector2 pos = npcs.npcGraphics.npcPos;
        Vector2 size = npcs.npcGraphics.npcRec;
        Color col = npcs.npcGraphics.npcColor;
        Rectangle visualRec = {
            pos.x - (size.x/2),
            pos.y - (size.y/2),
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

        if (isPlayerInRange) DrawText("Press [F] to talk", pos.x - 45, visualRec.y - 35, 10, YELLOW);
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