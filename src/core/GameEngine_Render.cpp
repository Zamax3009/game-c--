#include "GameEngine.h"
#include "raylib.h"
#include "rlImGui.h"
#include "imgui.h"

void GameEngine::render() {
    BeginTextureMode(m_targetTexture);
    ClearBackground(DARKGRAY);
    rlImGuiBegin();

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(SCREENSIZEX, SCREENSIZEY);
    float scale = std::min((float)GetScreenWidth() / SCREENSIZEX, (float)GetScreenHeight() / SCREENSIZEY);
    float offsetX = (GetScreenWidth() - (SCREENSIZEX * scale)) * 0.5f;
    float offsetY = (GetScreenHeight() - (SCREENSIZEY * scale)) * 0.5f;

    Vector2 realMouse = GetMousePosition();
    io.MousePos.x = (realMouse.x - offsetX) / scale;
    io.MousePos.y = (realMouse.y - offsetY) / scale;


    if (m_currentStatePtr) {
        m_currentStatePtr->render(*this);
    }

    if (m_notificationTime > 0.0f) {
        renderNotification();
    }
    rlImGuiEnd();
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);

    Rectangle sourceRec = { 0.0f, 0.0f, (float)m_targetTexture.texture.width, -(float)m_targetTexture.texture.height };
    Rectangle destRec = {
        (GetScreenWidth() - (SCREENSIZEX * scale)) * 0.5f,
        (GetScreenHeight() - (SCREENSIZEY * scale)) * 0.5f,
        SCREENSIZEX * scale,
        SCREENSIZEY * scale
    };

    DrawTexturePro(m_targetTexture.texture, sourceRec, destRec, { 0, 0 }, 0.0f, WHITE);

    EndDrawing();

}
void GameEngine::renderNotification() {
    int fontSize = 16;
    int textWidth = MeasureText(m_notificationText.c_str(), fontSize);

    int paddingX = 30;
    int boxWidth = textWidth + (paddingX * 2);
    int boxHeight = 45;

    int boxX = (SCREENSIZEX - boxWidth) / 2;
    int boxY = 25;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(BLACK, 0.8f));
    DrawRectangleLinesEx({ static_cast<float>(boxX), static_cast<float>(boxY), static_cast<float>(boxWidth), static_cast<float>(boxHeight) }, 2, GOLD);
    DrawText(m_notificationText.c_str(), boxX + paddingX, boxY + (boxHeight - fontSize) / 2, 14, GOLD);
}