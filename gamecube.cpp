#define GAMECUBE
#include "gamecube.h"
#include <sifteo.h>

void GameCube::initialize(int idIn, VideoBuffer* vid, TiltShakeRecognizer* motion)
{
    m_id = idIn;
    m_vid = vid;
    m_motion = motion;
    m_vid->initMode(BG0_ROM);
    m_vid->attach(m_id);
    m_motion->attach(m_id);
    m_cube = CubeID(m_id);
    m_nb = Neighborhood(m_cube);
    m_x = 0;
    m_y = 0;

    fillBackground(0);
}

void GameCube::fillBackground(unsigned color)
{
    BG0ROMDrawable &draw = m_vid->bg0rom;
    draw.erase();
}

void GameCube::highlight()
{
    BG0ROMDrawable &draw = m_vid->bg0rom;
    draw.fill(vec(0,0), vec(1,16), draw.ORANGE | draw.SOLID_FG);
    draw.fill(vec(0,0), vec(16,1), draw.ORANGE | draw.SOLID_FG);
    draw.fill(vec(15,0), vec(1,16), draw.ORANGE | draw.SOLID_FG);
    draw.fill(vec(0,15), vec(16,1), draw.ORANGE | draw.SOLID_FG);
}

void GameCube::render()
{
    fillBackground(0);
    drawCoord();
}

void GameCube::shutOff()
{
    BG0ROMDrawable &draw = m_vid->bg0rom;
    draw.fill(vec(0,0), vec(16,16), draw.BLACK | draw.SOLID_FG);
}

void GameCube::drawCoord()
{
        String<64> str;
        str << "Coord("
            << m_x << ","
            << m_y << ")";

        BG0ROMDrawable &draw = m_vid->bg0rom;
        draw.text(vec(1,6), str);
}

void GameCube::setPos(int x, int y)
{
    m_x = x;
    m_y = y;
}