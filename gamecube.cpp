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
    m_rotation = ROT_NORMAL;
    m_id = idIn;
    m_vid = vid;
    m_motion = motion;
    m_vid.initMode(BG0_BG1);
    m_vid.attach(m_id);
    m_north = TOP;

    // Allocate 16x2 tiles on BG1 for text at the bottom of the screen
    //m_vid.bg1.setMask(BG1Mask::filled(vec(0,0), vec(10,10)));

    m_vid.bg1.setMask(BG1Mask::filled(vec(0,0), vec(16,16)));
    //m_vid.bg1.fillMask(vec(0,0), vec(16,16));
    m_vid.bg1.fill(Transparent);
    m_vid.bg1.setPanning(vec(-32, -32));

    m_motion.attach(m_id);
    m_cube = CubeID(m_id);
    reset();
}

/**
* Resets cube back to initial state
*/
void GameCube::reset()
{
    m_isMiniMap = false;
    
    //m_x = 0;
    //m_y = 0;
    //m_isOn = false;

    //m_vid.bg1.eraseMask();
    //m_vid.bg1.fillMask(vec(0,0), vec(10,10));
    //m_vid.bg1.fill(Transparent);
    m_north = TOP;

    render();
    if (m_id == 0) highlight();

}

void GameCube::fillBackground()
{
    BG0Drawable &draw = m_vid.bg0;
    draw.image(vec(0,0), Emptiness);
    draw.erase();
    BG1Drawable &draw1 = m_vid.bg1;
    //draw1.fill(Transparent);
    draw1.erase();
}

void GameCube::highlight()
{
    BitArray<CUBE_ALLOCATION> seenCubes;
    seenCubes.clear();
    highlightRecursive(seenCubes);
}

void GameCube::highlightRecursive(BitArray<CUBE_ALLOCATION> &seenCubes)
{
    if (seenCubes.test(m_id))
    {
        return;
    }

    seenCubes.mark(m_id);
    LOG("Highlight: cube %d\n", m_id);

    Neighborhood neighborhood(m_id);

    updateRotation(m_rotation);

    if (neighborhood.hasNeighborAt(TOP) && neighborhood.neighborAt(TOP) < CUBE_ALLOCATION)
    {
        gameCubes[neighborhood.neighborAt(TOP)].highlightRecursive(seenCubes);
    }
    else
    {
        drawTopBorder();
    }
    
    if (neighborhood.hasNeighborAt(BOTTOM) && neighborhood.neighborAt(BOTTOM) < CUBE_ALLOCATION)
    {
        gameCubes[neighborhood.neighborAt(BOTTOM)].highlightRecursive(seenCubes);
    }
    else
    {
        drawBottomBorder();
    }
    
    if (neighborhood.hasNeighborAt(LEFT) && neighborhood.neighborAt(LEFT) < CUBE_ALLOCATION)
    {
        gameCubes[neighborhood.neighborAt(LEFT)].highlightRecursive(seenCubes);
    }
    else
    {
        drawLeftBorder();
    }
    
    if (neighborhood.hasNeighborAt(RIGHT) && neighborhood.neighborAt(RIGHT) < CUBE_ALLOCATION)
    {
        gameCubes[neighborhood.neighborAt(RIGHT)].highlightRecursive(seenCubes);
    }
    else
    {
        drawRightBorder();
    }
}

void GameCube::drawTopBorder()
{
    BG0Drawable &draw = m_vid.bg0;
    switch (m_rotation)
    {
        default:
            draw.fill(vec(0,0), vec(16,1), Highlight);
            return;
        case ROT_LEFT_90:
            draw.fill(vec(15,0), vec(1,16), Highlight);
            return;
        case ROT_RIGHT_90:
            draw.fill(vec(0,0), vec(1,16), Highlight);
            return;
        case ROT_180:
            draw.fill(vec(0,15), vec(16,1), Highlight);
            return;
    }
}

void GameCube::drawBottomBorder()
{
    BG0Drawable &draw = m_vid.bg0;
    switch (m_rotation)
    {
        default:
            draw.fill(vec(0,15), vec(16,1), Highlight);
            return;
        case ROT_LEFT_90:
            draw.fill(vec(0,0), vec(1,16), Highlight);
            return;
        case ROT_RIGHT_90:
            draw.fill(vec(15,0), vec(1,16), Highlight);
            return;
        case ROT_180:
            draw.fill(vec(0,0), vec(16,1), Highlight);
            return;
    }
}

