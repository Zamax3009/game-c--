#include "GameEngine.h"
#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

GameEngine::GameEngine() : m_player("Pilgrim") {
    InitWindow(1280, 720, "Realms of Elementa - Engine");
    SetTargetFPS(60);
    rlImGuiSetup(true);
    
    // Start initial dialogue
    if (m_dialogueManager.loadFromFile("assets/TerraDialogues.json", "terra_wizard")) {
        m_dialogueManager.startDialogue(0);
    }
}

GameEngine::~GameEngine() {
    rlImGuiShutdown();
    CloseWindow();
}

void GameEngine::run() {
    while (!WindowShouldClose()) {
        update();
        render();
    }
}

void GameEngine::update() {
    // Game update logic
}

void GameEngine::render() {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    rlImGuiBegin();
    renderUI();
    rlImGuiEnd();

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