#ifndef PLAYER_H
#define PLAYER_H

#include <string>

enum class Realm { Aer, Aqua, Terra, Ignis };

struct FactionReputation {
    int aer = 0;   // Da -100 (Ostile) a +100 (Alleato)
    int aqua = 0;
    int terra = 0;
    int ignis = 0;
};

class Player {
public:
    Player(const std::string& name);

    // Getters
    std::string getName() const { return m_name; }
    int getGold() const { return m_gold; }
    
    // Gestione Risorse
    void addGold(int amount) { m_gold += amount; }
    bool payGold(int amount);

    // Gestione Reputazione & Magia
    void modifyReputation(Realm realm, int amount);
    int getReputation(Realm realm) const;
    
    int aerMastery = 10;
    int aquaMastery = 10;
    int terraMastery = 10;
    int ignisMastery = 10;

private:
    std::string m_name;
    int m_gold = 50;
    FactionReputation m_reputation;
};

#endif