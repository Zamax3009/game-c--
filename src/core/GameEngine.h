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

struct Buildings {
    std::string name;
    Rectangle building;
    Color roof;
    Color floor;
    Rectangle entrance;
    Color door;
    bool isPlayerInside;

    void updateStatus(Vector2 playerPos) {
        isPlayerInside = CheckCollisionPointRec(playerPos, entrance) || CheckCollisionPointRec(playerPos, building);
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
    void renderRegion();
    void renderUI();

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
};

#endif