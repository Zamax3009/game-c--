#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "raylib.h"

class GameEngine;

class GameState {
public:
    virtual ~GameState() = default;


    virtual void enter(GameEngine& engine) {}
    virtual void update(GameEngine& GameEngine) = 0;
    virtual void render(GameEngine& engine) = 0;
    virtual void exit(GameEngine& engine) {}
};

#endif