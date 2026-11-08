#ifndef COMBAT_STATE_H
#define COMBAT_STATE_H

#include "GameState.h"
#include "GameEngine.h"
#include "raylib.h"
#include "raymath.h"
#include <algorithm>

class CombatState : public GameState {
public:
    void enter(GameEngine& engine) override {
        engine.m_currentPhase = CombatPhase::ActionSelect;
    }

    void update(GameEngine& engine) override {
        if (!engine.m_activeEnemy) return;

        if (engine.m_currentPhase == CombatPhase::ActionSelect) {
            int selectedAction = engine.getSelectedAttack();

            if (selectedAction != -1) {
                const auto& attacks = engine.m_player.getAttacks();
                const Attack& chosenAttack = attacks[selectedAction];
                
                if (engine.m_player.getMana() >= chosenAttack.manaUse) {
                    engine.m_player.useMana(chosenAttack.manaUse);
                    
                    if (chosenAttack.damage > 0) {
                        engine.m_activeEnemy->takeDamage(chosenAttack.damage);
                    }
                    if (chosenAttack.healing > 0) {
                        engine.m_player.heal(chosenAttack.healing);
                    }
                    
                    if (engine.m_activeEnemy->getHealth() <= 0) {
                        engine.m_activeEnemy->setDefeated(true);
                        engine.m_lastLootGold = 25;
                        engine.m_player.addGold(engine.m_lastLootGold);
                        int xpReward = engine.m_activeEnemy->getLevel() * 40;
                        engine.m_player.addXP(xpReward);

                        if (engine.m_activeEnemy->getName() == "Goblin") {
                            for (auto& quest : engine.m_quests) {
                                if (quest.id == QuestID::GoblinHunt && quest.isAccepted && !quest.isCompleted) {
                                    quest.currentProgress++;
                                    if (quest.currentProgress >= quest.targetProgress) {
                                        engine.completeQuest(quest);
                                    }
                                }
                            }
                        }
                        Vector2 dropPos = engine.m_activeEnemy->getPosition();
                        for (auto& hub : engine.m_regionHub) {
                            if (hub.realm == engine.m_selectRealm) {
                                for (auto& sa : hub.areas) {
                                    if (sa.id == hub.currenSubAreaId) {
                                        sa.items.push_back(
                                            WorldItem(
                                                ItemID::HealthElixir, 
                                                "Health Elixir", 
                                                "Heals a small amount of damage.", 
                                                1, 
                                                dropPos, 
                                                engine.m_selectRealm, 
                                                ItemType::Heal, 
                                                35, 
                                                false
                                            )
                                        );
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                        engine.m_currentPhase = CombatPhase::Victory;
                    } else {
                        engine.m_player.takeDamage(engine.m_activeEnemy->getAttackPower());

                        if (engine.m_player.getHP() <= 0) {
                            engine.m_currentPhase = CombatPhase::Defeat;
                        }
                    }
                }
            }

            if (IsKeyPressed(KEY_H)) {
                bool itemUsed = false;
                for (size_t i = 0; i < engine.m_inventory.size(); i++) {
                    auto& item = engine.m_inventory[i];
                    if (item.type == ItemType::Heal && item.quantity > 0) {
                        engine.m_player.heal(item.attribute);
                        item.quantity--;

                        if (item.quantity <= 0) {
                            engine.m_inventory.removeItem(i);
                        }

                        itemUsed = true;
                        TraceLog(LOG_INFO, "Health potion used in combat");
                        break;
                    }
                }
                if (itemUsed) {
                    engine.m_player.takeDamage(engine.m_activeEnemy->getAttackPower());

                    if (engine.m_player.getHP() <= 0) {
                        engine.m_currentPhase = CombatPhase::Defeat;
                    }
                } else {
                    TraceLog(LOG_WARNING, "No health elixirs available in inventory.");
                }
            }

            if (IsKeyPressed(KEY_R)) {
                engine.m_activeEnemy = nullptr;
                engine.goToRegionHub();
            }
        }
        else if (engine.m_currentPhase == CombatPhase::Victory) {
            if (IsKeyPressed(KEY_ENTER)) {
                engine.m_activeEnemy = nullptr;
                engine.m_currentPhase = CombatPhase::ActionSelect;
                engine.goToRegionHub();
            }
        }
        else { // Defeat
            if (IsKeyPressed(KEY_ENTER)) {
                engine.m_activeEnemy = nullptr;
                engine.m_player.resetHP();
                engine.m_currentPhase = CombatPhase::ActionSelect;
                engine.goToRegionHub();
            }
        }
    }

    void render(GameEngine& engine) override {
        DrawRectangle(0, 0, engine.SCREENSIZEX, engine.SCREENSIZEY, Fade(BLACK, 0.75f));

        if (!engine.m_activeEnemy) return;

        float arenaWidth = 700.0f;
        float arenaHeight = 450.0f;
        float arenaX = (engine.SCREENSIZEX - arenaWidth) / 2.0f;
        float arenaY = (engine.SCREENSIZEY - arenaHeight) / 2.0f;

        DrawRectangle(arenaX, arenaY, arenaWidth, arenaHeight, DARKGRAY);
        DrawRectangleLinesEx({arenaX, arenaY, arenaWidth, arenaHeight}, 2, RED);
        DrawText("COMBAT", arenaX + 30, arenaY + 15, 20, RED);

        DrawText(engine.m_activeEnemy->getName().c_str(), arenaX + 400, arenaY + 40, 20, RED);
        DrawText(engine.m_player.getName().c_str(), arenaX + 50, arenaY + 40, 20, GOLD);

        if (engine.m_currentPhase == CombatPhase::ActionSelect) {
            std::string enemyHP = "HP: " + std::to_string(engine.m_activeEnemy->getHealth()) + " / " + std::to_string(engine.m_activeEnemy->getMaxHealth());
            DrawText(enemyHP.c_str(), arenaX + 400, arenaY + 120, 16, LIGHTGRAY);

            float barWidth = 220.0f;
            float hpPercent = static_cast<float>(engine.m_activeEnemy->getHealth()) / static_cast<float>(engine.m_activeEnemy->getMaxHealth());

            DrawRectangle(arenaX + 400, arenaY + 140, barWidth, 18, BLACK);
            DrawRectangle(arenaX + 400, arenaY + 140, barWidth * hpPercent, 18, RED);
            DrawRectangleLines(arenaX + 400, arenaY + 140, barWidth, 18, WHITE);

            DrawText(engine.m_player.getName().c_str(), arenaX + 50, arenaY + 80, 22, GREEN);

            std::string playerHPText = "HP: " + std::to_string(engine.m_player.getHP()) + " / " + std::to_string(engine.m_player.getMaxHP());
            DrawText(playerHPText.c_str(), arenaX + 50, arenaY + 120, 16, LIGHTGRAY);

            float playerBarWidth = 220.0f;
            float playerHpPercent = static_cast<float>(engine.m_player.getHP()) / static_cast<float>(engine.m_player.getMaxHP());
            DrawRectangle(arenaX + 50, arenaY + 140, playerBarWidth, 18, BLACK);
            DrawRectangle(arenaX + 50, arenaY + 140, playerBarWidth * playerHpPercent, 18, GREEN);
            DrawRectangleLines(arenaX + 50, arenaY + 140, playerBarWidth, 18, WHITE);

            std::string playerManaText = "Mana: " + std::to_string(engine.m_player.getMana()) + " / " + std::to_string(engine.m_player.getMaxMana());
            DrawText(playerManaText.c_str(), arenaX + 50, arenaY + 170, 16, LIGHTGRAY);

            float playerManaPercent = static_cast<float>(engine.m_player.getMana()) / static_cast<float>(engine.m_player.getMaxMana());
            DrawRectangle(arenaX + 50, arenaY + 190, playerBarWidth, 18, BLACK);
            DrawRectangle(arenaX + 50, arenaY + 190, playerBarWidth * playerManaPercent, 18, BLUE);
            DrawRectangleLines(arenaX + 50, arenaY + 190, playerBarWidth, 18, WHITE);

            const auto& attacks = engine.m_player.getAttacks();
            int attackCount = static_cast<int>(attacks.size());
            
            DrawRectangle(arenaX + 50, arenaY + 260, 600, 120, Fade(BLACK, 0.5f));
            DrawRectangleLines(arenaX + 50, arenaY + 260, 600, 120, WHITE);

            int offsetY = 275;
            int index = 1;
            for (const auto& atk : engine.m_player.getAttacks()) {
                std::string label = "[" + std::to_string(index) + "]" + atk.name + "(Damage: " + std::to_string(atk.damage) + " | Healing: " + std::to_string(atk.healing) + ")" + " Mana: " + std::to_string(atk.manaUse);
                DrawText(label.c_str(), arenaX + 70, arenaY + offsetY, 14, GRAY);
                offsetY += 25;
                index++;
            }

            DrawText("Press [R] to Run", arenaX + 430, arenaY + 300, 16, YELLOW);
            DrawText("Press [H] to use Health Potion", arenaX + 430, arenaY + 330, 16, GREEN);
        }
        else if (engine.m_currentPhase == CombatPhase::Victory) {
            DrawRectangle(arenaX + 50, arenaY + 120, 600, 130, Fade(DARKGREEN, 0.9f));
            DrawRectangleLines(arenaX + 50, arenaY + 120, 600, 130, GREEN);

            DrawText("VICTORY!", arenaX + 250, arenaY + 140, 24, GOLD);
            std::string lootMsg = "You obtained " + std::to_string(engine.m_lastLootGold) + " gold coins!";
            DrawText(lootMsg.c_str(), arenaX + 190, arenaY + 175, 16, WHITE);
            DrawText("Press [ENTER] to continue...", arenaX + 180, arenaY + 210, 14, LIGHTGRAY);
        }
        else if (engine.m_currentPhase == CombatPhase::Defeat) {
            DrawRectangle(arenaX + 50, arenaY + 120, 600, 130, Fade(MAROON, 0.9f));
            DrawRectangleLines(arenaX + 50, arenaY + 120, 600, 130, RED);

            DrawText("DEFEAT!", arenaX + 180, arenaY + 140, 24, RED);
            DrawText("Your journey doesn't end here! Resume!", arenaX + 150, arenaY + 175, 16, WHITE);
            DrawText("Press [ENTER] to wake up...", arenaX + 180, arenaY + 210, 14, LIGHTGRAY);
        }
    }
};

#endif