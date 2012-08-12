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
#define TURTLE_ID  4
#define SNAKE_ID   5
#define FROG_ID    6
#define LADYBUG_ID 7
#define NUM_BOULDERS 30
#define NUM_RED_FLOWERS 10
#define NUM_BLUE_FLOWERS 10
#define NUM_ANIMALS_PER_TYPE 3
#define BOULDER_SPAWN_MIN_RADIUS 5
#define BOULDER_SPAWN_MAX_RADIUS 8
#define BLUE_FLOWER_SPAWN_MIN_RADIUS 1
#define BLUE_FLOWER_SPAWN_MAX_RADIUS 3
#define RED_FLOWER_SPAWN_MIN_RADIUS 1
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
    //LOG("itemInRange -- INFO: x %d, y %d, x %d, y %d\n", objectPos.x, objectPos.y, targetPos.x, targetPos.y);
    //LOG("itemInRange? dx %d, dy %d, radius%d\n", dx, dy, radius);
    if (dx*dx + dy*dy <= radius*radius) return true;

    dx = (MAPSIZE-dx)%MAPSIZE;
    dy = (MAPSIZE-dy)%MAPSIZE;
    return dx*dx + dy*dy <= radius*radius;
}

UInt2 Game::coordOnDonut(UInt2 pos, int minRadius, int maxRadius)
{
    //LOG("----------ENTER----------\n");
    int dx, dy;
    do
    {
        dy = Random().randrange(maxRadius*2+1) - maxRadius;
        dx = Random().randrange(maxRadius*2+1) - maxRadius;
        //LOG("x: %d\ty: %d\tmin_radius: %d\tmax_radius: %d\t--\t%d, %d, %d\n", dx, dy, minRadius, maxRadius, dx*dx + dy*dy, maxRadius*maxRadius, minRadius*minRadius);
    } while (dx*dx + dy*dy > maxRadius*maxRadius || dx*dx + dy*dy < minRadius*minRadius);
    return vec((pos.x + dx)%MAPSIZE, (pos.y + dy)%MAPSIZE);
}

