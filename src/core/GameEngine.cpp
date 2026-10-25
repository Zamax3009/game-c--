#include "GameEngine.h"
#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

GameEngine::GameEngine() {
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
            {
                Enemy("Goblin", 1, 30, 10, {400.0f, 300.0f}, {20.0f, 20.0f}, RED),
                Enemy("Guard", 2, 20, 12, {800.0f, 500.0f}, {20.0f, 20.0f}, PURPLE)
            },
            {0, 100, 1280, 620}
        },
        {
            "Ignis", Realm::Ignis, {
                {"Gwen", "Tavern owner", "assets/IgnisDialogues.json", "gwen", {50.0f, 55.0f}, {{10.0f, 15.0f}, {100.0f, 350.0f}, BROWN}},
                {"Ignis Wizard", "Wise old Wizard, keeper of secrets", "assets/IgnisDialogues.json", "ignis_wizard", {50.0f, 55.0f}, {{10.0f, 15.0f}, {1000.0f, 350.0f}, WHITE}}
            },
            {
                Enemy("Goblin", 1, 30, 10, {400.0f, 300.0f}, {20.0f, 20.0f}, RED),
                Enemy("Guard", 2, 20, 12, {800.0f, 500.0f}, {20.0f, 20.0f}, PURPLE)
            },
            {0, 100, 1280, 620}
        },
        {
            "Aqua", Realm::Aqua, {
                {"Gwen", "Tavern owner", "assets/AquaDialogues.json", "gwen", {50.0f, 55.0f}, {{10.0f, 15.0f}, {100.0f, 350.0f}, BROWN}},
                {"Aqua Wizard", "Wise old Wizard, keeper of secrets", "assets/AquaDialogues.json", "aqua_wizard", {50.0f, 55.0f}, {{10.0f, 15.0f}, {1000.0f, 350.0f}, WHITE}}
            },
            {
                Enemy("Goblin", 1, 30, 10, {400.0f, 300.0f}, {20.0f, 20.0f}, RED),
                Enemy("Guard", 2, 20, 12, {800.0f, 500.0f}, {20.0f, 20.0f}, PURPLE)
            },
            {0, 100, 1280, 620}
        },
        {
            "Terra", Realm::Terra, {
                {"Gwen", "Tavern owner", "assets/TerraDialogues.json", "gwen", {50.0f, 55.0f}, {{10.0f, 15.0f}, {100.0f, 350.0f}, BROWN}},
                {"Terra Wizard", "Wise old Wizard, keeper of secrets", "assets/TerraDialogues.json", "terra_wizard", {50.0f, 55.0f}, {{10.0f, 15.0f}, {1000.0f, 350.0f}, WHITE}}
            },
            {
                Enemy("Goblin", 1, 30, 10, {400.0f, 300.0f}, {20.0f, 20.0f}, RED),
                Enemy("Guard", 2, 20, 12, {800.0f, 500.0f}, {20.0f, 20.0f}, PURPLE)
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

    m_inventory = {
        {"Mana Potion", "Restores a small amount of mana.", 3},
        {"Health Elixir", "Heals a small amount of demege.", 2},
        {"Ancent Scroll", "contains unreadable runes.", 1},
        {"Elemental Cristal", "Emits a dimm blue light.", 5}
    };

    m_worldItems = {
        {"Mana Potion", "Restores a small amount of mana.", 1, {250.0f, 300.0f}, Realm::Aer, false},
        {"Health Elixir", "Heals a small amount of demege.", 1, {850.0f, 350.0f}, Realm::Aer, false},
        {"Ancent Scroll", "contains unreadable runes.", 2, {850.0f, 350.0f}, Realm::Aqua, false},
        {"Elemental Cristal", "Emits a dimm blue light.", 1, {600.0f, 500.0f}, Realm::Aqua, false}
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

