#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <raylib.h>
#include <vector>
#include <string>
#include <memory>
#include "raymath.h"
#include "GameState.h"
#include "../entities/Player.h"
#include "../dialogue/DialogueManager.h"

enum class AreaID {
    None = 0,
    Village,
    Capital,
    ScorchedSteppes,
    AncientTitanForge,
    MountPyroclast
};

enum class QuestID {
    None = 0,
    GoblinHunt,
    CrystalGathering
};

enum class CombatPhase {
    ActionSelect,
    Victory,
    Defeat
};

struct WallCollider {
    Rectangle bounds;
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
    float m_speed = 1.2f;
    bool m_isReturning = false;

public:
    Enemy(std::string name, int level, int hp, int attack, Vector2 pos, Rectangle box, Color col)
        : m_name(name), m_level(level), m_health(hp), m_maxHealth(hp), m_attackPower(attack), 
          m_position(pos), m_spawnPosition(pos), m_color(col), m_isDefeated(false) {
        float w = (box.width > 0.0f) ? box.width : (box.x > 0.0f ? box.x : 20.0f);
        float h = (box.height > 0.0f) ? box.height : (box.y > 0.0f ? box.y : 20.0f);
        m_collisionBox = { pos.x - (w / 2.0f), pos.y - (h / 2.0f), w, h };
    }
    std::string getName() const { return m_name; }
    int getLevel() const { return m_level; }
    int getHealth() const { return m_health; }
    int getMaxHealth() const { return m_maxHealth; }
    int getAttackPower() const { return m_attackPower; }
    Vector2 getPosition() const { return m_position; }
    Rectangle getCollisionBox() const { return m_collisionBox; }
    Color getColor() const { return m_color; }
    bool isDefeated() const { return m_isDefeated; }

    void takeDamage(int amount) {
        m_health -= amount;
        if (m_health < 0) m_health = 0;
    }
    void setHealth(int hp) { m_health = hp; }
    void setDefeated(bool defeated) { m_isDefeated = defeated; }

    void updateAI(Vector2 playerPos, const std::vector<WallCollider>& walls, Rectangle hubBounds) {
        if (m_isDefeated) return;
        
        float distToPlayer = Vector2Distance(m_position, playerPos);
        float distToSpawn = Vector2Distance(m_position, m_spawnPosition);
        float agroRange = 160.0f;
        float leashRange = 320.0f;

        if (distToSpawn > leashRange) {
            m_isReturning = true;
        } else if (distToSpawn < 10.0f && m_isReturning) {
            m_isReturning = false;
        }

        Vector2 target;
        if (m_isReturning) {
            target = m_spawnPosition;
        } else if (distToPlayer < agroRange) {
            target = playerPos;
        } else if (distToSpawn > 5.0f) {
            target = m_spawnPosition;
        } else {
            return;
        }

        float distToTarget = Vector2Distance(m_position, target);
        if (distToTarget <= m_speed) {
            m_position = target;
            return;
        }
        
        Vector2 direction = Vector2Normalize(Vector2Subtract(target, m_position));

        float nextX = m_position.x + (direction.x * m_speed);
        Rectangle nextRectX = {
            nextX - (m_collisionBox.width / 2.0f),
            m_position.y - (m_collisionBox.height / 2.0f),
            m_collisionBox.width,
            m_collisionBox.height
        };

        bool collisionX = false;
        if (nextRectX.x < hubBounds.x || (nextRectX.x + nextRectX.width) > (hubBounds.x + hubBounds.width)) {
            collisionX = true;
        } else {
            for (const auto& wall : walls) {
                if (CheckCollisionRecs(nextRectX, wall.bounds)) {
                    collisionX = true;
                    break;
                }
            }
        }
        if (!collisionX) {
            m_position.x = nextX;
        }

        float nextY = m_position.y + (direction.y * m_speed);
        Rectangle nextRectY = {
            m_position.x - (m_collisionBox.width / 2.0f),
            nextY - (m_collisionBox.height / 2.0f),
            m_collisionBox.width,
            m_collisionBox.height
        };

        bool collisionY = false;
        if (nextRectY.y < hubBounds.y || (nextRectY.y + nextRectY.height) > (hubBounds.y + hubBounds.height)) {
            collisionY = true;
        } else {
            for (const auto& wall : walls) {
                if (CheckCollisionRecs(nextRectY, wall.bounds)) {
                    collisionY = true;
                    break;
                }
            }
        }
        if (!collisionY) {
            m_position.y = nextY;
        }

        m_collisionBox.x = m_position.x - (m_collisionBox.width / 2.0f);
        m_collisionBox.y = m_position.y - (m_collisionBox.height / 2.0f);
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
        : m_id(id), m_name(name), m_description(desc), m_quantity(qty), m_position(pos), m_realm(realm), m_type(type), m_attribute(attr), m_isCollected(collected) {}
    
    ItemID getID() const { return m_id; }
    std::string getName() const { return m_name; }
    std::string getDescription() const { return m_description; }
    int getQuantity() const { return m_quantity; }
    Vector2 getPosition() const { return m_position; }
    Realm getRealm() const { return m_realm; }
    ItemType getType() const { return m_type; }
    int getAttribute() const { return m_attribute; }
    bool isCollected() const { return m_isCollected; }

    void setCollected(bool collected) { m_isCollected = collected; }
};

struct Buildings {
    AreaID areaID;
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

struct Quest {
    QuestID id;
    std::string title;
    std::string description;
    bool isAccepted = false;
    bool isCompleted = false;
    int currentProgress = 0;
    int targetProgress = 1;
    int rewardGold = 50;
    int rewardXP = 100;
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

struct SubArea {
    AreaID id;
    std::string name;
    Rectangle bounds;
    std::vector<Enemy> enemies;
    std::vector<WorldItem> items;
    Color backgroundColor;
};

struct RegionHubG {
    std::string name;
    Realm realm;
    std::vector<NPCInfo> npcs;
    std::vector<SubArea> areas;
    AreaID currenSubAreaId;
};

class GameEngine {
public:
    GameEngine();
    ~GameEngine();

