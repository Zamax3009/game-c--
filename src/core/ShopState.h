#ifndef SHOP_STATE_H
#define SHOP_STATE_H

#include "GameState.h"
#include "GameEngine.h"
#include "raylib.h"

class ShopState : public GameState {
public:
    void update(GameEngine& engine) override {
        // Chiudi il negozio e torna all'hub con [I]
        if (IsKeyPressed(KEY_I)) {
            engine.goToRegionHub();
            return;
        }

        // Cambia tra Acquisto e Vendita con [TAB] o [V]
        if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_V)) {
            engine.m_isShopSelling = !engine.m_isShopSelling;
            engine.m_shopIndex = 0;
        }

        if (!engine.m_isShopSelling) {
            if (engine.m_shopItems.empty()) return;

            if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
                engine.m_shopIndex = (engine.m_shopIndex - 1 + engine.m_shopItems.size()) % engine.m_shopItems.size();
            }
            if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
                engine.m_shopIndex = (engine.m_shopIndex + 1) % engine.m_shopItems.size();
            }

            // Acquista oggetto
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_E)) {
                const auto& shopItem = engine.m_shopItems[engine.m_shopIndex];

                if (engine.m_player.payGold(shopItem.price)) {
                    Item purchasedItem = engine.createItem(shopItem.id, 1);
                    engine.m_inventory.addItem(purchasedItem);
                    TraceLog(LOG_INFO, "Item purchased successfully.");
                } else {
                    TraceLog(LOG_WARNING, "Not enough gold.");
                }
            }
        } else {
            if (engine.m_inventory.isEmpty()) return;

            if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
                engine.m_shopIndex = (engine.m_shopIndex - 1 + engine.m_inventory.size()) % engine.m_inventory.size();
            }
            if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
                engine.m_shopIndex = (engine.m_shopIndex + 1) % engine.m_inventory.size();
            }
            
            // Vendi oggetto
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_E)) {
                if (engine.m_shopIndex < engine.m_inventory.size()) {
                    auto& item = engine.m_inventory[engine.m_shopIndex];

                    int sellPrice = 10;
                    if (item.id == ItemID::AncientScroll || item.id == ItemID::ElementalCrystal) sellPrice = 25;

                    engine.m_player.addGold(sellPrice);
                    engine.m_inventory.removeItem(engine.m_shopIndex, 1);

                    if (engine.m_shopIndex >= engine.m_inventory.size() && !engine.m_inventory.isEmpty()) {
                        engine.m_shopIndex = engine.m_inventory.size() - 1;
                    }

                    TraceLog(LOG_INFO, "Item sold successfully");
                }
            }
        }
    }

    void render(GameEngine& engine) override {
        DrawRectangle(0, 0, engine.SCREENSIZEX, engine.SCREENSIZEY, Fade(BLACK, 0.75f));

        float shopWidth = 650.0f;
        float shopHeight = 450.0f;
        float shopX = (engine.SCREENSIZEX - shopWidth) / 2.0f;
        float shopY = (engine.SCREENSIZEY - shopHeight) / 2.0f;

        DrawRectangle(shopX, shopY, shopWidth, shopHeight, DARKGRAY);
        DrawRectangleLines(shopX, shopY, shopWidth, shopHeight, GOLD);

        std::string title = !engine.m_isShopSelling ? "MERCHANT SHOP - BUY ITEMS" : "MERCHANT SHOP - SELL ITEMS";
        DrawText(title.c_str(), shopX + 30, shopY + 25, 22, GOLD);

        std::string goldText = "Gold: " + std::to_string(engine.m_player.getGold()) + " coins";
        DrawText(goldText.c_str(), shopX + shopWidth - 180, shopY + 28, 16, YELLOW);
        DrawText("Press [TAB] or [V] to switch Buy-Sell | [I] to exit", shopX + 30, shopY + 55, 12, LIGHTGRAY);

        float startY = shopY + 100.0f;
        if (!engine.m_isShopSelling) {
            for (size_t i = 0; i < engine.m_shopItems.size(); i++) {
                const auto& item = engine.m_shopItems[i];
                float itemY = startY + (i * 50.0f);

                Color rowColor = (i == static_cast<size_t>(engine.m_shopIndex)) ? Fade(GOLD, 0.25f) : Fade(BLACK, 0.3f);
                DrawRectangle(shopX + 25, itemY, shopWidth - 50, 40, rowColor);

                std::string prefix = (i == static_cast<size_t>(engine.m_shopIndex)) ? "> " : "  ";
                std::string itemText = prefix + item.name + "  -  Price:  " + std::to_string(item.price) + " gold";
                
                DrawText(itemText.c_str(), shopX + 40, itemY + 12, 14, WHITE);
                DrawText(item.description.c_str(), shopX + 350, itemY + 12, 14, LIGHTGRAY);
            }
            DrawText("Press [E] or [ENTER] to buy item", shopX + 30, shopY + shopHeight - 35, 12, YELLOW);
        } else {
            if (engine.m_inventory.isEmpty()) {
                DrawText("You have nothing to sell", shopX + 40, startY, 16, WHITE);
            } else {
                for (size_t i = 0; i < engine.m_inventory.size(); i++) {
                    const auto& item = engine.m_inventory[i];
                    float itemY = startY + (i * 50);

                    Color rowColor = (i == static_cast<size_t>(engine.m_shopIndex)) ? Fade(YELLOW, 0.25f) : Fade(BLACK, 0.3f);
                    DrawRectangle(shopX + 25, itemY, shopWidth - 50, 40, rowColor);

                    std::string prefix = (i == static_cast<size_t>(engine.m_shopIndex)) ? "> " : "  ";
                    std::string itemText = prefix + item.name + " (x" + std::to_string(item.quantity) + ") - Sell";

                    DrawText(itemText.c_str(), shopX + 40, itemY + 12, 14, WHITE);
                    DrawText(item.description.c_str(), shopX + 350, itemY + 12, 14, LIGHTGRAY);
                }
                DrawText("Press [E] or [ENTER] to sell item", shopX + 30, shopY + shopHeight - 35, 12, YELLOW);
            }
        }
    }
};

#endif