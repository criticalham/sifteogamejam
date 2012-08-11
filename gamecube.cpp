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
    m_vid.initMode(BG0);
    m_vid.attach(m_id);
    m_motion.attach(m_id);
    m_cube = CubeID(m_id);
    m_nb = Neighborhood(m_cube);
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

    setRotation();
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

    BG0Drawable &draw = m_vid.bg0;
    draw.text(vec(1,1), Font, str);
}

void GameCube::setRotation()
{

}

void GameCube::setPos(int x, int y)
{
    m_x = x;
    m_y = y;
}