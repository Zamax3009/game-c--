#include "MainMenuState.h"
#include "RegionHubState.h"
#include "GameEngine.h"
#include "PauseState.h"
#include "OptionsState.h"
#include "InventoryState.h"
#include "CombatState.h"
#include "ShopState.h"
#include "QuestLogState.h"
#include "DialogueState.h"
#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"
#include "fstream"
#include "sstream"
#include "sqlite3.h"

void GameEngine::goToRegionHub() { changeState(std::make_unique<RegionHubState>()); }
void GameEngine::goToWorldMap() { changeState(std::make_unique<WorldMapState>()); }
void GameEngine::goToInventory() { changeState(std::make_unique<InventoryState>()); }
void GameEngine::goToCombat() { changeState(std::make_unique<CombatState>()); }
void GameEngine::goToShop() { changeState(std::make_unique<ShopState>()); }
void GameEngine::goToQuestLog() { changeState(std::make_unique<QuestLogState>()); }
void GameEngine::goToDialogue() { changeState(std::make_unique<DialogueState>()); }
void GameEngine::goToMainMenu() { changeState(std::make_unique<MainMenuState>()); }
void GameEngine::goToPause() { changeState(std::make_unique<PauseState>()); }
void GameEngine::goToOptions() { changeState(std::make_unique<OptionsState>()); }

