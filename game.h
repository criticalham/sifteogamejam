/*
 * Sifteo SDK Example.
 */

#ifndef _GAME_H
#define _GAME_H

#include <sifteo.h>
#include "assets.gen.h"

using namespace Sifteo;

class Game {
public:
    void init();
    void cleanup();
    void reset();
    void draw();
    void run();
    void visited(int x, int y);

private:
    // Meta game state
    bool running;

    // Game state
    bool foundKey;
    bool foundChest;

    int keyX;
    int keyY;
    int chestX;
    int chestY;
};

extern Game g_game;

#endif
