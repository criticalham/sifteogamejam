/*
 * Explorathon main game class
 */
#include "game.h"
#include "gamecube.h"
#include <sifteo.h>

#define DEBUG

void Game::initWithCubes(GameCube gameCubes[CUBE_ALLOCATION])
{
    m_gameCubes[CUBE_ALLOCATION] = *gameCubes;

    reset();

    LOG("init() completed\n");
}

void Game::reset()
{
    foundKey = false;
    foundChest = false;

    keyX = 0;
    keyY = 0;
    chestX = 1;
    chestY = 0;
}

void Game::restartGame()
{
    for(int cubeIndex=0; cubeIndex < CUBE_ALLOCATION; cubeIndex++)
    {
        m_gameCubes[cubeIndex].reset();
    }
}

void Game::run()
{
    System::paint();
}

/**
  * Marks that a given location has been visited
  */
void Game::visited(int x, int y)
{
    if(!foundKey && keyX == x && keyY == y)
    {
        foundKey = true;
        LOG("Game key found!\n");
    }

    if(!foundChest && chestX == x && chestY == y)
    {
        foundChest = true;
        LOG("Game chest found!\n");
    }

    if(foundKey && foundChest)
    {
        restartGame();
        LOG("Game over! Restarting game.");
    }

    #ifdef DEBUG
        LOG("Checking for key %d, %d, at %d, %d\n", keyX, keyY, x, y);
        LOG("Checking for chest %d, %d, at %d, %d\n", chestX, chestY, x, y);
    #endif
}