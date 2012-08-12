#define GAMECUBE
#include "gamecube.h"
#include "game.h"
#include <sifteo.h>
#include "assets.gen.h"
#include "mapgen.h"

static AssetSlot MainSlot = AssetSlot::allocate()
    .bootstrap(GameAssets);

Rotation addRotations(Rotation r1, Rotation r2);
int rotToInt(Rotation r);

void GameCube::initialize(int idIn, VideoBuffer &vid, TiltShakeRecognizer &motion)
{
    m_id = idIn;
    m_vid = vid;
    m_motion = motion;
    m_vid.initMode(BG0_BG1);
    m_vid.attach(m_id);

    // Allocate 16x2 tiles on BG1 for text at the bottom of the screen
    m_vid.bg1.setMask(BG1Mask::filled(vec(0,0), vec(16,2)));
    m_vid.bg1.fill(Transparent);

    m_motion.attach(m_id);
    m_cube = CubeID(m_id);
    reset();
}

/**
* Resets cube back to initial state
*/
void GameCube::reset()
{
    m_x = 0;
    m_y = 0;
    m_isOn = false;

    fillBackground();
}

void GameCube::fillBackground()
{
    BG0Drawable &draw = m_vid.bg0;
    draw.image(vec(0,0), Emptiness);
    draw.erase();
}

void GameCube::highlight()
{
    BG0Drawable &draw = m_vid.bg0;
    draw.fill(vec(0,0), vec(1,16), Highlight);
    draw.fill(vec(0,0), vec(16,1), Highlight);
    draw.fill(vec(15,0), vec(1,16), Highlight);
    draw.fill(vec(0,15), vec(16,1), Highlight);
}

void GameCube::undoHighlight()
{
    render();
}

void GameCube::render()
{
    fillBackground();
	MapGen::drawMap(this);
    drawCoord();
    visitAndDrawItems();

    //updateRotation();
}

/**
* Update game state based on this cube visit
*/
void GameCube::visitAndDrawItems()
{
    g_game.visitAndDrawItemsAt(this);
}

void GameCube::shutOff()
{
    BitArray<CUBE_ALLOCATION> seenCubes;
    seenCubes.clear();
    shutOffRecursive(seenCubes);
}

void GameCube::shutOffRecursive(BitArray<CUBE_ALLOCATION> &seenCubes)
{
    if (seenCubes.test(m_id))
    {
        return;
    }

    seenCubes.mark(m_id);

    LOG("Shutting off ID %d\n", m_id);

    fillBackground();
    m_isOn = false;

    Neighborhood neighborhood(m_id);

    if (neighborhood.hasNeighborAt(TOP))
    {
        gameCubes[neighborhood.neighborAt(TOP)].shutOffRecursive(seenCubes);
    }
    
    if (neighborhood.hasNeighborAt(BOTTOM))
    {
        gameCubes[neighborhood.neighborAt(BOTTOM)].shutOffRecursive(seenCubes);
    }
    
    if (neighborhood.hasNeighborAt(LEFT))
    {
        gameCubes[neighborhood.neighborAt(LEFT)].shutOffRecursive(seenCubes);
    }
    
    if (neighborhood.hasNeighborAt(RIGHT))
    {
        gameCubes[neighborhood.neighborAt(RIGHT)].shutOffRecursive(seenCubes);
    }
}

void GameCube::drawCoord()
{
    String<64> str;
    str << "Coord("
        << m_x << ","
        << m_y << ")";

    BG1Drawable &draw = m_vid.bg1;

    draw.text(vec(0,0), Font, str);
}
int i = 0;
void GameCube::updateRotation(Rotation r)
{
/*
    Neighborhood nb(m_cube);
    if (nb.neighborAt(TOP))
    {
        m_vid.setRotation(ROT_NORMAL);
    }
    else if (nb.neighborAt(BOTTOM))
    {
        m_vid.setRotation(ROT_180);
    }
    else if (nb.neighborAt(RIGHT))
    {
        m_vid.setRotation(ROT_RIGHT_90);
    }
    else if (nb.neighborAt(LEFT))
    {
        m_vid.setRotation(ROT_LEFT_90);
    }*/
    m_vid.setRotation(r);
}

