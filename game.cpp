/*
 * Explorathon main game class
 */
#include "game.h"
#include "gamecube.h"
#include <sifteo.h>
#include <sifteo/audio.h>
#include "mapgen.h"

#define DEBUG

#define BOULDER_ID 1
#define RED_FLOWER_ID 2
#define BLUE_FLOWER_ID 3
#define NUM_BOULDERS 3
#define NUM_RED_FLOWERS 5
#define NUM_BLUE_FLOWERS 2

void Game::initWithCubes(GameCube gameCubes[CUBE_ALLOCATION])
{
    m_gameCubes[CUBE_ALLOCATION] = *gameCubes;
    reset();

    LOG("init() completed\n");
}

void Game::generateItems()
{
    int i, x, y;
    for (i=0; i < 16*16; i++)
    {
        worldObjects[i%16][i/16] = 0;
    }

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

    for (i=0; i < NUM_BOULDERS; i++)
    {
        x = Random().randrange(16);
        y = Random().randrange(16);
        if (! worldObjects[x][y] && x != keyX && y != keyY && x != chestX && y != chestY)
        {
            worldObjects[x][y] = BOULDER_ID;
            LOG("Boulder at %d, %d\n", x*2, y*2);
        }
        else
        {
            i--;
        }
    }
    for (i=0; i < NUM_RED_FLOWERS; i++)
    {
        x = Random().randrange(16);
        y = Random().randrange(16);
        if (! worldObjects[x][y] && x != keyX && y != keyY && x != chestX && y != chestY)
        {
            worldObjects[x][y] = RED_FLOWER_ID;
            LOG("RedFlower at %d, %d\n", x*2, y*2);
        }
        else
        {
            i--;
        }
    }
    for (i=0; i < NUM_BLUE_FLOWERS; i++)
    {
        x = Random().randrange(16);
        y = Random().randrange(16);
        if (! worldObjects[x][y] && x != keyX && y != keyY && x != chestX && y != chestY)
        {
            worldObjects[x][y] = BLUE_FLOWER_ID;
            LOG("BlueFlower at %d, %d\n", x*2, y*2);
        }
        else
        {
            i--;
        }
    }
    debugWorld();
}

void Game::debugWorld()
{
    int i;
    LOG("World objects: ");
    for (i=0; i < 16*16; i++)
    {
        if (i%16 == 0) LOG("\n");
        if (positionVisible(i%16, i/16)) LOG("*");
        else LOG("%d", worldObjects[i%16][i/16]);
    }
}

void Game::reset()
{
    foundKey = false;
    foundChest = false;
    gotKey = false;
    gotChest = false;
    generateItems();

    /*do
    {
        keyX = Random().randrange(16) * 2;
        keyY = Random().randrange(16) * 2;
    } while (positionVisible(keyX, keyY));
    do
    {
        chestX = Random().randrange(16) * 2;
        chestY = Random().randrange(16) * 2;
    } while (positionVisible(chestX, chestY));
    */
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

    LOG("World object: %d", worldObjects[x/2][y/2]);
    if (worldObjects[x/2][y/2])
    {
        draw.maskedImage(MapGen::intToAsset(worldObjects[x/2][y/2]), Transparent);
        LOG("DRAWING IMAGE %d", worldObjects[x/2][y/2]);
    }

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

    //debugWorld();
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
                gameCube->highlight();
            }

            if(gotKey)
            {
                gotChest = true;
                AudioChannel(0).play(CoinSound);
                LOG("Chest got!\n");
                draw.maskedImage(ChestOpen, Transparent);
                gameCube->highlight();
            }
        }

        if(!gotKey && keyX == x && keyY == y)
        {
            AudioChannel(0).play(KeySound);
            LOG("Key got!\n");
            gotKey = true;
            gameCube->render();
            gameCube->highlight();
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