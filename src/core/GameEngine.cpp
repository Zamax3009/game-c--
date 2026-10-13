#include "GameEngine.h"
#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

GameEngine::GameEngine() : m_player("Pilgrim") {
    InitWindow(1280, 720, "Realms of Elementa - Engine");
    InitAudioDevice();
    SetTargetFPS(60);
    rlImGuiSetup(true);
    
    m_aerSound = LoadMusicStream("assets/Aer_Main_Theme.mp3");
    m_ignisSound = LoadMusicStream("assets/Ignis_Main_Theme.mp3");
    m_currentState = GameState::WorldMap;

    m_regions = {
        {"Aer", Realm::Aer, { 0, 100, 640, 310 }, SKYBLUE},
        {"Aqua", Realm::Aqua, { 640, 410, 640, 310 }, BLUE},
        {"Terra", Realm::Terra, { 640, 100, 640, 310 }, GREEN},
        {"Ignis", Realm::Ignis, { 0, 410, 640, 310 }, ORANGE}
    };

    GameEngine::update();
}



GameEngine::~GameEngine() {
    rlImGuiShutdown();
    UnloadMusicStream(m_aerSound);
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


DialogueManager::DialogueManager(){

}



void GameEngine::update() {
    //Audio update
    if (m_selectRealm == Realm::Ignis && m_currentState == GameState::Dialogue) UpdateMusicStream(m_ignisSound);
    else if (m_selectRealm == Realm::Aer && m_currentState == GameState::Dialogue) UpdateMusicStream(m_aerSound);
    else if (m_currentState == GameState::WorldMap){
        if (!m_dialogueManager.isDialogueActive()){
            StopMusicStream(m_ignisSound);
            StopMusicStream(m_aerSound);
        }
    }
    //Map selection
    if (m_currentState == GameState::WorldMap) {
        Vector2 mousePos = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            for (const auto& region : m_regions){
                if (CheckCollisionPointRec(mousePos, region.bounds)){
                    m_selectRealm = region.realm;

                    if (m_selectRealm == Realm::Aer){
                        PlayMusicStream(m_aerSound);
                        if (m_dialogueManager.loadFromFile("assets//AerDialogues.json", "aer_wizard")) m_dialogueManager.startDialogue(0);
                    }
                    else if (m_selectRealm == Realm::Aqua){
                        if (m_dialogueManager.loadFromFile("assets//AquaDialogues.json", "aqua_wizard")) m_dialogueManager.startDialogue(0);
                    }
                    else if (m_selectRealm == Realm::Terra){
                        if (m_dialogueManager.loadFromFile("assets//TerraDialogues.json", "terra_wizard")) m_dialogueManager.startDialogue(0);
                    }
                    else if (m_selectRealm == Realm::Ignis){
                        PlayMusicStream(m_ignisSound);
                        if (m_dialogueManager.loadFromFile("assets//IgnisDialogues.json", "ignis_wizard")) m_dialogueManager.startDialogue(0);
                    }
                    m_currentState == GameState::Dialogue;
                    break;
                }
            }
        }
    }
    else if (m_currentState == GameState::Dialogue) {
        if(!m_dialogueManager.isDialogueActive()){
            m_currentState = GameState::WorldMap;
        }
    }

}

void GameEngine::render() {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    if (m_currentState == GameState::WorldMap) {
        Vector2 mousePos = GetMousePosition();
        DrawText("World map: choose a destination", 450, 40, 20, WHITE);
        for (const auto& region : m_regions){
            bool isHovered = CheckCollisionPointRec(mousePos, region.bounds);

            renderColor = region.color;
            if (isHovered){
                renderColor = ColorAlpha(region.color, 0.6f);
            }

            DrawRectangleRec(region.bounds, renderColor);
            
            Color borderColor = isHovered ?  WHITE : BLACK;
            int borderWidth = isHovered ? 3 : 1;
            
            DrawRectangleLinesEx(region.bounds, borderWidth, borderColor);

            DrawText(region.name.c_str(), region.bounds.x + 15, region.bounds.y + 15, 14, WHITE);

            if (isHovered) {
                DrawText("Click to enter...", region.bounds.x + 20, region.bounds.y + 60, 14, LIGHTGRAY);
            }

        }
    }
    else if (m_currentState == GameState::Dialogue) {
        rlImGuiBegin();
        renderUI();
        rlImGuiEnd();
    }

    

    EndDrawing();
}

void GameEngine::renderUI() {
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

    if (m_dialogueManager.isDialogueActive()) {
        const DialogueNode* node = m_dialogueManager.getCurrentNode();
        if (node) {
            ImGui::Begin("Dialogue", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::TextDisabled("[%s]", node->speaker.c_str());
            ImGui::TextWrapped("%s", node->text.c_str());
            ImGui::Separator();

            for (size_t i = 0; i < node->options.size(); ++i) {
                if (ImGui::Button(node->options[i].text.c_str())) {
                    m_dialogueManager.selectOption(static_cast<int>(i), m_player);
                }
            }
            ImGui::End();
        }
    }
}