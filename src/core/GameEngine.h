#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H
#include <raylib.h>
#include <vector>
#include <string>
#include "raymath.h"
#include "../entities/Player.h"
#include "../dialogue/DialogueManager.h"

enum class GameState {
    MainMenu,
    WorldMap,
    RegionHub,
    Dialogue,
    Inventory,
    Combat,
    Shop
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
    Vector2 m_spawnPosition;
    Rectangle m_collisionBox;
    Color m_color;
    bool m_isDefeated;
    float m_speed = 1.5f;
    bool m_isReturning = false;

public:
    // Constructor
    Enemy(std::string name, int level, int hp, int attack, Vector2 pos, Rectangle box, Color col)
        : m_name(name), m_level(level), m_health(hp), m_maxHealth(hp), m_attackPower(attack), m_position(pos), m_spawnPosition(pos), m_collisionBox(box), m_color(col), m_isDefeated(false) {}

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

    // Enemy AI logic
    void updateAI(Vector2 playerPos) {
        if (m_isDefeated) return;
        
        float distToPlayer = Vector2Distance(m_position, playerPos);
        float distToSpawn = Vector2Distance(m_position, m_spawnPosition);
        float agroRange = 150.0f;
        float leashRange = 300.0f;

        if (distToSpawn > leashRange) {
            m_isReturning = true;
        }
        else if (distToSpawn < 10.0f && m_isReturning) {
            m_isReturning = false;
        }

        Vector2 target;
        if (m_isReturning) {
            target = m_spawnPosition;
        }
        else if (distToPlayer < agroRange) {
            target = playerPos;
        }
        else if (distToSpawn > 5.0f) {
            target = m_spawnPosition;
        }
        else {
            return;
        }

        float distToTarget = Vector2Distance(m_position, target);
        if (distToTarget <= m_speed) {
            m_position = target;
        } else {
            Vector2 direction = Vector2Normalize(Vector2Subtract(target, m_position));
            m_position = Vector2Add(m_position, Vector2Scale(direction, m_speed));
        }
        
    } 
};



class WorldItem {
private:
    ItemID m_id;
    std::string m_name;
    std::string m_description;
    int m_quantity;
    Vector2 m_position;
    Realm m_realm;
    ItemType m_type;
    int m_attribute;
    bool m_isCollected = false;
public:
    WorldItem(ItemID id, std::string name, std::string desc, int qty, Vector2 pos, Realm realm, ItemType type, int attr, bool collected = false)
        : m_id(id), m_name(name),m_description(desc), m_quantity(qty), m_position(pos), m_realm(realm), m_type(type), m_attribute(attr), m_isCollected(collected) {}
    
    //GGETTERS
    ItemID getID() const { return m_id; }
    std::string getName() const { return m_name; }
    std::string getDescription() const { return m_description; }
    int getQuantity() const { return m_quantity; }
    Vector2 getPosition() const { return m_position; }
    Realm getRealm() const { return m_realm; }
    ItemType getType() const { return m_type; }
    int getAttribute() const { return m_attribute; }
    bool isCollected() const { return m_isCollected; }

    //SETTERS
    void setCollected(bool collected) { m_isCollected = collected; }
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

struct ShopItem {
    ItemID id;
    std::string name;
    std::string description;
    int price;
    ItemType type;
    int attribute;
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
    void updateMainMenu();
    void updateWorldMap();
    void updateRegionHub();
    void updateDialogue();
    void updateCombat();
    void updateInventory();
    void updateShop();

    void render();
    void renderShop();
    void renderMainMenu();
    void renderInventory();
    void renderMap();
    void renderMapWindow();
    void renderRegion();
    void renderUI();
    void renderMinimap();
    void renderCombat();
    void renderNotification();
    void playerMovement(float velocity);
    void updateCamera(const RegionHubG* currentHub);
    void updateRegionHubNotMap(RegionHubG* currentHub);
    
    int getSelectedAttack();
    bool checkWallCollision(const Rectangle& rec);
    Item createItem(ItemID id, int quantity = 1);

    // FILE SAVE
    bool doesSaveFileExist(const std::string& filename);
    void saveGame(const std::string& filename);
    void loadGame(const std::string& filename);

    

    Player m_player;
    DialogueManager m_dialogueManager;
    GameState m_currentState;
    CombatPhase m_currentPhase = CombatPhase::ActionSelect;
    std::vector<NPCInfo> m_npcs;
    std::vector<RegionMap> m_regions;
    std::vector<RegionHubG> m_regionHub;
    std::vector<RegionBuildings> m_buildings;
    Inventory m_inventory;
    std::vector<WorldItem> m_worldItems;
    std::vector<ShopItem> m_shopItems;
    int m_shopIndex = 0;
    Realm m_selectRealm;
    Color renderColor;
    Music m_ignisSound;
    Music m_aquaSound;
    Camera2D m_camera;
    bool m_isMapWindowOpen = false;
    int m_mapRow = 0;
    int m_mapCol = 0;
    int m_menuIndex = 0;
    Enemy* m_activeEnemy = nullptr;
    int m_lastLootGold = 0;
    int m_inventoryIndex = 0;
    float m_notificationTime = 0;
    std::string m_notificationText = "";
};

#endif