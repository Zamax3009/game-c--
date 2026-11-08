#ifndef DIALOGUE_STATE_H
#define DIALOGUE_STATE_H

#include "GameState.h"
#include "GameEngine.h"
#include "raylib.h"

class DialogueState : public GameState {
private:
    int m_selectedOptionIndex = 0;

public:
    void enter(GameEngine& engine) override {
        m_selectedOptionIndex = 0;
        if (!engine.m_dialogueManager.isDialogueActive()) {
            TraceLog(LOG_WARNING, "Dialogue failed to start! Check if JSON file exists.");
            engine.goToRegionHub();
        }
    }

    void update(GameEngine& engine) override {
        if (!engine.m_dialogueManager.isDialogueActive()) {
            engine.goToRegionHub();
        }

        const DialogueNode* node = engine.m_dialogueManager.getCurrentNode();
        if (!node) return;

        int optionCount = static_cast<int>(node->options.size());

        if (optionCount > 0) {
            if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
                m_selectedOptionIndex = (m_selectedOptionIndex - 1 + optionCount) % optionCount;
            }
            if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
                m_selectedOptionIndex = (m_selectedOptionIndex + 1) % optionCount;
            }

            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                QuestID qId = engine.m_dialogueManager.selectOption(m_selectedOptionIndex, engine.m_player);
                if (qId != QuestID::None) {
                    engine.acceptQuest(qId);
                }
                m_selectedOptionIndex = 0;
            }
        } else {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                engine.goToRegionHub();
            }
        }
    }

    void render(GameEngine& engine) override {
        const DialogueNode* node = engine.m_dialogueManager.getCurrentNode();
        if (!node) return;

        DrawRectangle(0, 0, engine.SCREENSIZEX, engine.SCREENSIZEY, Fade(BLACK, 0.4f));

        float boxWidth = engine.SCREENSIZEX - 160.0f;
        float boxHeigh = 180.0f;
        float boxX = 80.0f;
        float boxY = engine.SCREENSIZEY - boxHeigh - 40.0f;

        DrawRectangle(boxX, boxY, boxWidth, boxHeigh, Fade(DARKGRAY, 0.95f));;
        DrawRectangleLinesEx({ boxX, boxY, boxWidth, boxHeigh}, 2, GOLD);

        float tagWidth = 200.0f;
        float tagHeight = 35.0f;
        DrawRectangle(boxX + 20, boxY - 15, tagWidth, tagHeight, BLACK);
        DrawRectangleLinesEx({ boxX + 20, boxY - 15, tagWidth, tagHeight}, 1, GOLD);
        DrawText(node->speaker.c_str(), boxX + 35.0f, boxY - 7.0f, 16, WHITE);
        DrawText(node->text.c_str(), boxX + 30.0f, boxY + 35.0f, 16, RAYWHITE);

        if (!node->options.empty()) {
            float optionStartY = boxY + 85.0f;
            for (size_t i = 0; i < node->options.size(); i++) {
                bool isSelected = (static_cast<int>(i) == m_selectedOptionIndex);
                Color textColor = isSelected ? GOLD : LIGHTGRAY;
                std::string prefix = isSelected ? "> " : "  ";
                std::string displayOptionText = prefix + node->options[i].text;
                DrawText(displayOptionText.c_str(), boxX + 40.0f, optionStartY + (i * 25.0f), 14, textColor);
            }
            DrawText("[W/S] Choose | [ENTER] Select", boxX + boxWidth - 240.0f, boxY + boxHeigh - 25.0f, 10, GRAY);
        } else {
            DrawText("[ENTER] Continue", boxX + boxWidth - 120.0f, boxY + boxHeigh - 25.0f, 10, GRAY);
        }
    }
};

#endif