Rotation GameCube::getRotation()
{
    return m_vid.rotation();
}

void GameCube::setPos(int x, int y)
{
    m_x = x;
    m_y = y;
    LOG("Tile %d: %d, %d\n", m_id, m_x, m_y);
}

void GameCube::turnOn(int referenceCubeID)
{
    BitArray<CUBE_ALLOCATION> seenCubes;
    seenCubes.clear();
    turnOnRecursive(referenceCubeID, seenCubes);
}

void GameCube::turnOnRecursive(int referenceCubeID, BitArray<CUBE_ALLOCATION> &seenCubes)
{
    if (seenCubes.test(m_id))
    {
        return;
    }

    seenCubes.mark(m_id);

    if (m_isOn)
    {
        return;
    }

    m_isOn = true;

    GameCube &referenceCube = gameCubes[referenceCubeID];
    Neighborhood referenceNeighborhood(referenceCube.m_cube);
    Neighborhood currentNeighborhood(m_cube);

    LOG("Turning on ID %d\n", m_id);

    // Ensure the attached cube is facing the same direction as the main cube
    if (referenceNeighborhood.neighborAt(TOP) == referenceCubeID)
    {
        setPos(referenceCube.m_x, referenceCube.m_y+2);

        if (currentNeighborhood.neighborAt(TOP) == referenceCubeID)
        {
            updateRotation(addRotations(ROT_180, referenceCube.getRotation()));
        }
        else if (currentNeighborhood.neighborAt(RIGHT) == referenceCubeID)
        {
            updateRotation(addRotations(ROT_LEFT_90, referenceCube.getRotation()));
        }
        else if (currentNeighborhood.neighborAt(BOTTOM) == referenceCubeID)
        {
            updateRotation(addRotations(ROT_NORMAL, referenceCube.getRotation()));
        }
        else if (currentNeighborhood.neighborAt(LEFT) == referenceCubeID)
        {
            updateRotation(addRotations(ROT_RIGHT_90, referenceCube.getRotation()));
        }
    }
    else if (referenceNeighborhood.neighborAt(LEFT) == m_id)
    {
        setPos(referenceCube.m_x-2, referenceCube.m_y);
        if (currentNeighborhood.neighborAt(TOP) == referenceCubeID)
        {
            updateRotation(addRotations(ROT_LEFT_90, referenceCube.getRotation()));
        }
        else if (currentNeighborhood.neighborAt(RIGHT) == referenceCubeID)
        {
            updateRotation(addRotations(ROT_NORMAL, referenceCube.getRotation()));
        }
        else if (currentNeighborhood.neighborAt(BOTTOM) == referenceCubeID)
        {
            updateRotation(addRotations(ROT_RIGHT_90, referenceCube.getRotation()));
        }
        else if (currentNeighborhood.neighborAt(LEFT) == referenceCubeID)
        {
            updateRotation(addRotations(ROT_180, referenceCube.getRotation()));
        }
    }

    else if (referenceNeighborhood.neighborAt(RIGHT) == m_id)
    {
        setPos(referenceCube.m_x+2, referenceCube.m_y);
        if (currentNeighborhood.neighborAt(TOP) == referenceCubeID)
        {
            updateRotation(addRotations(ROT_RIGHT_90, referenceCube.getRotation()));
        }
        else if (currentNeighborhood.neighborAt(RIGHT) == referenceCubeID)
        {
            updateRotation(addRotations(ROT_180, referenceCube.getRotation()));
        }
        else if (currentNeighborhood.neighborAt(BOTTOM) == referenceCubeID)
        {
            updateRotation(addRotations(ROT_LEFT_90, referenceCube.getRotation()));
        }
        else if (currentNeighborhood.neighborAt(LEFT) == referenceCubeID)
        {
            updateRotation(addRotations(ROT_NORMAL, referenceCube.getRotation()));
        }
    }
    else if (referenceNeighborhood.neighborAt(BOTTOM) == m_id)
    {
        setPos(referenceCube.m_x, referenceCube.m_y-2);
        if (currentNeighborhood.neighborAt(TOP) == referenceCubeID)
        {
            updateRotation(addRotations(ROT_NORMAL, referenceCube.getRotation()));
        }
        else if (currentNeighborhood.neighborAt(RIGHT) == referenceCubeID)
        {
            updateRotation(addRotations(ROT_RIGHT_90, referenceCube.getRotation()));
        }
        else if (currentNeighborhood.neighborAt(BOTTOM) == referenceCubeID)
        {
            updateRotation(addRotations(ROT_180, referenceCube.getRotation()));
        }
        else if (currentNeighborhood.neighborAt(LEFT) == referenceCubeID)
        {
            updateRotation(addRotations(ROT_LEFT_90, referenceCube.getRotation()));
        }
    }
    render();

    if (currentNeighborhood.hasNeighborAt(TOP))
    {
        gameCubes[currentNeighborhood.neighborAt(TOP)].turnOnRecursive(m_id, seenCubes);
    }
    
    if (currentNeighborhood.hasNeighborAt(BOTTOM))
    {
        gameCubes[currentNeighborhood.neighborAt(BOTTOM)].turnOnRecursive(m_id, seenCubes);
    }
    
    if (currentNeighborhood.hasNeighborAt(LEFT))
    {
        gameCubes[currentNeighborhood.neighborAt(LEFT)].turnOnRecursive(m_id, seenCubes);
    }
    
    if (currentNeighborhood.hasNeighborAt(RIGHT))
    {
        gameCubes[currentNeighborhood.neighborAt(RIGHT)].turnOnRecursive(m_id, seenCubes);
    }
}

