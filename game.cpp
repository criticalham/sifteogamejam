/*
 * Explorathon main game class
 */
#include "game.h"
#include "gamecube.h"
#include <sifteo.h>
#include <sifteo/audio.h>

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

    do
    {
        keyX = Random().randrange(16) * 2;
        keyY = Random().randrange(16) * 2;
    } while (positionVisible(keyX, keyY));
    do
    {
        chestX = Random().randrange(16) * 2;
        chestY = Random().randrange(16) * 2;
    } while (positionVisible(chestX, chestY));
}

void Game::restartGame()
{
    LOG("Game reset!\n");

    // Reset game data
    reset();

    // Reset all cubes
    for (int cubeIndex = 0; cubeIndex < CUBE_ALLOCATION; cubeIndex++)
    {
        LOG("Resetting m_gameCube[%d]\n", cubeIndex);
        m_gameCubes[cubeIndex].reset();
        //visitAndDrawItemsAt(&m_gameCubes[cubeIndex]);
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
            draw.maskedImage(Key, Emptiness);

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
    LOG("isDown is %d\n", isDown);
    if(isDown)
    {
        int x = gameCube->m_x;
        int y = gameCube->m_y;
        BG1Drawable& draw = gameCube->m_vid.bg1;

        if(chestX == x && chestY == y)
        {
            if(gotChest)
            {
                LOG("Restarting game.\n");
                restartGame();
                gameCube->render();
            }

            if(gotKey)
            {
                gotChest = true;
                AudioChannel(0).play(CoinSound);
                LOG("Chest got!\n");
                draw.maskedImage(ChestOpen, Transparent);
            }
        }

        if(!gotKey && keyX == x && keyY == y)
        {
            AudioChannel(0).play(KeySound);
            LOG("Key got!\n");
            gotKey = true;
            gameCube->render();
        }

        draw.setPanning(vec(-32, -32));
    }
}

bool Game::positionVisible(int x, int y)
{
    for (int cubeIndex = 0; cubeIndex < CUBE_ALLOCATION; cubeIndex++)
    {
        if (m_gameCubes[cubeIndex].m_x == x && m_gameCubes[cubeIndex].m_y == y) return true;
    }
    return false;
}