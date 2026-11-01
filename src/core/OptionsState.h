#ifndef OPTIONS_STATE_H
#define OPTIONS_STATE_H

#include "GameState.h"
#include "GameEngine.h"
#include "raylib.h"
#include "imgui.h"

class OptionsState : public GameState {
private:
    float m_masterVolume = 1.0f;
    bool m_isFullscreen = false;

public:
    void enter(GameEngine& engine) override {
        m_masterVolume = GetMasterVolume();
        m_isFullscreen = IsWindowFullscreen();
    }

    void update(GameEngine& engine) override {
        if (IsKeyPressed(KEY_ESCAPE)) {
            engine.goToPause();
        }
    }

    void render(GameEngine& engine) override {
        DrawRectangle(0, 0, 1280, 720, Fade(BLACK, 0.6f));

        ImGui::SetNextWindowSize(ImVec2(500, 350), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2((1280 - 500) / 2.0f, (720 - 350) / 2.0f), ImGuiCond_Always);

        ImGui::Begin("Opzioni di Gioco", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        ImGui::Text("Regola le impostazioni di sistema");
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::SliderFloat("Volume Generale", &m_masterVolume, 0.0f, 1.0f, "%.2f")) {
            SetMasterVolume(m_masterVolume);
        }

        ImGui::Spacing();

        if (ImGui::Checkbox("Schermo Intero", &m_isFullscreen)) {
            ToggleFullscreen();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Indietro", ImVec2(120, 35))) {
            engine.goToPause();
        }

        ImGui::End();
    }
};

#endif