bool GameCube::isConnectedTo(int cubeID)
{
    BitArray<CUBE_ALLOCATION> seenCubes;
    seenCubes.clear();
    return isConnectedToRecursive(cubeID, seenCubes);
}

bool GameCube::isConnectedToRecursive(int cubeID, BitArray<CUBE_ALLOCATION> &seenCubes)
{
    if (seenCubes.test(m_id))
    {
        return false;
    }

    if (m_id == cubeID)
    {
        return true;
    }

    seenCubes.mark(m_id);

    bool foundCubeID = false;
    Neighborhood neighborhood(m_id);

    if (neighborhood.hasNeighborAt(TOP))
    {
        foundCubeID = gameCubes[neighborhood.neighborAt(TOP)].isConnectedToRecursive(cubeID, seenCubes);
        if (foundCubeID) { return true; }
    }
    
    if (neighborhood.hasNeighborAt(BOTTOM))
    {
        foundCubeID = gameCubes[neighborhood.neighborAt(BOTTOM)].isConnectedToRecursive(cubeID, seenCubes);
        if (foundCubeID) { return true; }
    }
    
    if (neighborhood.hasNeighborAt(LEFT))
    {
        foundCubeID = gameCubes[neighborhood.neighborAt(LEFT)].isConnectedToRecursive(cubeID, seenCubes);
        if (foundCubeID) { return true; }
    }
    
    if (neighborhood.hasNeighborAt(RIGHT))
    {
        foundCubeID = gameCubes[neighborhood.neighborAt(RIGHT)].isConnectedToRecursive(cubeID, seenCubes);
        if (foundCubeID) { return true; }
    }

    return false;
}

Rotation addRotations(Rotation r1, Rotation r2)
{
    int rotationIndex = (rotToInt(r1) + rotToInt(r2))%4;

    switch (rotationIndex) {
        case 0:         return ROT_NORMAL;
        case 1:         return ROT_RIGHT_90;
        case 2:         return ROT_180;
        case 3:         return ROT_LEFT_90;
        default:        return ROT_NORMAL;
    }
}

int rotToInt(Rotation r)
{
    switch (r) {
        case ROT_RIGHT_90:  return 1;
        case ROT_180:       return 2;
        case ROT_LEFT_90:   return 3;
        default:            return 0;
    }
}
