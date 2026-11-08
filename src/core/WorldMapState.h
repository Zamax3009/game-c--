#ifndef WORLD_MAP_STATE_H
#define WORLD_MAP_STATE_H

#include "GameState.h"
#include "GameEngine.h"
#include "raylib.h"

class WorldMapState : public GameState {
public:
    void update(GameEngine& engine) override {
        engine.playerMovement(4.0f);
        Vector2 playerPos = engine.m_player.getPosition();
        engine.m_camera.target = engine.m_player.getPosition();
        
        if (IsKeyPressed(KEY_F)) {
            for (const auto& region : engine.m_regions) {
                if (CheckCollisionPointRec(playerPos, region.bounds)) {
                    engine.m_selectRealm = region.realm;
                    
                    if (engine.m_selectRealm == Realm::Aqua) PlayMusicStream(engine.m_aquaSound);
                    else if (engine.m_selectRealm == Realm::Ignis) PlayMusicStream(engine.m_ignisSound);

                    for (auto& hub : engine.m_regionHub) {
                        if (hub.realm == engine.m_selectRealm) {
                            hub.currenSubAreaId = AreaID::Capital;

                            for (const auto& subArea : hub.areas) {
                                if (subArea.id == AreaID::Capital) {
                                    engine.m_player.setPosition({
                                        subArea.bounds.x + (subArea.bounds.width / 2),
                                        subArea.bounds.y + (subArea.bounds.height / 2)
                                    });
                                }
                            }
                            break;
                        }
                    }

                    engine.goToRegionHub();
                    break;
                }
            }
        }
    }

    void render(GameEngine& engine) override {
        BeginMode2D(engine.m_camera);

        Vector2 playerPos = engine.m_player.getPosition();
        DrawText("World map: choose a destination", 450, 40, 20, WHITE);
        
        for (const auto& region : engine.m_regions){
            bool isPlayerInside = CheckCollisionPointRec(playerPos, region.bounds);

            Color renderColor = region.color;
            if (isPlayerInside){
                renderColor = ColorAlpha(region.color, 0.6f);
            }

            DrawRectangleRec(region.bounds, renderColor);
            
            Color borderColor = isPlayerInside ? WHITE : BLACK;
            int borderWidth = isPlayerInside ? 3 : 1;
            
            DrawRectangleLinesEx(region.bounds, borderWidth, borderColor);
            DrawText(region.name.c_str(), region.bounds.x + 15, region.bounds.y + 15, 14, WHITE);

            if (isPlayerInside) {
                DrawText("Press [F] to enter...", region.bounds.x + 20, region.bounds.y + 60, 14, LIGHTGRAY);
            }
        }

        DrawRectangleV(playerPos, {10.0f, 15.0f}, YELLOW);

        EndMode2D();
    }
};

#endif