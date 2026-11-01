#ifndef MAIN_MENU_STATE_H
#define MAIN_MENU_STATE_H

#include "GameState.h"
#include "GameEngine.h"
#include "RegionHubState.h"
#include "WorldMapState.h"
#include "raylib.h"

class MainMenuState : public GameState {
public:
    void update(GameEngine& engine) override {
        bool saveExists = engine.doesSaveFileExist("savegame.sav");
        int maxOptions = saveExists ? 2 : 1;

        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            engine.m_menuIndex = (engine.m_menuIndex - 1 + maxOptions) % maxOptions;
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            engine.m_menuIndex = (engine.m_menuIndex + 1) % maxOptions;
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (engine.m_menuIndex == 0) {
                engine.m_player.resetToDefault();
                engine.m_inventory.clear();
                for (auto& wItem : engine.m_worldItems) {
                    wItem.setCollected(false);
                }
                // Passa alla mappa del mondo per una nuova avventura
                engine.changeState(std::make_unique<WorldMapState>());
            }
            else if (engine.m_menuIndex == 1 && saveExists) {
                engine.loadGame("savegame.sav");
                // Passa all'hub di regione dopo aver caricato la partita
                engine.changeState(std::make_unique<RegionHubState>());
            }
        }
    }

    void render(GameEngine& engine) override {
        DrawRectangle(0, 0, 1280, 720, DARKBLUE);
        DrawText("ELEMENTAL RPG", 1280 / 2 - MeasureText("ELEMENTAL RPG", 40) / 2, 150, 40, GOLD);

        bool saveExists = engine.doesSaveFileExist("savegame.sav");

        Color colorNewGame = (engine.m_menuIndex == 0) ? YELLOW : WHITE;
        DrawText("> New Adventure", 1280 / 2 - 100, 320, 22, colorNewGame);

        if (saveExists) {
            Color colorLoadGame = (engine.m_menuIndex == 1) ? YELLOW : WHITE;
            DrawText("> Load Save File", 1280 / 2 - 100, 370, 22, colorLoadGame);
        } else {
            DrawText(" Load Save File (no save file found)", 1280 / 2 - 100, 370, 22, GRAY);
        }
        DrawText("Use [W/S] or [Arrow Keys] to navigate, [ENTER] to select", 1280 / 2 - 220, 550, 14, LIGHTGRAY);
    }
};

#endif