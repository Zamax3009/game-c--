#include "Player.h"

Player::Player(const std::string& name) : m_name(name) {}

bool Player::payGold(int amount) {
    if (m_gold >= amount) {
        m_gold -= amount;
        return true;
    }
    return false;
}

void Player::modifyReputation(Realm realm, int amount) {
    switch (realm) {
        case Realm::Aer:   m_reputation.aer += amount; break;
        case Realm::Aqua:  m_reputation.aqua += amount; break;
        case Realm::Terra: m_reputation.terra += amount; break;
        case Realm::Ignis: m_reputation.ignis += amount; break;
    }
}

int Player::getReputation(Realm realm) const {
    switch (realm) {
        case Realm::Aer:   return m_reputation.aer;
        case Realm::Aqua:  return m_reputation.aqua;
        case Realm::Terra: return m_reputation.terra;
        case Realm::Ignis: return m_reputation.ignis;
    }
    return 0;
}