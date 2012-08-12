/*
 * Sifteo SDK Example.
 */

#ifndef _GAME_H
#define _GAME_H

#include <sifteo.h>
#include "assets.gen.h"
#include "gamecube.h"

using namespace Sifteo;

class Game {
public:
    void initWithCubes(GameCube gameCubes[CUBE_ALLOCATION]);
    void cleanup();
    void reset();
    void draw();
    void run();
    void visitAndDrawItemsAt(GameCube* gameCube);
    void handleCubeTouch(GameCube* gameCube, bool isDown);
    bool positionVisible(int, int);

    
    void drawMiniMap(GameCube* gc);
    int m_referenceCubeID;
private:
    void restartGame();
    void generateItems();
    void debugWorld();
    GameCube m_gameCubes[CUBE_ALLOCATION];

    // Meta game state
    bool running;

    // Game state
    bool foundKey;
    bool foundChest;
    bool gotKey;
    bool gotChest;

    int keyX;
    int keyY;
    int chestX;
    int chestY;
    int worldObjects[16][16];
};

extern Game g_game;

#endif
