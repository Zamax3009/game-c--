#ifndef PLAYER_H
#define PLAYER_H

#include <string>

enum class Realm { Aer, Aqua, Terra, Ignis }; // The four realms

struct FactionReputation {
    int aer   = 0;   // From -100 (Ostile) to +100 (Allay)
    int aqua  = 0;
    int terra = 0;
    int ignis = 0;
};

struct ElementalMastery {
    int aer   = 0;   // From 0 (none) to 100 (Master)
    int aqua  = 0;
    int terra = 0;
    int ignis = 0;
};

class Player {
public:
    Player(const std::string& name);

    // GETTERS
    std::string getName() const { return m_name; }
    int getGold() const { return m_gold; }
    int getReputation(Realm realm) const;
    int getMastery(Realm realm) const;

    // SETTERS
    // Resource management
    void addGold(int amount) { m_gold += amount; }
    bool payGold(int amount);

    // Reputation and Elemental Mastery management
    void modifyReputation(Realm realm, int amount);
    void modifyMastery(Realm realm, int amount);

private:
    std::string m_name;
    int m_gold = 50;
    FactionReputation m_reputation;
    ElementalMastery m_mastery;
};

#endif