void GameCube::drawLeftBorder()
{
    BG0Drawable &draw = m_vid.bg0;
    switch (m_rotation)
    {
        default:
            draw.fill(vec(0,0), vec(1,16), Highlight);
            return;
        case ROT_LEFT_90:
            draw.fill(vec(0,0), vec(16,1), Highlight);
            return;
        case ROT_RIGHT_90:
            draw.fill(vec(0,15), vec(16,1), Highlight);
            return;
        case ROT_180:
            draw.fill(vec(15,0), vec(1,16), Highlight);
            return;
    }
    
}

void GameCube::drawRightBorder()
{
    BG0Drawable &draw = m_vid.bg0;
    switch (m_rotation)
    {
        default:
            draw.fill(vec(15,0), vec(1,16), Highlight);
            return;
        case ROT_LEFT_90:
            draw.fill(vec(0,15), vec(16,1), Highlight);
            return;
        case ROT_RIGHT_90:
            draw.fill(vec(0,0), vec(16,1), Highlight);
            return;
        case ROT_180:
            draw.fill(vec(0,0), vec(1,16), Highlight);
            return;
    }
}

void GameCube::undoHighlight()
{
    render();
}

void GameCube::render()
{

    CubeID cube(m_id);
    auto accel = cube.accel();

    if(abs(accel.x) > 40 || abs(accel.y) > 40)
    {
        fillBackground();
        LOG("Accels are %d %d %d, drawing mini map", accel.x, accel.y, accel.z);
        MapGen::drawMiniMap(this);
    }
    else
    {
        fillBackground();
        MapGen::drawMap(this);
        visitAndDrawItems();
    }

    //drawCoord();
    //updateRotation();
}

int GameCube::clusterSize()
{
    int total = 0;
    BitArray<CUBE_ALLOCATION> seenCubes;
    seenCubes.clear();
    return clusterSizeRecursive(total, seenCubes);
}

int GameCube::clusterSizeRecursive(int &total, BitArray<CUBE_ALLOCATION> &seenCubes)
{
    if (seenCubes.test(m_id))
    {
        return total;
    }

    seenCubes.mark(m_id);

    Neighborhood neighborhood(m_id);
    if (neighborhood.hasNeighborAt(TOP))
    {
        if (neighborhood.neighborAt(TOP) < CUBE_ALLOCATION)
        {
            gameCubes[neighborhood.neighborAt(TOP)].clusterSizeRecursive(total, seenCubes);
        }
    }
    
    if (neighborhood.hasNeighborAt(BOTTOM))
    {
        if (neighborhood.neighborAt(BOTTOM) < CUBE_ALLOCATION)
        {
            gameCubes[neighborhood.neighborAt(BOTTOM)].clusterSizeRecursive(total, seenCubes);
        }
    }
    
    if (neighborhood.hasNeighborAt(LEFT))
    {
        if (neighborhood.neighborAt(LEFT) < CUBE_ALLOCATION)
        {
            gameCubes[neighborhood.neighborAt(LEFT)].clusterSizeRecursive(total, seenCubes);
        }
    }
    
    if (neighborhood.hasNeighborAt(RIGHT))
    {
        if (neighborhood.neighborAt(RIGHT) < CUBE_ALLOCATION)
        {
            gameCubes[neighborhood.neighborAt(RIGHT)].clusterSizeRecursive(total, seenCubes);
        }
    }

    return total++;
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
        if (neighborhood.neighborAt(TOP) < CUBE_ALLOCATION)
        {
            gameCubes[neighborhood.neighborAt(TOP)].shutOffRecursive(seenCubes);
        }
    }
    
    if (neighborhood.hasNeighborAt(BOTTOM))
    {
        if (neighborhood.neighborAt(BOTTOM) < CUBE_ALLOCATION)
        {
            gameCubes[neighborhood.neighborAt(BOTTOM)].shutOffRecursive(seenCubes);
        }
    }
    
    if (neighborhood.hasNeighborAt(LEFT))
    {
        if (neighborhood.neighborAt(LEFT) < CUBE_ALLOCATION)
        {
            gameCubes[neighborhood.neighborAt(LEFT)].shutOffRecursive(seenCubes);
        }
    }
    
    if (neighborhood.hasNeighborAt(RIGHT))
    {
        if (neighborhood.neighborAt(RIGHT) < CUBE_ALLOCATION)
        {
            gameCubes[neighborhood.neighborAt(RIGHT)].shutOffRecursive(seenCubes);
        }
    }
}

