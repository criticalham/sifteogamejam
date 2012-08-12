/*
 * Explorathon main game class
 */
#include "game.h"
#include "gamecube.h"
#include "mapgen.h"
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
    for (int i = 0; i < MAPSIZE; i++) {
        for (int j = 0; j < MAPSIZE; j++) {
            m_visited[i][j] = false;
        }
    }
}

void Game::generateItems()
{
    int i, x, y;
    for (i=0; i < MAPSIZE*MAPSIZE; i++)
    {
        worldObjects[i%MAPSIZE][i/MAPSIZE] = 0;
    }

    do
    {
        keyX = Random().randrange(MAPSIZE/2) * 2;
        keyY = Random().randrange(MAPSIZE/2) * 2;
    } while (positionVisible(keyX, keyY));
    do
    {
        chestX = Random().randrange(MAPSIZE/2) * 2;
        chestY = Random().randrange(MAPSIZE/2) * 2;
    } while (positionVisible(chestX, chestY));

    for (i=0; i < NUM_BOULDERS; i++)
    {
        x = Random().randrange(MAPSIZE);
        y = Random().randrange(MAPSIZE);
        if (! worldObjects[x][y] && x != keyX && y != keyY && x != chestX && y != chestY)
        {
            worldObjects[x][y] = BOULDER_ID;
            LOG("Boulder at %d, %d\n", x, y);
        }
        else
        {
            i--;
        }
    }
    for (i=0; i < NUM_RED_FLOWERS; i++)
    {
        x = Random().randrange(MAPSIZE);
        y = Random().randrange(MAPSIZE);
        if (! worldObjects[x][y] && x != keyX && y != keyY && x != chestX && y != chestY)
        {
            worldObjects[x][y] = RED_FLOWER_ID;
            LOG("RedFlower at %d, %d\n", x, y);
        }
        else
        {
            i--;
        }
    }
    for (i=0; i < NUM_BLUE_FLOWERS; i++)
    {
        x = Random().randrange(MAPSIZE);
        y = Random().randrange(MAPSIZE);
        if (! worldObjects[x][y] && x != keyX && y != keyY && x != chestX && y != chestY)
        {
            worldObjects[x][y] = BLUE_FLOWER_ID;
            LOG("BlueFlower at %d, %d\n", x, y);
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
    for (i=0; i < MAPSIZE*MAPSIZE; i++)
    {
        if (i%MAPSIZE == 0) LOG("\n");
        if (positionVisible(i%MAPSIZE, i/MAPSIZE)) LOG("*");
        else LOG("%d", worldObjects[i%MAPSIZE][i/MAPSIZE]);
    }
}

void Game::reset()
{
    foundKey = false;
    foundChest = false;
    gotKey = false;
    gotChest = false;
    generateItems();
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

        MapGen::randomize();
    }
}

void Game::run()
{
    System::paint();
}

void Game::drawWorldObjects(GameCube *gameCube, int x, int y)
{
    gameCube->m_vid.sprites.erase();

    int currentX, currentY;

    // TOP LEFT
    currentX = ((x-1) + MAPSIZE) % MAPSIZE;
    currentY = ((y-1) + MAPSIZE) % MAPSIZE;
    if (worldObjects[currentX][currentY])
    {
        gameCube->m_vid.sprites[0].setImage(MapGen::intToAsset(worldObjects[currentX][currentY]));
        gameCube->m_vid.sprites[0].move(-32, 32);
    }

    // TOP CENTER
    currentX = ((x+0) + MAPSIZE) % MAPSIZE;
    currentY = ((y-1) + MAPSIZE) % MAPSIZE;
    if (worldObjects[currentX][currentY])
    {
        gameCube->m_vid.sprites[1].setImage(MapGen::intToAsset(worldObjects[currentX][currentY]));
        gameCube->m_vid.sprites[1].move(32, 32);
    }

    // TOP RIGHT
    currentX = ((x+1) + MAPSIZE) % MAPSIZE;
    currentY = ((y-1) + MAPSIZE) % MAPSIZE;
    if (worldObjects[currentX][currentY])
    {
        gameCube->m_vid.sprites[2].setImage(MapGen::intToAsset(worldObjects[currentX][currentY]));
        gameCube->m_vid.sprites[2].move(96, 32);
    }

    // CENTER LEFT
    currentX = ((x-1) + MAPSIZE) % MAPSIZE;
    currentY = ((y+0) + MAPSIZE) % MAPSIZE;
    if (worldObjects[currentX][currentY])
    {
        gameCube->m_vid.sprites[0].setImage(MapGen::intToAsset(worldObjects[currentX][currentY]));
        gameCube->m_vid.sprites[0].move(-32, 32);
    }

    // CENTER
    currentX = ((x+0) + MAPSIZE) % MAPSIZE;
    currentY = ((y+0) + MAPSIZE) % MAPSIZE;
    if (worldObjects[currentX][currentY])
    {
        gameCube->m_vid.sprites[1].setImage(MapGen::intToAsset(worldObjects[currentX][currentY]));
        gameCube->m_vid.sprites[1].move(32, 32);
    }

    // CENTER RIGHT
    currentX = ((x+1) + MAPSIZE) % MAPSIZE;
    currentY = ((y+0) + MAPSIZE) % MAPSIZE;
    if (worldObjects[currentX][currentY])
    {
        gameCube->m_vid.sprites[2].setImage(MapGen::intToAsset(worldObjects[currentX][currentY]));
        gameCube->m_vid.sprites[2].move(96, 32);
    }

    // BOTTOM LEFT
    currentX = ((x-1) + MAPSIZE) % MAPSIZE;
    currentY = ((y+1) + MAPSIZE) % MAPSIZE;
    if (worldObjects[currentX][currentY])
    {
        gameCube->m_vid.sprites[0].setImage(MapGen::intToAsset(worldObjects[currentX][currentY]));
        gameCube->m_vid.sprites[0].move(-32, 32);
    }

    // BOTTOM CENTER
    currentX = ((x+0) + MAPSIZE) % MAPSIZE;
    currentY = ((y+1) + MAPSIZE) % MAPSIZE;
    if (worldObjects[currentX][currentY])
    {
        gameCube->m_vid.sprites[1].setImage(MapGen::intToAsset(worldObjects[currentX][currentY]));
        gameCube->m_vid.sprites[1].move(32, 32);
    }

    // BOTTOM RIGHT
    currentX = ((x+1) + MAPSIZE) % MAPSIZE;
    currentY = ((y+1) + MAPSIZE) % MAPSIZE;
    if (worldObjects[currentX][currentY])
    {
        gameCube->m_vid.sprites[2].setImage(MapGen::intToAsset(worldObjects[currentX][currentY]));
        gameCube->m_vid.sprites[2].move(96, 32);
    }
}

/**
  * Marks that a given location has been visited
  */
void Game::visitAndDrawItemsAt(GameCube* gameCube)
{

    int x = gameCube->m_x;
    int y = gameCube->m_y;
    BG1Drawable& draw = gameCube->m_vid.bg1;

    drawWorldObjects(gameCube, x, y);

    if(!m_visited[x][y])
    {
        // Play a sound effect for finding a new tile?
        LOG("Found unvisited tile.\n");
        m_visited[x][y] = true;
    }

    if(keyX == x && keyY == y)
    {
        foundKey = true;

        if(!gotKey)
        {
            draw.maskedImage(Key, Emptiness);
        }

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

        //draw.setPanning(vec(-32, -32));
    }
}

void Game::drawMiniMap(GameCube* gc)
{
    LOG("Drawing mini map");
    BG0Drawable& draw = gc->m_vid.bg0;

//    gc->m_vid.colormap[0].set(1.f, 0.f, 0.f);
//    gc->m_vid.colormap[1].set(0.f, 1.f, 0.f);

    // TODO: draw 128x128 background grid

    for (int i = 0; i < MAPSIZE; i+=2) {
        for (int j = 0; j < MAPSIZE; j+=2) {
            if(m_visited[i][j] == true)
                draw.fill(vec(i, j), vec(2,2), Visited);
            else
                draw.fill(vec(i, j), vec(2,2), Unvisited);
        }
    }

    for (int i = 0; i < CUBE_ALLOCATION; i++) {
        if(!gameCubes[i].m_isOn)
        {
//            draw.fill(vec(int(gameCubes[i].m_x), int(gameCubes[i].m_y)), vec(2,2), Unvisited);
        }
        else
        {
            LOG("Drawing point at %d, %d", gameCubes[i].m_x, gameCubes[i].m_y);
            draw.fill(vec(gameCubes[i].m_x, gameCubes[i].m_y), vec(2,2), Highlight);
        }
    }

    System::paint();
}

bool Game::positionVisible(int x, int y)
{
    for (int cubeIndex = 0; cubeIndex < CUBE_ALLOCATION; cubeIndex++)
    {
        if (m_gameCubes[cubeIndex].m_x == x && m_gameCubes[cubeIndex].m_y == y) return true;
    }
    return false;
}
