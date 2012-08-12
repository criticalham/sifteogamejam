#define GAMECUBE
#include "gamecube.h"
#include <sifteo.h>
#include "assets.gen.h"
#include "mapgen.h"

static AssetSlot MainSlot = AssetSlot::allocate()
    .bootstrap(GameAssets);

void GameCube::initialize(int idIn, VideoBuffer &vid, TiltShakeRecognizer &motion)
{
    m_id = idIn;
    m_vid = vid;
    m_motion = motion;
    m_vid.initMode(BG0_BG1);
    m_vid.attach(m_id);

    // Allocate 16x2 tiles on BG1 for text at the bottom of the screen
    m_vid.bg1.setMask(BG1Mask::filled(vec(0,0), vec(16,2)));

    m_motion.attach(m_id);
    m_cube = CubeID(m_id);
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
    //draw.fill(vec(0,0), vec(1,16), Emptiness);
    //draw.fill(vec(0,0), vec(16,1), Emptiness);
    //draw.fill(vec(15,0), vec(1,16), Emptiness);
    //draw.fill(vec(0,15), vec(16,1), Emptiness);
}

void GameCube::render()
{
    fillBackground();
	MapGen::drawMap(this);
    drawCoord();

    //updateRotation();
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

    BG0Drawable &draw = m_vid.bg0;
    draw.image(vec(8,8), GrassDark);
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
    draw.
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
    m_isOn = true;
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
