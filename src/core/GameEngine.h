#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <raylib.h>
#include <vector>
#include <string>
#include "../entities/Player.h"
#include "../dialogue/DialogueManager.h"

enum class GameState {
    WorldMap,
    Dialogue,
    Inventory
};


struct Region {
    std::string name;
    Realm realm;
    Rectangle bounds;
    Color color;
};

class GameEngine {
public:
    GameEngine();
    ~GameEngine();

    void run();

private:
    void update();
    void render();
    void renderUI();

    Player m_player;
    DialogueManager m_dialogueManager;
    GameState m_currentState;
    std::vector<Region> m_regions;
    Realm m_selectRealm;
    Color renderColor;
    Music m_ignisSound;
};

#endif