bool GameEngine::doesSaveFileExist(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

void GameEngine::saveGame(const std::string& filename) {
    sqlite3* db;
    char* errMsg = nullptr;

    if (sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
        TraceLog(LOG_ERROR, "Can't open database: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    
    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg);

    std::string createPlayerTable = 
        "CREATE TABLE IF NOT EXISTS player ("
        "hp REAL, max_hp INTEGER, mana INTEGER, max_mana INTEGER, gold INTEGER, x REAL, y REAL, realm INTEGER, level INTEGER, xp INTEGER, xp_to_next_level INTEGER, rep_aer INTEGER, rep_aqua INTEGER, rep_terra INTEGER, rep_ignis INTEGER, mast_aer INTEGER, mast_aqua INTEGER, mast_terra INTEGER, mast_ignis INTEGER);";
    
    sqlite3_exec(db, createPlayerTable.c_str(), nullptr, nullptr, &errMsg);
    sqlite3_exec(db, "DELETE FROM player;", nullptr, nullptr, &errMsg);
    Vector2 playerPos = m_player.getPosition();
    std::string insertPlayer = "INSERT INTO player VALUES (" +
        std::to_string(m_player.getHP())                     + ", " +
        std::to_string(m_player.getMaxHP())                  + ", " +
        std::to_string(m_player.getMana())                   + ", " +
        std::to_string(m_player.getMaxMana())                + ", " +
        std::to_string(m_player.getGold())                   + ", " +
        std::to_string(playerPos.x)                          + ", " +
        std::to_string(playerPos.y)                          + ", " +
        std::to_string(static_cast<int>(m_selectRealm))      + ", " +
        std::to_string(m_player.getLevel())                  + ", " +
        std::to_string(m_player.getXP())                     + ", " +
        std::to_string(m_player.getXPToNextLevel())          + ", " +
        std::to_string(m_player.getReputation(Realm::Aer))   + ", " +
        std::to_string(m_player.getReputation(Realm::Aqua))  + ", " +
        std::to_string(m_player.getReputation(Realm::Terra)) + ", " +
        std::to_string(m_player.getReputation(Realm::Ignis)) + ", " +
        std::to_string(m_player.getMastery(Realm::Aer))      + ", " +
        std::to_string(m_player.getMastery(Realm::Aqua))     + ", " +
        std::to_string(m_player.getMastery(Realm::Terra))    + ", " +
        std::to_string(m_player.getMastery(Realm::Ignis))    + ");";
    
    sqlite3_exec(db, insertPlayer.c_str(), nullptr, nullptr, &errMsg);
        sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS inventory (item_id INTEGER, quantity INTEGER);", nullptr, nullptr, &errMsg);
    sqlite3_exec(db, "DELETE FROM inventory;", nullptr, nullptr, &errMsg);
    for (const auto& item : m_inventory.getItems()) {
        std::string insertInv = "INSERT INTO inventory VALUES (" + std::to_string(static_cast<int>(item.id)) + ", " + std::to_string(item.quantity) + ");";
        sqlite3_exec(db, insertInv.c_str(), nullptr, nullptr, &errMsg);
    }
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS world_items (item_index INTEGER, is_collected INTEGER);", nullptr, nullptr, &errMsg);
    sqlite3_exec(db, "DELETE FROM world_items;", nullptr, nullptr, &errMsg);
    for (size_t i = 0; i < m_worldItems.size(); i++) {
        std::string insertWI = "INSERT INTO world_items VALUES (" + std::to_string(i) + ", " + (m_worldItems[i].isCollected() ? "1" : "0") + ");";
        sqlite3_exec(db, insertWI.c_str(), nullptr, nullptr, &errMsg);
    }
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS enemies (hub_index INTEGER, area_index INTEGER, enemy_index INTEGER, is_defeated INTEGER);", nullptr, nullptr, &errMsg);
    sqlite3_exec(db, "DELETE FROM enemies;", nullptr, nullptr, &errMsg);
    for (size_t h = 0; h < m_regionHub.size(); h++) {
        for (size_t a = 0; a < m_regionHub[h].areas.size(); a++) {
            for (size_t e = 0; e < m_regionHub[h].areas[a].enemies.size(); e++) {
                std::string insertEnemy = "INSERT INTO enemies VALUES (" + std::to_string(h) + ", " + std::to_string(a) + ", " + std::to_string(e) + ", " + (m_regionHub[h].areas[a].enemies[e].isDefeated() ? "1" : "0") + ");";
                sqlite3_exec(db, insertEnemy.c_str(), nullptr, nullptr, &errMsg);
            }
        }
    }
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS hub_state ( hub_index INTEGER, current_sub_area INTEGER);", nullptr, nullptr, &errMsg);
    sqlite3_exec(db, "DELETE FROM hub_state;", nullptr, nullptr, &errMsg);
    for (size_t h = 0; h < m_regionHub.size(); h++) {
        std::string insertHub = "INSERT INTO hub_state VALUES (" +
            std::to_string(h) + ", " +
            std::to_string(static_cast<int>(m_regionHub[h].currenSubAreaId)) + ");";
        sqlite3_exec(db, insertHub.c_str(), nullptr, nullptr, &errMsg);
    }

    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS quests (quest_id INTEGER, is_accepted INTEGER, is_completed INTEGER, current_progress INTEGER);", nullptr, nullptr, &errMsg);
    sqlite3_exec(db, "DELETE FROM quests;", nullptr, nullptr, &errMsg);
    for (const auto& quest : m_quests) {
        std::string insertQuest = "INSERT INTO quests VALUES (" + 
            std::to_string(static_cast<int>(quest.id)) + ", " + 
            (quest.isAccepted ? "1" : "0") + ", " + 
            (quest.isCompleted ? "1" : "0") + ", " + 
            std::to_string(quest.currentProgress) + ");";
        sqlite3_exec(db, insertQuest.c_str(), nullptr, nullptr, &errMsg);
    }

    sqlite3_exec(db, "END TRANSACTION;", nullptr, nullptr, &errMsg);
    sqlite3_close(db);
    TraceLog(LOG_INFO, "Game saved successfully to SQLite.");
}

void GameEngine::loadGame(const std::string& filename) {
    sqlite3* db;
    sqlite3_stmt* stmt;

    if (sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
        TraceLog(LOG_WARNING, "Save database not found or cannot be opened.");
        sqlite3_close(db);
        return;
    }

    std::string selectPlayerQuery = "SELECT hp, max_hp, mana, max_mana, gold, x, y, realm, level, xp, xp_to_next_level, rep_aer, rep_aqua, rep_terra, rep_ignis, mast_aer, mast_aqua, mast_terra, mast_ignis FROM player;";
    
    if (sqlite3_prepare_v2(db, selectPlayerQuery.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            m_player.setHP(static_cast<float>(sqlite3_column_double(stmt, 0)));
            m_player.setMaxHp(sqlite3_column_int(stmt, 1));
            m_player.setMana(sqlite3_column_int(stmt, 2));
            m_player.setMaxMana(sqlite3_column_int(stmt, 3));
            m_player.setGold(sqlite3_column_int(stmt, 4));

            float posX = static_cast<float>(sqlite3_column_double(stmt, 5));
            float posY = static_cast<float>(sqlite3_column_double(stmt, 6));
            m_player.setPosition({posX, posY});
            
            m_selectRealm = static_cast<Realm>(sqlite3_column_int(stmt, 7));
            
            m_player.setLevel(sqlite3_column_int(stmt, 8));
            m_player.setXP(sqlite3_column_int(stmt, 9));
            m_player.setXPToNextLevel(sqlite3_column_int(stmt, 10));
            m_player.modifyReputation(Realm::Aer, sqlite3_column_int(stmt, 11)   -   m_player.getReputation(Realm::Aer));
            m_player.modifyReputation(Realm::Aqua, sqlite3_column_int(stmt, 12)  -  m_player.getReputation(Realm::Aqua));
            m_player.modifyReputation(Realm::Terra, sqlite3_column_int(stmt, 13) - m_player.getReputation(Realm::Terra));
            m_player.modifyReputation(Realm::Ignis, sqlite3_column_int(stmt, 14) - m_player.getReputation(Realm::Ignis));
            m_player.modifyMastery(Realm::Aer, sqlite3_column_int(stmt, 15)      -      m_player.getMastery(Realm::Aer));
            m_player.modifyMastery(Realm::Aqua, sqlite3_column_int(stmt, 16)     -     m_player.getMastery(Realm::Aqua));
            m_player.modifyMastery(Realm::Terra, sqlite3_column_int(stmt, 17)    -    m_player.getMastery(Realm::Terra));
            m_player.modifyMastery(Realm::Ignis, sqlite3_column_int(stmt, 18)    -    m_player.getMastery(Realm::Ignis));
        }
        sqlite3_finalize(stmt);
    }

    m_inventory.clear();
    if (sqlite3_prepare_v2(db, "SELECT item_id, quantity FROM inventory;", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            ItemID id = static_cast<ItemID>(sqlite3_column_int(stmt, 0));
            int qty = sqlite3_column_int(stmt, 1);
            m_inventory.addItem(createItem(id, qty));
        }
        sqlite3_finalize(stmt);
    }

    if (sqlite3_prepare_v2(db, "SELECT item_index, is_collected FROM world_items;", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int index = sqlite3_column_int(stmt, 0);
            int collected = sqlite3_column_int(stmt, 1);
            if (index >= 0 && index < m_worldItems.size()) {
                m_worldItems[index].setCollected(collected == 1);
            }
        }
        sqlite3_finalize(stmt);
    }

    if (sqlite3_prepare_v2(db, "SELECT hub_index, area_index, enemy_index, is_defeated FROM enemies;", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int h = sqlite3_column_int(stmt, 0);
            int a = sqlite3_column_int(stmt, 1);
            int e = sqlite3_column_int(stmt, 2);
            int defeated = sqlite3_column_int(stmt, 3);
            if (h < m_regionHub.size() && a < m_regionHub[h].areas.size() && e < m_regionHub[h].areas[a].enemies.size()) {
                m_regionHub[h].areas[a].enemies[e].setDefeated(defeated == 1);
            }
        }
        sqlite3_finalize(stmt);
    }

    if (sqlite3_prepare_v2(db, "SELECT hub_index, current_sub_area FROM hub_state;", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int h = sqlite3_column_int(stmt, 0);
            int subAreaInt = sqlite3_column_int(stmt, 1);
            if (h < m_regionHub.size()) {
                m_regionHub[h].currenSubAreaId = static_cast<AreaID>(subAreaInt);
            }
        }
        sqlite3_finalize(stmt);
    }

    if (sqlite3_prepare_v2(db, "SELECT quest_id, is_accepted, is_completed, current_progress FROM quests;", -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int qIdInt = sqlite3_column_int(stmt, 0);
            int accepted = sqlite3_column_int(stmt, 1);
            int completed = sqlite3_column_int(stmt, 2);
            int progress = sqlite3_column_int(stmt, 3);

            QuestID qId = static_cast<QuestID>(qIdInt);
            for (auto& quest : m_quests) {
                if (quest.id == qId) {
                    quest.isAccepted = (accepted == 1);
                    quest.isCompleted = (completed == 1);
                    quest.currentProgress = progress;
                }
            }
        }
        sqlite3_finalize(stmt);
    }
    
    goToRegionHub();
    sqlite3_close(db);
    TraceLog(LOG_INFO, "Game loaded successfully from SQLite with all variables!");
}

void GameEngine::acceptQuest(QuestID id) {
    for (auto& quest : m_quests) {
        if (quest.id == id && !quest.isAccepted) {
            quest.isAccepted = true;
            TraceLog(LOG_INFO, "Quest accepted via dialogue!");
        }
    }
}

void GameEngine::completeQuest(Quest& quest) {
    if (quest.isCompleted) return;
    quest.isCompleted = true;

    m_player.addGold(quest.rewardGold);
    m_player.addXP(quest.rewardXP);

    showNotification("Quest completed: " + quest.title + "!");
}



Item GameEngine::createItem(ItemID id, int quantity) {
    switch (id)
    {
    case ItemID::ManaPotion:
        return { ItemID::ManaPotion, "Mana Potion", "Restores a small amount of mana.", quantity, ItemType::RestoreMana, 35};
        break;
    
    case ItemID::HealthElixir:
        return {ItemID::HealthElixir, "Health Elixir", "Heals a small amount of demege.", quantity, ItemType::Heal, 35};
        break;
    
    case ItemID::AncientScroll:
        return {ItemID::AncientScroll, "Ancent Scroll", "Contains unreadable runes.", quantity, ItemType::None, 0};
        break;

    case ItemID::ElementalCrystal:
        return {ItemID::ElementalCrystal, "Elemental Crystal", "Emits a dimm blue light.", quantity, ItemType::None, 0};
        break;
    default:
        return {ItemID::None, "Unknown", "Unknown item, don't touch", 0, ItemType::None, 0};
        break;
    }
}

GameEngine::GameEngine() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(SCREENSIZEX, SCREENSIZEY, "Realms of Elementa - Engine");
    SetExitKey(KEY_NULL);
    InitAudioDevice();
    SetTargetFPS(60);
    rlImGuiSetup(true);

    m_targetTexture = LoadRenderTexture(SCREENSIZEX, SCREENSIZEY);

    m_player.setPosition({ SCREENSIZEX / 2.0f, SCREENSIZEY / 2.0f});
    
    m_aquaSound = LoadMusicStream("assets/Aqua_Main_Theme.mp3");
    m_ignisSound = LoadMusicStream("assets/Ignis_Main_Theme.mp3");
    goToMainMenu();

    m_camera.offset = { SCREENSIZEX / 2.0f, SCREENSIZEY / 2.0f };
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
                {}
            },
            {
                { AreaID::Village, "Aer Village", {0, 100, 1280, 620}, {}, {}, BLUE }
            },
            AreaID::Village
        },
        {
            "Aqua", Realm::Aqua, {
                {}
            },
            {
                { AreaID::Village, "Aqua Village", {0, 100, 1280, 620}, {}, {}, BLUE }
            },
            AreaID::Village
        },
        {
            "Terra", Realm::Terra, {
                {}
            },
            {
                { AreaID::Village, "Terra Village", {0, 100, 1280, 620}, {}, {}, BLUE }
            },
            AreaID::Village
        },
        {
            "Ignis", Realm::Ignis, {
                {"Gwen", "Tavern owner", "assets/IgnisDialogues.json", "gwen", {50.0f, 55.0f}, {{10.0f, 15.0f}, {100.0f, 350.0f}, BROWN}},
                {"Ignis Wizard", "Wise old Wizard, keeper of secrets", "assets/IgnisDialogues.json", "ignis_wizard", {50.0f, 55.0f}, {{10.0f, 15.0f}, {1000.0f, 350.0f}, WHITE}}
            },
            {
                { AreaID::Village, "Ignis Village", {0, 100, 1280, 620}, {}, {}, BLUE },
                { AreaID::Capital, "Ignis Citadel", {0, 100, 1280, 620}, {Enemy("Guard", 2, 20, 12, {800.0f, 500.0f}, {20.0f, 20.0f}, PURPLE)}, {}, DARKGRAY },
                { AreaID::ScorchedSteppes, "Scorched Steppes", {0, 100, 1280, 620}, {Enemy("Magma Goblin", 3, 40, 15, {400.0f, 300.0f}, {20.0f, 20.0f}, ORANGE), Enemy("Magma Goblin", 3, 40, 15, {500.0f, 300.0f}, {20.0f, 20.0f}, ORANGE)}, {}, MAROON },
                { AreaID::AncientTitanForge, "Ancient Titan Forge", {0, 100, 1280, 620}, {Enemy("Titan Golem", 5, 100, 25, {640.0f, 350.0f}, {40.0f, 40.0f}, GOLD)}, {WorldItem(ItemID::ElementalCrystal, "Titan Core", "A glowing relic from the forge.", 1, {640.0f, 250.0f}, Realm::Ignis, ItemType::None, 0, false)}, BLACK}
            },
            AreaID::Village
        }
    };
    m_buildings = {
        {Realm::Aer, {
            {AreaID::Capital, "gwen", "Tavern", {50, 200, 150, 250}, DARKBROWN, {51, 201, 148, 248}, BEIGE, {110, 440, 30, 20}, BROWN},
            {AreaID::Capital, "aer_wizard", "Library", {900, 200, 150, 250}, LIGHTGRAY, {901, 201, 148, 248}, WHITE, {960, 440, 30, 20}, BROWN},
        }},
        {Realm::Aqua, {
            {AreaID::Capital, "gwen", "Tavern", {50, 200, 150, 250}, DARKBROWN, {51, 201, 148, 248}, BEIGE, {110, 440, 30, 20}, BROWN},
            {AreaID::Capital, "aqua_wizard", "Library", {900, 200, 150, 250}, WHITE, {901, 201, 148, 248}, LIGHTGRAY, {960, 440, 30, 20}, BROWN},
        }},
        {Realm::Terra, {
            {AreaID::Capital, "gwen", "Tavern", {50, 200, 150, 250}, DARKBROWN, {51, 201, 148, 248}, BEIGE, {110, 440, 30, 20}, BROWN},
            {AreaID::Capital, "terra_wizard", "Library", {900, 200, 150, 250}, WHITE, {901, 201, 148, 248}, LIGHTGRAY, {960, 440, 30, 20}, BROWN},
        }},
        {Realm::Ignis, {
            {AreaID::Capital, "gwen", "Tavern", {50, 200, 150, 250}, DARKBROWN, {51, 201, 148, 248}, BEIGE, {110, 440, 30, 20}, BROWN},
            {AreaID::Capital, "ignis_wizard", "Library", {900, 200, 150, 250}, WHITE, {901, 201, 148, 248}, LIGHTGRAY, {960, 440, 30, 20}, BROWN},
        }}
    };

    m_inventory = {
        {ItemID::ManaPotion, "Mana Potion", "Restores a small amount of mana.", 0, ItemType::RestoreMana, 35},
        {ItemID::HealthElixir, "Health Elixir", "Heals a small amount of demege.", 0, ItemType::Heal, 35},
        {ItemID::AncientScroll, "Ancent Scroll", "Contains unreadable runes.", 0, ItemType::None, 0},
        {ItemID::ElementalCrystal, "Elemental Crystal", "Emits a dimm blue light.", 0, ItemType::None, 0}
    };

    m_worldItems = {
        {ItemID::ManaPotion, "Mana Potion", "Restores a small amount of mana.", 1, {250.0f, 300.0f}, Realm::Aer, ItemType::RestoreMana, 35, false},
        {ItemID::HealthElixir, "Health Elixir", "Heals a small amount of demege.", 1, {850.0f, 350.0f}, Realm::Aer, ItemType::Heal, 35, false},
        {ItemID::AncientScroll, "Ancent Scroll", "Contains unreadable runes.", 2, {850.0f, 350.0f}, Realm::Aqua, ItemType::None, 0, false},
        {ItemID::ElementalCrystal, "Elemental Crystal", "Emits a dimm blue light.", 1, {600.0f, 500.0f}, Realm::Aqua, ItemType::None, 0, false}
    };

    m_shopItems = {
        {ItemID::ManaPotion, "Mana Potion", "Restores a small amount of mana.", 20, ItemType::RestoreMana, 35},
        {ItemID::HealthElixir, "Health Elixir", "Heals a small amount of demege.", 20, ItemType::Heal, 35},
    };

    m_quests = {
        { QuestID::GoblinHunt, "Goblin Hunt", "Defeat 2 Goblins", false, false, 0, 2, 60, 150 },
        { QuestID::CrystalGathering, "Gather Crystal", "Gather 1 Elemental Crystal in the Aqua Realm", false, false, 0, 1, 60, 150 },
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
    UnloadRenderTexture(m_targetTexture);
    UnloadMusicStream(m_aquaSound);
    UnloadMusicStream(m_ignisSound);
    CloseAudioDevice();
    CloseWindow();
}

