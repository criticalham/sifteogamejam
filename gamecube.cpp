#define GAMECUBE
#include "gamecube.h"
#include <sifteo.h>
#include "assets.gen.h"

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

    fillBackground(0);
}

void GameCube::fillBackground(unsigned color)
{
    BG0Drawable &draw = m_vid.bg0;
    draw.erase();
}

void GameCube::highlight()
{
    BG0Drawable &draw = m_vid.bg0;
    draw.fill(vec(0,0), vec(1,16), Water);
    //draw.fill(vec(0,0), vec(16,1), Icon);
    //draw.fill(vec(15,0), vec(1,16), Icon);
    //draw.fill(vec(0,15), vec(16,1), Icon);
}

void GameCube::render()
{
    fillBackground(0);
    drawCoord();

    //updateRotation();
}

void GameCube::shutOff()
{
    BG0Drawable &draw = m_vid.bg0;
    draw.image(vec(8,8), GrassDark);
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

void GameCube::setPos(int x, int y)
{
    m_x = x;
    m_y = y;
}