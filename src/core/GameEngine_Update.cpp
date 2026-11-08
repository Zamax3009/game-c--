#include "GameEngine.h"
#include "raylib.h"
#include "raymath.h"
#include "imgui.h"
#include "rlImGui.h"

void GameEngine::update() {

    if (IsKeyPressed(KEY_F5)) {
        saveGame("savegame.sav");
        showNotification("Game saved successfully!");
    }

    if (m_player.getLevel() > m_lastPlayerLevel) {
        m_lastPlayerLevel = m_player.getLevel();
        showNotification("LEVEL UP! Reached Lv. " + std::to_string(m_lastPlayerLevel));
    }

    if (m_notificationTime > 0.0f) {
        m_notificationTime -= GetFrameTime();
    }

    if (m_currentStatePtr) {
        m_currentStatePtr->update(*this);
    }
}

void GameEngine::updateCamera(const SubArea* currentSubArea) {
    if (!currentSubArea) return;

    Vector2 cameraTarget = m_player.getPosition();
    float halfScreenWidth = (SCREENSIZEX / 2.0f) / m_camera.zoom;
    float halfScreenHeight = (SCREENSIZEY / 2.0f) / m_camera.zoom;

    float minCameraX = currentSubArea->bounds.x + halfScreenWidth;
    float maxCameraX = currentSubArea->bounds.x + currentSubArea->bounds.width - halfScreenWidth;
    if (cameraTarget.x < minCameraX) cameraTarget.x = minCameraX;
    if (cameraTarget.x > maxCameraX) cameraTarget.x = maxCameraX;

    float minCameraY = currentSubArea->bounds.y + halfScreenHeight;
    float maxCameraY = currentSubArea->bounds.y + currentSubArea->bounds.height - halfScreenHeight;
    if (cameraTarget.y < minCameraY) cameraTarget.y = minCameraY;
    if (cameraTarget.y > maxCameraY) cameraTarget.y = maxCameraY;

    m_camera.target = cameraTarget;
}