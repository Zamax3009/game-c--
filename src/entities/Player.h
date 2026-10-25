#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"
#include <string>
#include <vector>

enum class Realm { Aer, Aqua, Terra, Ignis }; // The four realms

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
};

struct ElementalMastery {
    int aer   = 0;   // From 0 (none) to 100 (Master)
    int aqua  = 0;
    int terra = 0;
    int ignis = 0;
};

class Player {
public:

    Player() {
        m_name = "Hero";
        m_hp = 100;
        m_maxHP = 100;
        m_gold = 50;

        m_attacks = {
            {"Quick Strike", 12,  0, "A fast light attack with low damage."},
            {"Heavy Slash",  28,  0, "A powerful swing dealing high damage."},
            {"Inner Calm",    0, 20, "Restores health by focusing energy."}
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

    // SETTERS
    // Resource management
    void addGold(int amount) { m_gold += amount; }
    bool payGold(int amount);
    void setPosition(Vector2 pos) { m_position = pos; }
    void move(Vector2 offset) { m_position.x += offset.x, m_position.y += offset.y; }
    void takeDamage(int amount) {
        m_hp -= amount;
        if (m_hp < 0) m_hp = 0;
    }
    void resetHP() { m_hp = m_maxHP; }
    void heal(int amount) { 
        m_hp += amount;
        if (m_hp > m_maxHP) m_hp = m_maxHP;
    }
    
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
    FactionReputation m_reputation;
    ElementalMastery m_mastery;
    std::vector<Attack> m_attacks;
};

#endif