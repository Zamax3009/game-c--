#include "GameEngine.h"
#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "rlImGui.h"

void GameEngine::render() {
    BeginDrawing();
    ClearBackground(DARKGRAY);
    rlImGuiBegin();

    if (m_currentStatePtr) {
        m_currentStatePtr->render(*this);
    }

    if (m_notificationTime > 0.0f) {
        renderNotification();
    }
    rlImGuiEnd();
    EndDrawing();
}
void GameEngine::renderNotification() {
    int fontSize = 16;
    int textWidth = MeasureText(m_notificationText.c_str(), fontSize);

    int paddingX = 30;
    int boxWidth = textWidth + (paddingX * 2);
    int boxHeight = 45;

    int boxX = (1280 - boxWidth) / 2;
    int boxY = 25;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(BLACK, 0.8f));
    DrawRectangleLinesEx({ static_cast<float>(boxX), static_cast<float>(boxY), static_cast<float>(boxWidth), static_cast<float>(boxHeight) }, 2, GOLD);
    DrawText(m_notificationText.c_str(), boxX + paddingX, boxY + (boxHeight - fontSize) / 2, 14, GOLD);
}