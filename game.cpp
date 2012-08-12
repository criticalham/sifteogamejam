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
#define NUM_BOULDERS 30
#define NUM_RED_FLOWERS 8
#define NUM_BLUE_FLOWERS 8
#define BOULDER_SPAWN_MIN_RADIUS 6
#define BOULDER_SPAWN_MAX_RADIUS 8
#define BLUE_FLOWER_SPAWN_MIN_RADIUS 2
#define BLUE_FLOWER_SPAWN_MAX_RADIUS 3
#define RED_FLOWER_SPAWN_MIN_RADIUS 2
#define RED_FLOWER_SPAWN_MAX_RADIUS 3

void Game::initWithCubes(GameCube gameCubes[CUBE_ALLOCATION])
{
    m_gameCubes[CUBE_ALLOCATION] = *gameCubes;
    reset();

    LOG("init() completed\n");
}

bool Game::itemInRange(UInt2 objectPos, UInt2 targetPos, int radius)
{
    int dx = objectPos.x - targetPos.x;
    int dy = objectPos.y - targetPos.y;
    if (dx*dx + dy*dy <= radius*radius) return true;

    dx = (16-dx)%16;
    dy = (16-dy)%16;
    return dx*dx + dy*dy <= radius*radius;
}

UInt2 Game::coordOnDonut(UInt2 pos, int minRadius, int maxRadius)
{
    //LOG("----------ENTER----------\n");
    int dx, dy;
    do
    {
        dy = Random().randrange(-maxRadius, maxRadius);
        dx = Random().randrange(-maxRadius, maxRadius);
        //LOG("x: %d\ty: %d\tmin_radius: %d\tmax_radius: %d\t--\t%d, %d, %d\n", dx, dy, minRadius, maxRadius, dx*dx + dy*dy, maxRadius*maxRadius, minRadius*minRadius);
    } while (dx*dx + dy*dy > maxRadius*maxRadius || dx*dx + dy*dy < minRadius*minRadius);
    return vec((pos.x + dx)%16, (pos.y + dy)%16);
}

void Game::generateItems()
{
    int i, x, y;
    UInt2 randomPos;
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
        randomPos = coordOnDonut(vec(keyX/2, keyY/2), BOULDER_SPAWN_MIN_RADIUS, BOULDER_SPAWN_MAX_RADIUS);
        x = randomPos.x;
        if (x < 0) x += 16;
        y = randomPos.y;
        if (y < 0) y += 16;
        if (! worldObjects[x][y] && !itemInRange(vec(chestX/2, chestY/2), randomPos, BOULDER_SPAWN_MIN_RADIUS))
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
        randomPos = coordOnDonut(vec(keyX, keyY), RED_FLOWER_SPAWN_MIN_RADIUS, RED_FLOWER_SPAWN_MAX_RADIUS);
        x = randomPos.x;
        if (x < 0) x += 16;
        y = randomPos.y;
        if (y < 0) y += 16;
        if (! worldObjects[x][y] && x != chestX/2 && y != chestY/2)
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
        randomPos = coordOnDonut(vec(chestX, chestY), BLUE_FLOWER_SPAWN_MIN_RADIUS, BLUE_FLOWER_SPAWN_MAX_RADIUS);
        x = randomPos.x;
        if (x < 0) x += 16;
        y = randomPos.y;
        if (y < 0) y += 16;
        if (! worldObjects[x][y] && x != keyX/2 && y != keyY/2)
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

        MapGen::randomize();
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

    LOG("World object: %d\n", worldObjects[x/2][y/2]);
    if (worldObjects[x/2][y/2])
    {
        draw.maskedImage(MapGen::intToAsset(worldObjects[x/2][y/2]), Transparent);
        LOG("DRAWING IMAGE %d\n", worldObjects[x/2][y/2]);
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

        draw.setPanning(vec(-32, -32));
    }
}

void Game::drawMiniMap(GameCube* gc)
{
    LOG("Drawing mini map");
    BG0Drawable& draw = gc->m_vid.bg0;

//    gc->m_vid.colormap[0].set(1.f, 0.f, 0.f);
//    gc->m_vid.colormap[1].set(0.f, 1.f, 0.f);

    // TODO: draw 128x128 background grid

    for (int i = 0; i < CUBE_ALLOCATION; i++) {
        if(!gameCubes[i].m_isOn)
        {
            draw.fill(vec(int(gameCubes[i].m_x), int(gameCubes[i].m_y)), vec(2,2), Emptiness);
        }
        else
        {
            LOG("Drawing point at %d, %d", gameCubes[i].m_x, gameCubes[i].m_y);
            draw.fill(vec(int(gameCubes[i].m_x), int(gameCubes[i].m_y)), vec(2,2), Highlight);
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
