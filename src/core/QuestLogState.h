#ifndef QUEST_LOG_STATE_H
#define QUEST_LOG_STATE_H

#include "GameState.h"
#include "GameEngine.h"
#include "raylib.h"

class QuestLogState : public GameState {
public:
    void update(GameEngine& engine) override {
        // Chiudi il registro missioni e torna all'hub con [J] o [I]
        if (IsKeyPressed(KEY_J) || IsKeyPressed(KEY_I)) {
            engine.goToRegionHub();
            return;
        }
        
        if (engine.m_quests.empty()) return;

        // Navigazione verticale tra le quest
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            engine.m_questLogIndex = (engine.m_questLogIndex - 1 + engine.m_quests.size()) % engine.m_quests.size();
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            engine.m_questLogIndex = (engine.m_questLogIndex + 1) % engine.m_quests.size();
        }
    }

    void render(GameEngine& engine) override {
        DrawRectangle(0, 0, 1280, 720, Fade(BLACK, 0.75f));

        float boxWidth = 700.0f;
        float boxHeight = 450.0f;
        float boxX = (1280.0f - boxWidth) / 2.0f;
        float boxY = (720.0f - boxHeight) / 2.0f;

        DrawRectangle(boxX, boxY, boxWidth, boxHeight, DARKGRAY);
        DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, GOLD);

        DrawText("QUEST DIARY", boxX + 30, boxY + 25, 22, GOLD);
        DrawText("Press [J] or [I] to exit", boxX + 30, boxY + 55, 12, LIGHTGRAY);

        float startY = boxY + 100.0f;

        if (engine.m_quests.empty()) {
            DrawText("No active quests in your diary.", boxX + 40, startY, 16, WHITE);
        } else {
            for (size_t i = 0; i < engine.m_quests.size(); i++) {
                const auto& quest = engine.m_quests[i];
                float itemY = startY + (i * 80.0f);

                Color rowColor = (i == static_cast<size_t>(engine.m_questLogIndex)) ? Fade(GOLD, 0.2f) : Fade(BLACK, 0.3f);
                DrawRectangle(boxX + 25, itemY, boxWidth - 50, 70, rowColor);

                std::string prefix = (i == static_cast<size_t>(engine.m_questLogIndex)) ? "> " : "  ";
                std::string statusText = quest.isCompleted ? "[COMPLETED]" : (quest.isAccepted ? "[IN PROGRESS]" : "[NOT STARTED]");
                Color statusColor = quest.isCompleted ? GREEN : (quest.isAccepted ? YELLOW : GRAY);

                DrawText((prefix + quest.title).c_str(), boxX + 40, itemY + 10, 16, WHITE);
                DrawText(statusText.c_str(), boxX + boxWidth - 160, itemY + 10, 12, statusColor);
                DrawText(quest.description.c_str(), boxX + 40, itemY + 32, 12, LIGHTGRAY);

                std::string progressStr = "Progress: " + std::to_string(quest.currentProgress) + " / " + std::to_string(quest.targetProgress);
                std::string rewardStr = "Reward: " + std::to_string(quest.rewardGold) + " Gold, " + std::to_string(quest.rewardXP) + " XP";

                DrawText(progressStr.c_str(), boxX + 40, itemY + 48, 11, SKYBLUE);
                DrawText(rewardStr.c_str(), boxX + 350, itemY + 48, 11, ORANGE);
            }
        }
    }
};

#endif