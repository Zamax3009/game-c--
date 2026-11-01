#ifndef INVENTORY_STATE_H
#define INVENTORY_STATE_H

#include "GameState.h"
#include "GameEngine.h"
#include "raylib.h"

class InventoryState : public GameState {
public:
    void update(GameEngine& engine) override {
        if (IsKeyPressed(KEY_I) || IsKeyPressed(KEY_ESCAPE)) {
            engine.goToRegionHub();
            return;
        }

        if (engine.m_inventory.isEmpty()) return;

        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
            engine.m_inventoryIndex = (engine.m_inventoryIndex - 1 + engine.m_inventory.size()) % engine.m_inventory.size();
        }
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
            engine.m_inventoryIndex = (engine.m_inventoryIndex + 1) % engine.m_inventory.size();
        }

        if (IsKeyPressed(KEY_E) || IsKeyPressed(KEY_ENTER)) {
            auto& item = engine.m_inventory[engine.m_inventoryIndex];
            bool itemUsed = false;
            
            switch (item.type) {
                case ItemType::Heal:
                    engine.m_player.heal(item.attribute);
                    itemUsed = true;
                    break;
                case ItemType::RestoreMana:
                    engine.m_player.addMana(item.attribute);
                    itemUsed = true;
                    break;
                case ItemType::None:
                default:
                    break;
            }

            if (itemUsed) {
                item.quantity--;
                if (item.quantity <= 0) {
                    engine.m_inventory.removeItem(engine.m_inventoryIndex);
                    if (engine.m_inventoryIndex >= engine.m_inventory.size() && !engine.m_inventory.isEmpty()) {
                        engine.m_inventoryIndex = engine.m_inventory.size() - 1;
                    }
                }
            }
        }
    }

    void render(GameEngine& engine) override {
        DrawRectangle(0, 0, 1280, 720, Fade(BLACK, 0.75f));

        float invWidth = 600.0f;
        float invHeight = 450.0f;
        float invX = (1280.0f - invWidth) / 2.0f;
        float invY = (720.0f - invHeight) / 2.0f;

        DrawRectangle(invX, invY, invWidth, invHeight, DARKGRAY);
        DrawRectangleLines(invX, invY, invWidth, invHeight, WHITE);
        DrawText("INVENTORY", invX + 30, invY + 25, 22, GOLD);
        DrawText("Press [I] or [ESC] to close", invX + 30, invY + 55, 12, LIGHTGRAY);

        float startY = invY + 100.0f;
        if (engine.m_inventory.isEmpty()) {
            DrawText("Inventory is empty", invX + 30, startY, 16, WHITE);
        } else {
            for (size_t i = 0; i < engine.m_inventory.size(); i++) {
                const auto& item = engine.m_inventory[i];
                float itemY = startY + (i * 40.0f);

                Color rowColor = (i == static_cast<size_t>(engine.m_inventoryIndex)) ? Fade(YELLOW, 0.25f) : Fade(BLACK, 0.3f);
                DrawRectangle(invX + 25, itemY, invWidth - 50, 32, rowColor);

                std::string prefix = (i == static_cast<size_t>(engine.m_inventoryIndex)) ? "> " : "  ";
                std::string itemText = prefix + item.name + "  x" + std::to_string(item.quantity);

                DrawText(itemText.c_str(), invX + 40, itemY + 8, 14, WHITE);
                DrawText(item.description.c_str(), invX + 250, itemY + 8, 12, LIGHTGRAY);
            }
        }
        DrawText("Press [ENTER] or [E] to use item", invX + 30, invY + invHeight - 35, 12, YELLOW);
    }
};

#endif