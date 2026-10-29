#include "Player.h"

void Player::resetToDefault() {
    m_hp = 100.0f;
    m_gold = 50;
    m_position = { 200.0f, 200.0f };
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

int Player::getMastery(Realm realm) const {
    switch (realm) {
        case Realm::Aer:   return m_mastery.aer;
        case Realm::Aqua:  return m_mastery.aqua;
        case Realm::Terra: return m_mastery.terra;
        case Realm::Ignis: return m_mastery.ignis;
    }
    return 0;
}

bool Player::payGold(int amount) {
    if (m_gold >= amount) {
        m_gold -= amount;
        return true;
    }
    return false;
}

void Player::modifyReputation(Realm realm, int amount) {
    switch (realm) {
        case Realm::Aer:   m_reputation.aer   += amount; break;
        case Realm::Aqua:  m_reputation.aqua  += amount; break;
        case Realm::Terra: m_reputation.terra += amount; break;
        case Realm::Ignis: m_reputation.ignis += amount; break;
    }
}

void Player::modifyMastery(Realm realm, int amount) {
    switch (realm)
    {
    case Realm::Aer:   m_mastery.aer   += amount; break;
    case Realm::Aqua:  m_mastery.aqua  += amount; break;
    case Realm::Terra: m_mastery.terra += amount; break;
    case Realm::Ignis: m_mastery.ignis += amount; break;
    }
}