void GameEngine::run() {
    while (!WindowShouldClose() && !m_exitGame) {
        update();
        render();
    }
}

bool GameEngine::checkWallCollision(const Rectangle& rec) {
    RegionHubG* currentHub = nullptr;
    for (auto& hub : m_regionHub) {
        if (hub.realm == m_selectRealm) {
            currentHub = &hub;
            break;
        }
    }
    if (!currentHub) return false;

    SubArea* currentSubArea = nullptr;
    for (auto& sa : currentHub->areas) {
        if (sa.id == currentHub->currenSubAreaId) {
            currentSubArea = &sa;
            break;
        }
    }
    if (!currentSubArea) return false;

    if (rec.x < currentSubArea->bounds.x ||
        (rec.x + rec.width) > (currentSubArea->bounds.x + currentSubArea->bounds.width) ||
        rec.y < currentSubArea->bounds.y ||
        (rec.y + rec.width) > (currentSubArea->bounds.y + currentSubArea->bounds.height)) {
            return true;
    }

    for (auto& regionBldg : m_buildings) {
        if (regionBldg.realm == m_selectRealm) {
            for (auto& bldg : regionBldg.buildings) {
                if (bldg.areaID != currentSubArea->id) continue;
                for (const auto& wall : bldg.walls) {
                    if (CheckCollisionRecs(rec, wall.bounds)){
                        return true;
                    }
                }
                if (CheckCollisionRecs(rec, bldg.entrance) || CheckCollisionRecs(rec, bldg.interior)) {
                    bldg.isPlayerInside = true;
                }
                else {
                    bldg.isPlayerInside = false;
                }
            }
        }
    }
    return false;
}
void GameEngine::playerMovement(float velocity) {
    float dt = GetFrameTime();
    Vector2 dir = { 0.0f, 0.0f };
    
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) dir.y -= 1.0f;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) dir.y += 1.0f;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) dir.x -= 1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) dir.x += 1.0f;

    if (Vector2Length(dir) > 0.0f) {
        dir = Vector2Normalize(dir);
        m_player.setLastDirection(dir);
    }

    if ((IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_SPACE)) && !m_player.getIsDashing() && m_player.getStamina() >= m_player.getDashCost()) {
        m_player.setIsDashing(true);
        m_player.setDashTimer(m_player.getDashDuration());
        m_player.setStamina(m_player.getStamina() - m_player.getDashCost());
        
        m_player.setStaminaVisibilityTimer(1.0f);
    }

    if (m_player.getIsDashing()) {
        m_player.setDashTimer(m_player.getDashTimer() - dt);
        
        Vector2 lastDir = m_player.getLastDirection();
        float dashSpd = m_player.getDashSpeed();
        
        Vector2 nextStep = { lastDir.x * dashSpd, lastDir.y * dashSpd };
        Rectangle nextRect = { m_player.getPosition().x + nextStep.x, m_player.getPosition().y + nextStep.y, 10.0f, 15.0f };
        
        if (!checkWallCollision(nextRect)) {
            m_player.move(nextStep);
        }

        if (m_player.getDashTimer() <= 0.0f) {
            m_player.setIsDashing(false);
        }
    } else {
        Vector2 movement = { dir.x * velocity, dir.y * velocity };
        Vector2 playerPos = m_player.getPosition();
        
        Rectangle nextRectX = { playerPos.x + movement.x, playerPos.y, 10.0f, 15.0f };
        if (movement.x != 0.0f && !checkWallCollision(nextRectX)) {
            m_player.move({ movement.x, 0.0f });
        }

        playerPos = m_player.getPosition();
        Rectangle nextRectY = { playerPos.x, playerPos.y + movement.y, 10.0f, 15.0f };
        if (movement.y != 0.0f && !checkWallCollision(nextRectY)) {
            m_player.move({ 0.0f, movement.y });
        }

        float currentStamina = m_player.getStamina();
        float maxStamina = m_player.getMaxStamina();

        if (currentStamina < maxStamina) {
            float newStamina = currentStamina + (m_player.getStaminaRegenRate() * dt);
            if (newStamina > maxStamina) newStamina = maxStamina;
            m_player.setStamina(newStamina);
            
            m_player.setStaminaVisibilityTimer(1.0f);
        } else {
            float timer = m_player.getStaminaVisibilityTimer();
            if (timer > 0.0f) {
                m_player.setStaminaVisibilityTimer(timer - dt);
            }
        }
    }
}