void GameCube::drawCoord()
{
    String<64> str;
    str << "Coord("
        << m_x << ","
        << m_y << ")";

    BG1Drawable &draw = m_vid.bg1;

    //draw.text(vec(0,0), Font, str);
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
    m_rotation = r;
    m_vid.setRotation(r);
}

Rotation GameCube::getRotation()
{
    return m_rotation;//m_vid.rotation();
}

void GameCube::setPos(int x, int y)
{
    m_x = (x + 64) % 64;
    m_y = (y + 64) % 64;
    LOG("Tile %d: %d, %d\n", m_id, m_x, m_y);
}

void GameCube::turnOn(int referenceCubeID)
{
    BitArray<CUBE_ALLOCATION> seenCubes;
    seenCubes.clear();
    turnOnRecursive(referenceCubeID, seenCubes);
}

int GameCube::directionTo(int cubeID)
{
    Neighborhood neighborhood(m_id);
    Side connectedSide = neighborhood.sideOf(cubeID);

    switch (m_north)
    {
        default:
        case TOP:
            switch (connectedSide)
            {
                default:
                case TOP:    return NORTH;
                case RIGHT:  return EAST;
                case BOTTOM: return SOUTH;
                case LEFT:   return WEST;
            }
            break;

        case LEFT:
            switch (connectedSide)
            {
                default:
                case TOP:    return EAST;
                case RIGHT:  return SOUTH;
                case BOTTOM: return WEST;
                case LEFT:   return NORTH;
            }
            break;

        case BOTTOM:
            switch (connectedSide)
            {
                default:
                case TOP:    return SOUTH;
                case RIGHT:  return WEST;
                case BOTTOM: return NORTH;
                case LEFT:   return EAST;
            }
            break;

        case RIGHT:
            switch (connectedSide)
            {
                default:
                case TOP:    return WEST;
                case RIGHT:  return NORTH;
                case BOTTOM: return EAST;
                case LEFT:   return SOUTH;
            }
            break;
    }
}

