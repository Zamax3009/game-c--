#ifndef PAUSE_STATE_H
#define PAUSE_STATE_H

#include "GameState.h"
#include "GameEngine.h"
#include "raylib.h"

class PauseState : public GameState {
public:
    void update(GameEngine& engine) override {
        if (IsKeyPressed(KEY_ESCAPE)) {
            engine.goToRegionHub();
        }

        if (IsKeyPressed(KEY_M)) {
            engine.goToMainMenu();
        }
        if (IsKeyPressed(KEY_O)) {
            engine.goToOptions();
        }
    }

    void render(GameEngine& engine) override {
        DrawRectangle(0, 0, 1280, 720, Fade(BLACK, 0.75f));

        float boxWidth = 400.0f;
        float boxHeight = 300.0f;
        float boxX = (1280.0f - boxWidth) / 2.0f;
        float boxY = (720.0f - boxHeight) / 2.0f;

        DrawRectangle(boxX, boxY, boxWidth, boxHeight, DARKGRAY);
        DrawRectangleLinesEx({boxX, boxY, boxWidth, boxHeight}, 2, GOLD);

        DrawText("GAME PAUSED", boxX + (boxWidth - MeasureText("GAME PAUSED", 22)) / 2, boxY + 30, 22, GOLD);
        DrawText("Press [ESC] Resume", boxX + 50, boxY + 110, 14, WHITE);
        DrawText("Press [O] for Options", boxX + 50, boxY + 160, 14, LIGHTGRAY);
        DrawText("Press [M] to Main Menu", boxX + 50, boxY + 200, 14, GRAY);
    }
};

#endif