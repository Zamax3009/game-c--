#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "../entities/Player.h"
#include "../dialogue/DialogueManager.h"

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
};

#endif