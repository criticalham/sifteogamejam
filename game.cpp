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
    gotKey = false;
    gotChest = false;

    keyX = 0;
    keyY = 0;
    chestX = 2;
    chestY = 0;
}

void Game::restartGame()
{
    reset();

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
void Game::visitAndDrawItemsAt(GameCube* gameCube)
{
    int x = gameCube->m_x;
    int y = gameCube->m_y;
    BG1Drawable& draw = gameCube->m_vid.bg1;

    if(keyX == x && keyY == y)
    {
        foundKey = true;

        if(!gotKey)
            draw.maskedImage(Key, Transparent);


        LOG("Game key found!\n");
    }

    if(chestX == x && chestY == y)
    {
        foundChest = true;

        if(!gotChest)
            draw.maskedImage(ChestClosed, Emptiness);
        else
            draw.maskedImage(ChestOpen, Emptiness);

        LOG("Game chest found!\n");
    }

    #ifdef DEBUG
        LOG("Checking for key %d, %d, at %d, %d\n", keyX, keyY, x, y);
        LOG("Checking for chest %d, %d, at %d, %d\n", chestX, chestY, x, y);
    #endif

    draw.setPanning(vec(-32, -32));
}

/**
  * Handles cube touches
  */
void Game::handleCubeTouch(GameCube* gameCube, bool isDown)
{
    if(isDown)
    {
        int x = gameCube->m_x;
        int y = gameCube->m_y;
        BG1Drawable& draw = gameCube->m_vid.bg1;

        if(chestX == x && chestY == y)
        {
            if(gotChest)
            {
                LOG("Winner! Restarting game.\n");
                restartGame();
            }

            if(gotKey)
            {
                gotChest = true;
                draw.maskedImage(ChestOpen, Transparent);
            }
        }

        if(!gotKey && keyX == x && keyY == y)
        {
            gotKey = true;
            gameCube->render();
        }

        draw.setPanning(vec(-32, -32));
    }
}