int GameEngine::getSelectedAttack() {
    const auto& attacks = m_player.getAttacks();

    const KeyboardKey attackKeys[] = { 
        KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, 
        KEY_FIVE, KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE 
    };

    int maxCheck = std::min(attacks.size(), sizeof(attackKeys) / sizeof(attackKeys[0]));
    
    for (int i = 0; i < maxCheck; i++) {
        if (IsKeyPressed(attackKeys[i])) {
            return i;
        }
    }

    return -1;
}

void GameEngine::updateRealtimeCombat(SubArea* currentSubArea) {
    float dt = GetFrameTime();

    if (m_player.getCooldownTimer() > 0.0f) {
        m_player.setCooldownTimer(m_player.getCooldownTimer() - dt);
    }

    if (m_player.getIsAttacking()) {
        m_player.setAttackTimer(m_player.getAttackTimer() - dt);
        if (m_player.getAttackTimer() <= 0.0f) {
            m_player.setIsAttacking(false);
        }
    }

    if ((IsKeyPressed(KEY_K) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) && m_player.canAttack()) {
        m_player.setIsAttacking(true);
        m_player.setAttackTimer(0.15f);
        m_player.setCooldownTimer(0.35f);

        Rectangle attackBox = m_player.getAttackHitbox();

        if (currentSubArea) {
            for (auto& enemy : currentSubArea->enemies) {
                if (!enemy.isDefeated()) {
                    Rectangle enemyBox = enemy.getCollisionBox();
                    if (CheckCollisionRecs(attackBox, enemyBox)) {
                        enemy.takeDamage(m_player.getAttackDamage());
                        int dmg = m_player.getAttackDamage();
                        showFloatingText("-" + std::to_string(dmg), enemy.getPosition(), RED);
                        
                        TraceLog(LOG_INFO, "Hit %s for %d damage!", enemy.getName().c_str(), m_player.getAttackDamage());

                        if (enemy.getHealth() <= 0) {
                            enemy.setDefeated(true);
                            
                            int lootGold = 25;
                            m_player.addGold(lootGold);

                            int xpReward = enemy.getLevel() * 40;
                            m_player.addXP(xpReward);

                            if (enemy.getName() == "Magma Goblin" || enemy.getName() == "Goblin") {
                                for (auto& quest : m_quests) {
                                    if (quest.id == QuestID::GoblinHunt && quest.isAccepted && !quest.isCompleted) {
                                        quest.currentProgress++;
                                        if (quest.currentProgress >= quest.targetProgress) {
                                            completeQuest(quest);
                                        }
                                    }
                                }
                            }

                            Vector2 dropPos = enemy.getPosition();
                            currentSubArea->items.push_back(
                                WorldItem(
                                    ItemID::ManaPotion, 
                                    "Mana Potion", 
                                    "Restores a small amount of mana.", 
                                    1, 
                                    dropPos, 
                                    m_selectRealm, 
                                    ItemType::RestoreMana, 
                                    25, 
                                    false
                                )
                            );

                            showNotification("Enemy defeated! +25 Gold, +" + std::to_string(xpReward) + " XP");
                        }
                    }
                }
            }
        }
    }
}

void GameEngine::useHotbarItem(ItemType type) {
    bool itemUsed = false;
    for (size_t i = 0; i < m_inventory.size(); i++) {
        auto& item = m_inventory[i];
        if (item.type == type && item.quantity > 0) {
            if (type == ItemType::Heal) {
                m_player.heal(item.attribute);
                showNotification("Used " + item.name + " (+" + std::to_string(item.attribute) + " HP)");
            } else if (type == ItemType::RestoreMana) {
                m_player.addMana(item.attribute);
                showNotification("Used " + item.name + " (+" + std::to_string(item.attribute) + " Mana)");
            }

            m_inventory.removeItem(i, 1);
            
            itemUsed = true;
            break;
        }
    }
    if (!itemUsed) {
        showNotification("No items available!");
    }
}

void GameEngine::showFloatingText(const std::string text, Vector2 pos, Color color) {
    m_floatingTexts.push_back({ text, pos, color, 1.0f });
}

DialogueManager::DialogueManager() {}