void GameCube::turnOnRecursive(int referenceCubeID, BitArray<CUBE_ALLOCATION> &seenCubes)
{
    if (seenCubes.test(m_id))
    {
        return;
    }

    seenCubes.mark(m_id);

    GameCube &referenceCube = gameCubes[referenceCubeID];
    Neighborhood referenceNeighborhood(referenceCube.m_cube);
    Neighborhood currentNeighborhood(m_cube);

    if (!m_isOn)
    {
        m_isOn = true;

        LOG("Turning on ID %d\n", m_id);

        int directionMoved = referenceCube.directionTo(m_id);
        Side connectedSide = currentNeighborhood.sideOf(referenceCubeID);
        switch (directionMoved)
        {
            default:
            case NORTH:
                LOG("Cube %d is north of cube %d\n", m_id, referenceCubeID);
                setPos(referenceCube.m_x, referenceCube.m_y-2);
                switch (connectedSide)
                {
                    default:
                    case BOTTOM: m_north = TOP; break;
                    case LEFT:   m_north = RIGHT; break;
                    case TOP:    m_north = BOTTOM; break;
                    case RIGHT:  m_north = LEFT; break;
                }
                break;
            case EAST:
                LOG("Cube %d is east of cube %d\n", m_id, referenceCubeID);
                setPos(referenceCube.m_x+2, referenceCube.m_y);
                switch (connectedSide)
                {
                    default:
                    case BOTTOM: m_north = LEFT; break;
                    case LEFT:   m_north = TOP; break;
                    case TOP:    m_north = RIGHT; break;
                    case RIGHT:  m_north = BOTTOM; break;
                }
                break;
            case SOUTH:
                LOG("Cube %d is south of cube %d\n", m_id, referenceCubeID);
                setPos(referenceCube.m_x, referenceCube.m_y+2);
                switch (connectedSide)
                {
                    default:
                    case BOTTOM: m_north = BOTTOM; break;
                    case LEFT:   m_north = LEFT; break;
                    case TOP:    m_north = TOP; break;
                    case RIGHT:  m_north = RIGHT; break;
                }
                break;
            case WEST:
                LOG("Cube %d is west of cube %d\n", m_id, referenceCubeID);
                setPos(referenceCube.m_x-2, referenceCube.m_y);
                switch (connectedSide)
                {
                    default:
                    case BOTTOM: m_north = RIGHT; break;
                    case LEFT:   m_north = BOTTOM; break;
                    case TOP:    m_north = LEFT; break;
                    case RIGHT:  m_north = TOP; break;
                }
                break;
        }

        switch (m_north)
        {
            default:
            case BOTTOM: LOG("Cube %d north is BOTTOM\n", m_id); break;
            case LEFT:   LOG("Cube %d north is LEFT\n", m_id); break;
            case TOP:    LOG("Cube %d north is TOP\n", m_id); break;
            case RIGHT:  LOG("Cube %d north is RIGHT\n", m_id); break;
        }

        // Ensure the attached cube is facing the same direction as the main cube
        if (referenceNeighborhood.neighborAt(TOP) == m_id)
        {
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

        if (currentNeighborhood.hasNeighborAt(TOP))
        {
            if (currentNeighborhood.neighborAt(TOP) < CUBE_ALLOCATION)
            {
                gameCubes[currentNeighborhood.neighborAt(TOP)].turnOnRecursive(m_id, seenCubes);
            }
        }
        
        if (currentNeighborhood.hasNeighborAt(BOTTOM))
        {
            if (currentNeighborhood.neighborAt(BOTTOM) < CUBE_ALLOCATION)
            {
                gameCubes[currentNeighborhood.neighborAt(BOTTOM)].turnOnRecursive(m_id, seenCubes);
            }
        }
        
        if (currentNeighborhood.hasNeighborAt(LEFT))
        {
            if (currentNeighborhood.neighborAt(LEFT) < CUBE_ALLOCATION)
            {
                gameCubes[currentNeighborhood.neighborAt(LEFT)].turnOnRecursive(m_id, seenCubes);
            }
        }
        
        if (currentNeighborhood.hasNeighborAt(RIGHT))
        {
            if (currentNeighborhood.neighborAt(RIGHT) < CUBE_ALLOCATION)
            {
                gameCubes[currentNeighborhood.neighborAt(RIGHT)].turnOnRecursive(m_id, seenCubes);
            }
        }
    }

    render();
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
        if (neighborhood.neighborAt(TOP) < CUBE_ALLOCATION)
        {
            foundCubeID = gameCubes[neighborhood.neighborAt(TOP)].isConnectedToRecursive(cubeID, seenCubes);
            if (foundCubeID) { return true; }
        }
    }
    
    if (neighborhood.hasNeighborAt(BOTTOM))
    {
        if (neighborhood.neighborAt(BOTTOM) < CUBE_ALLOCATION)
        {
            foundCubeID = gameCubes[neighborhood.neighborAt(BOTTOM)].isConnectedToRecursive(cubeID, seenCubes);
            if (foundCubeID) { return true; }
        }
    }
    
    if (neighborhood.hasNeighborAt(LEFT))
    {
        if (neighborhood.neighborAt(LEFT) < CUBE_ALLOCATION)
        {
            foundCubeID = gameCubes[neighborhood.neighborAt(LEFT)].isConnectedToRecursive(cubeID, seenCubes);
            if (foundCubeID) { return true; }
        }
    }
    
    if (neighborhood.hasNeighborAt(RIGHT))
    {
        if (neighborhood.neighborAt(RIGHT) < CUBE_ALLOCATION)
        {
            foundCubeID = gameCubes[neighborhood.neighborAt(RIGHT)].isConnectedToRecursive(cubeID, seenCubes);
            if (foundCubeID) { return true; }
        }
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
