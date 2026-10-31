#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <string>
#include <vector>

enum class Realm { Aer, Aqua, Terra, Ignis }; // The four realms

enum class ItemType {
    None,
    Heal,
    RestoreMana
};

enum class ItemID {
    None = 0,
    HealthElixir,
    ManaPotion,
    AncientScroll,
    ElementalCrystal
};

struct Item {
    ItemID id;
    std::string name;
    std::string description;
    int quantity;
    ItemType type;
    int attribute;
};

struct FactionReputation {
    int aer   = 0;   // From -100 (Ostile) to +100 (Allay)
    int aqua  = 0;
    int terra = 0;
    int ignis = 0;
};

struct Attack {
    std::string name;
    int damage;
    int healing;
    std::string description;
    int manaUse;
};

struct ElementalMastery {
    int aer   = 0;   // From 0 (none) to 100 (Master)
    int aqua  = 0;
    int terra = 0;
    int ignis = 0;
};

class Inventory {
private:
    std::vector<Item> m_items;
    int m_selectedIndex = 0;

public:
    Inventory() = default;
    Inventory(std::initializer_list<Item> items) : m_items(items) {}

    void addItem(const Item& item) {
        bool found = false;
        for (auto& invItem : m_items) {
            if (invItem.id == item.id) {
                invItem.quantity += item.quantity;
                found = true;
                break;
            }
        }
        if (!found) {
            m_items.push_back(item);
        }
    }

    bool removeItem(size_t index, int amount = 1) {
        if (index >= m_items.size()) return false;
        m_items[index].quantity -= amount;
        if (m_items[index].quantity <= 0) {
            m_items.erase(m_items.begin() + index);
        }
        return true;
    }

    const std::vector<Item>& getItems() const { return m_items; }
    std::vector<Item> getItemsNonConst() { return m_items; }
    bool isEmpty() const { return m_items.empty(); }
    int getIndex() const { return m_selectedIndex; }
    void setIndex(int idx) { m_selectedIndex = idx; }
    size_t size() { return m_items.size(); }
    void clear() { m_items.clear(); }

    Item& operator[](size_t index) { return m_items[index]; }
    const Item& operator[](size_t index) const { return m_items[index]; }
    
};

class Player {
public:

    Player() {
        m_name = "Hero";
        m_hp = 100;
        m_maxHP = 100;
        m_gold = 50;
        m_mana = 100;

        m_attacks = {
            {"Quick Strike", 12,  0, "A fast light attack with low damage.", 10},
            {"Heavy Slash",  28,  0, "A powerful swing dealing high damage.", 20},
            {"Inner Calm",    0, 20, "Restores health by focusing energy.", 10}
        };
    }
    // GETTERS
    Vector2 getPosition() const { return m_position; }
    std::string getName() const { return m_name; }
    int getGold() const { return m_gold; }
    int getReputation(Realm realm) const;
    int getMastery(Realm realm) const;
    int getHP() const { return m_hp; }
    int getMaxHP() const { return m_maxHP; }
    int getMana() const { return m_mana; }
    int getMaxMana() const { return m_maxMana; }

    // SETTERS
    // Resource management
    void addGold(int amount) { m_gold += amount; }
    bool payGold(int amount);
    void setGold(int amount) { m_gold = amount; }
    void setPosition(Vector2 pos) { m_position = pos; }
    void move(Vector2 offset) { m_position.x += offset.x, m_position.y += offset.y; }
    void takeDamage(int amount) {
        m_hp -= amount;
        if (m_hp < 0) m_hp = 0;
    }
    void setMaxHp(int maxHP) { m_maxHP = maxHP; }
    void resetHP() { m_hp = m_maxHP; }
    void heal(int amount) { 
        m_hp += amount;
        if (m_hp > m_maxHP) m_hp = m_maxHP;
    }
    void setHP(int amount) { m_hp = amount; }
    void addMana(int amount) { 
        m_mana += amount;
        if (m_mana > m_maxMana) m_mana = m_maxMana;
    }
    void useMana(int amount) {
        m_mana -= amount;
        if (m_mana < 0 ) m_mana = 0;
    }
    void setMana(int amount) {
        m_mana = amount;
        if (m_mana > m_maxMana) m_mana = m_maxMana;
        if (m_mana < 0) m_mana = 0;
    }
    void resetMana(int amount) { m_mana = m_maxMana; }
    void resetToDefault();
    // Reputation and Elemental Mastery management
    void modifyReputation(Realm realm, int amount);
    void modifyMastery(Realm realm, int amount);

    const std::vector<Attack>& getAttacks() const { return m_attacks; }

private:
    int m_hp = 100;
    int m_maxHP = 100;
    Vector2 m_position;
    std::string m_name;
    int m_gold = 50;
    int m_mana = 100;
    int m_maxMana = 100;
    FactionReputation m_reputation;
    ElementalMastery m_mastery;
    std::vector<Attack> m_attacks;
};

#endif