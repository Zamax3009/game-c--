#ifndef DIALOGUE_STATE_H
#define DIALOGUE_STATE_H

#include "GameState.h"
#include "GameEngine.h"
#include "raylib.h"
#include "imgui.h"

class DialogueState : public GameState {
public:
    void update(GameEngine& engine) override {
        if (!engine.m_dialogueManager.isDialogueActive()) {
            engine.goToRegionHub();
        }
    }

    void render(GameEngine& engine) override {
        const DialogueNode* node = engine.m_dialogueManager.getCurrentNode();
        if (node) {
            ImGui::Begin("Dialogue", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::TextDisabled("[%s]", node->speaker.c_str());
            ImGui::TextWrapped("%s", node->text.c_str());
            ImGui::Separator();

            for (size_t i = 0; i < node->options.size(); i++) {
                if (ImGui::Button(node->options[i].text.c_str())) {
                    QuestID qId = engine.m_dialogueManager.selectOption(static_cast<int>(i), engine.m_player);
                    if (qId != QuestID::None) {
                        engine.acceptQuest(qId);
                    }
                }
            }
            ImGui::End();
        }
    }
};

#endif