void Game::generateItems()
{
    int i, x, y, tries;
    UInt2 randomPos;
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
    tries = 0;
    for (i=0; i < NUM_BOULDERS; i++)
    {
        randomPos = coordOnDonut(vec(keyX/2, keyY/2), BOULDER_SPAWN_MIN_RADIUS, BOULDER_SPAWN_MAX_RADIUS);
        x = randomPos.x;
        if (x < 0) x += MAPSIZE;
        y = randomPos.y;
        if (y < 0) y += MAPSIZE;
        if (worldObjects[x][y] == 0 && !itemInRange(vec(chestX/2, chestY/2), vec(x,y), BOULDER_SPAWN_MIN_RADIUS))
        {
            worldObjects[x][y] = BOULDER_ID;
            //LOG("Boulder @ %d, %d\n", x, y);
        }
        else
        {
            i--;
            tries++;
            if (tries > 30000)
            {
                LOG("COULDN'T SPAWN ALL BOULDERS\n");
                break;
            }
        }
    }
    tries = 0;
    for (i=0; i < NUM_RED_FLOWERS; i++)
    {
        randomPos = coordOnDonut(vec(keyX/2, keyY/2), RED_FLOWER_SPAWN_MIN_RADIUS, RED_FLOWER_SPAWN_MAX_RADIUS);
        x = randomPos.x;
        if (x < 0) x += MAPSIZE;
        y = randomPos.y;
        if (y < 0) y += MAPSIZE;
        if (! worldObjects[x][y] && x != chestX && y != chestY)
        {
            worldObjects[x][y] = RED_FLOWER_ID;
        }
        else
        {
            i--;
            tries++;
            if (tries > 30000)
            {
                LOG("COULDN'T SPAWN ALL RED FLOWERS\n");
                break;
            }
        }
    }
    tries = 0;
    for (i=0; i < NUM_BLUE_FLOWERS; i++)
    {
        randomPos = coordOnDonut(vec(chestX/2, chestY/2), BLUE_FLOWER_SPAWN_MIN_RADIUS, BLUE_FLOWER_SPAWN_MAX_RADIUS);
        x = randomPos.x;
        if (x < 0) x += MAPSIZE;
        y = randomPos.y;
        if (y < 0) y += MAPSIZE;
        if (! worldObjects[x][y] && x != keyX && y != keyY)
        {
            worldObjects[x][y] = BLUE_FLOWER_ID;
        }
        else
        {
            i--;
            tries++;
            if (tries > 30000)
            {
                LOG("COULDN'T SPAWN ALL BLUE FLOWERS\n");
                break;
            }
        }
    }

    tries = 0;
    for (i=0; i < NUM_ANIMALS_PER_TYPE; i++)
    {
        x = Random().randrange(MAPSIZE);
        y = Random().randrange(MAPSIZE);
        
        if (!worldObjects[x][y] && (x != keyX && y != keyY))
        {
            worldObjects[x][y] = FROG_ID;
        }
        else
        {
            i--;
            tries++;
            if (tries > 300) break;
        }
    }
    tries = 0;
    for (i=0; i < NUM_ANIMALS_PER_TYPE; i++)
    {
        x = Random().randrange(MAPSIZE/2) * 2
        y = Random().randrange(MAPSIZE/2) * 2
        
        if (!worldObjects[x][y] && (x != keyX && y != keyY))
        {
            worldObjects[x][y] = TURTLE_ID;
        }
        else
        {
            i--;
            tries++;
            if (tries > 300) break;
        }
    }
    tries = 0;
    for (i=0; i < NUM_ANIMALS_PER_TYPE; i++)
    {
        x = Random().randrange(MAPSIZE/2) * 2
        y = Random().randrange(MAPSIZE/2) * 2
        
        if (!worldObjects[x][y] && (x != keyX && y != keyY))
        {
            worldObjects[x][y] = SNAKE_ID;
        }
        else
        {
            i--;
            tries++;
            if (tries > 300) break;
        }
    }
    tries = 0;
    for (i=0; i < NUM_ANIMALS_PER_TYPE; i++)
    {
        x = Random().randrange(MAPSIZE/2) * 2
        y = Random().randrange(MAPSIZE/2) * 2
        
        if (!worldObjects[x][y] && (x != keyX && y != keyY))
        {
            worldObjects[x][y] = LADYBUG_ID;
        }
        else
        {
            i--;
            tries++;
            if (tries > 300) break;
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
        else if (keyX/2 == i%MAPSIZE && keyY/2 == i/MAPSIZE) LOG("`");
        else if (chestX/2 == i%MAPSIZE && chestY/2 == i/MAPSIZE) LOG(".");
        else LOG("%d", worldObjects[i%MAPSIZE][i/MAPSIZE]);
    }
}

void Game::reset()
{
    // Reset visited map
    for (int i = 0; i < MAPSIZE; i++)
    {
        for (int j = 0; j < MAPSIZE; j++)
        {
            m_visited[i][j] = false;
        }
    }

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
        m_visited[x-1][y-1] = true;
        m_visited[x-1][y] = true;
        m_visited[x-1][y+1] = true;
        m_visited[x][y-1] = true;
        m_visited[x][y] = true;
        m_visited[x][y+1] = true;
        m_visited[x+1][y-1] = true;
        m_visited[x+1][y] = true;
        m_visited[x+1][y+1] = true;
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

    for (int i = 0; i < MAPSIZE; i+=1)
    {
        for (int j = 0; j < MAPSIZE; j+=1)
        {
            if(m_visited[i][j] == true && !worldObjects[i][j])
                draw.fill(vec(i, j), vec(1,1), Visited);
            else
                draw.fill(vec(i, j), vec(1,1), Unvisited);

            if(m_visited[i][j] == true)
            {
//                draw.fill(vec(i, j), vec(1,1), Visited);

                if(worldObjects[i][j])
                {
                    LOG("Adding asset tile to map.\n");
                    draw.fill(vec(i, j), vec(1,1), MapGen::intToAssetSmall(worldObjects[i][j]));
                }
            }

        }
    }

    for (int i = 0; i < CUBE_ALLOCATION; i++) {
        if(!gameCubes[i].m_isOn)
        {
//            draw.fill(vec(int(gameCubes[i].m_x), int(gameCubes[i].m_y)), vec(2,2), Unvisited);
        }
        else {
            LOG("Drawing point at %d, %d", gameCubes[i].m_x, gameCubes[i].m_y);
            draw.fill(vec((gameCubes[i].m_x - 1 + MAPSIZE) % MAPSIZE,
                    (gameCubes[i].m_y - 1 + MAPSIZE) % MAPSIZE), vec(1, 1), Highlight);
            draw.fill(vec((gameCubes[i].m_x - 1 + MAPSIZE) % MAPSIZE,
                    (gameCubes[i].m_y + MAPSIZE) % MAPSIZE), vec(1, 1), Highlight);
            draw.fill(vec((gameCubes[i].m_x - 1 + MAPSIZE) % MAPSIZE,
                    (gameCubes[i].m_y + 1 + MAPSIZE) % MAPSIZE), vec(1, 1), Highlight);
            draw.fill(vec((gameCubes[i].m_x + MAPSIZE) % MAPSIZE,
                    (gameCubes[i].m_y - 1 + MAPSIZE) % MAPSIZE), vec(1, 1), Highlight);
            draw.fill(vec((gameCubes[i].m_x + MAPSIZE) % MAPSIZE,
                    (gameCubes[i].m_y + MAPSIZE) % MAPSIZE), vec(1, 1), Highlight);
            draw.fill(vec((gameCubes[i].m_x + MAPSIZE) % MAPSIZE,
                    (gameCubes[i].m_y + 1 + MAPSIZE) % MAPSIZE), vec(1, 1), Highlight);
            draw.fill(vec((gameCubes[i].m_x + 1 + MAPSIZE) % MAPSIZE,
                    (gameCubes[i].m_y - 1+ MAPSIZE) % MAPSIZE), vec(1, 1), Highlight);
            draw.fill(vec((gameCubes[i].m_x + 1 + MAPSIZE) % MAPSIZE,
                    (gameCubes[i].m_y + 1+ MAPSIZE) % MAPSIZE), vec(1, 1), Highlight);
            draw.fill(vec((gameCubes[i].m_x + 1 + MAPSIZE) % MAPSIZE,
                    (gameCubes[i].m_y + MAPSIZE) % MAPSIZE), vec(1, 1), Highlight);
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