    void run();

    friend class RegionHubState;
    friend class WorldMapState;
    friend class InventoryState;
    friend class CombatState;
    friend class ShopState;
    friend class QuestLogState;
    friend class DialogueState;
    friend class MainMenuState;
    friend class PauseState;
    friend class OptionsState;

private:
    void update();
    void render();
    
    int getSelectedAttack();
    bool checkWallCollision(const Rectangle& rec);
    void playerMovement(float velocity);
    void updateCamera(const SubArea* currentSubArea);
    Item createItem(ItemID id, int quantity = 1);

    // FILE SAVE
    bool doesSaveFileExist(const std::string& filename);
    void saveGame(const std::string& filename);
    void loadGame(const std::string& filename);

    void acceptQuest(QuestID id);
    void completeQuest(Quest& quest);
    
    void showNotification(const std::string& text, float duration = 3.5f) {
        if (m_notificationTime > 0.0f && !m_notificationText.empty()) {
            m_notificationText += " | " + text;
        } else {
            m_notificationText = text;
        }
        m_notificationTime = duration;
    }

    void renderNotification();

    // State Pattern
    std::unique_ptr<GameState> m_currentStatePtr;
    
    void changeState(std::unique_ptr<GameState> newState) {
        if (m_currentStatePtr) {
            m_currentStatePtr->exit(*this);
        }
        m_currentStatePtr = std::move(newState);
        if (m_currentStatePtr) {
            m_currentStatePtr->enter(*this);
        }
    }


    void goToRegionHub();
    void goToWorldMap();
    void goToInventory();
    void goToCombat();
    void goToShop();
    void goToQuestLog();
    void goToDialogue();
    void goToMainMenu();
    void goToPause();
    void goToOptions();


    const float SCREENSIZEX = 1280.0f;
    const float SCREENSIZEY = 720.0f;
    Player m_player;
    DialogueManager m_dialogueManager;
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
    bool m_isShopSelling = false;
    std::vector<Quest> m_quests;
    int m_questLogIndex = 0;
    int m_lastPlayerLevel = 1;
    RenderTexture2D m_targetTexture;
    bool m_exitGame = false;
};

#endif