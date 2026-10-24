#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H
#include <raylib.h>
#include <vector>
#include <string>
#include "../entities/Player.h"
#include "../dialogue/DialogueManager.h"

enum class GameState {
    WorldMap,
    RegionHub,
    Dialogue,
    Inventory
};

struct WallCollider {
    Rectangle bounds;
};

struct Buildings {
    std::string npc;
    std::string name;
    Rectangle building;
    Color roof;
    Rectangle interior;
    Color floor;
    Rectangle entrance;
    Color door;
    bool isPlayerInside = false;

    std::vector<WallCollider> walls;

    void generateWalls() {
        walls.clear();
        float wallThickness = 2.0f;

        walls.push_back({{ building.x, building.y, building.width, wallThickness }});

        walls.push_back({{ building.x, building.y, wallThickness, building.height }});

        walls.push_back({{ building.x + building.width - wallThickness, building.y, wallThickness, building.height }});

        float leftWidth = entrance.x - building.x;
        if (leftWidth > 0) {
            walls.push_back({{ building.x, building.y + building.height - wallThickness, leftWidth, wallThickness }});
        }

        float rightStartX = entrance.x + entrance.width;
        float rightWidth = (building.x + building.width) - rightStartX;
        if (rightWidth > 0) {
            walls.push_back({{ rightStartX, building.y + building.height - wallThickness, rightWidth, wallThickness }});
        }
    }

    void updateStatus(Vector2 playerPos) {
        isPlayerInside = CheckCollisionPointRec(playerPos, interior) || CheckCollisionPointRec(playerPos, entrance);
    }
};

struct RegionBuildings {
    Realm realm;
    std::vector<Buildings> buildings;
};

struct NPCGraphics {
    Vector2 npcRec;
    Vector2 npcPos;
    Color npcColor;
};

struct NPCInfo {
    std::string name;
    std::string description;
    std::string dialogueFile;
    std::string npcId;
    Vector2 npcCollision;
    NPCGraphics npcGraphics;
};

struct RegionMap {
    std::string name;
    Realm realm;
    Rectangle bounds;
    Color color;
};

struct RegionHubG {
    std::string name;
    Realm realm;
    std::vector<NPCInfo> npcs;
    Rectangle rectangle;
};

class GameEngine {
public:
    GameEngine();
    ~GameEngine();

    void run();

private:
    void update();
    void render();
    void renderMap();
    void renderMapWindow();
    void renderRegion();
    void renderUI();
    void renderMinimap();

    Player m_player;
    DialogueManager m_dialogueManager;
    GameState m_currentState;
    std::vector<NPCInfo> m_npcs;
    std::vector<RegionMap> m_regions;
    std::vector<RegionHubG> m_regionHub;
    std::vector<RegionBuildings> m_buildings;
    Realm m_selectRealm;
    Color renderColor;
    Music m_ignisSound;
    Music m_aquaSound;
    Camera2D m_camera;
    bool m_isMapWindowOpen = false;
    int m_mapRow = 0;
    int m_mapCol = 0;
};

#endif