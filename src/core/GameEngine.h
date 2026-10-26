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
    Inventory,
    Combat
};

enum class CombatPhase {
    ActionSelect,
    Victory,
    Defeat
};

class Enemy {
private:
    std::string m_name;
    int m_level;
    int m_health;
    int m_maxHealth;
    int m_attackPower;
    Vector2 m_position;
    Rectangle m_collisionBox;
    Color m_color;
    bool m_isDefeated;

public:
    // Constructor
    Enemy(std::string name, int level, int hp, int attack, Vector2 pos, Rectangle box, Color col)
        : m_name(name), m_level(level), m_health(hp), m_maxHealth(hp), m_attackPower(attack), m_position(pos), m_collisionBox(box), m_color(col), m_isDefeated(false) {}

    // Getters
    std::string getName() const { return m_name; }
    int getLevel() const { return m_level; }
    int getHealth() const { return m_health; }
    int getMaxHealth() const { return m_maxHealth; }
    int getAttackPower() const { return m_attackPower; }
    Vector2 getPosition() const { return m_position; }
    Rectangle getCollisionBox() const { return m_collisionBox; }
    Color getColor() const { return m_color; }
    bool isDefeated() const { return m_isDefeated; }

    // Setters
    void takeDamage(int amount) {
        m_health -= amount;
        if (m_health < 0) m_health = 0;
    }
    void setHealth(int hp) { m_health = hp; }
    void setDefeated(bool defeated) { m_isDefeated = defeated; }
};


struct Item {
    std::string name;
    std::string description;
    int quantity;
};

struct WorldItem {
    std::string name;
    std::string description;
    int quantity;
    Vector2 position;
    Realm realm;
    bool isCollected = false;
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
    std::vector<Enemy> enemies;
    Rectangle rectangle;
};

class GameEngine {
public:
    GameEngine();
    ~GameEngine();

    void run();

private:
    void update();
    void updateWorldMap();
    void updateRegionHub();
    void updateDialogue();
    void updateCombat();
    void updateInventory();
    void render();
    void renderInventory();
    void renderMap();
    void renderMapWindow();
    void renderRegion();
    void renderUI();
    void renderMinimap();
    void renderCombat();
    void playerMovement(float velocity);
    void updateCamera(const RegionHubG* currentHub);
    void updateRegionHubNotMap(RegionHubG* currentHub);
    
    int getSelectedAttack();
    bool checkWallCollision(const Rectangle& rec);

    

    Player m_player;
    DialogueManager m_dialogueManager;
    GameState m_currentState;
    CombatPhase m_currentPhase = CombatPhase::ActionSelect;
    std::vector<NPCInfo> m_npcs;
    std::vector<RegionMap> m_regions;
    std::vector<RegionHubG> m_regionHub;
    std::vector<RegionBuildings> m_buildings;
    std::vector<Item> m_inventory;
    std::vector<WorldItem> m_worldItems;
    Realm m_selectRealm;
    Color renderColor;
    Music m_ignisSound;
    Music m_aquaSound;
    Camera2D m_camera;
    bool m_isMapWindowOpen = false;
    int m_mapRow = 0;
    int m_mapCol = 0;
    Enemy* m_activeEnemy = nullptr;
    int m_lastLootGold = 0;
};

#endif