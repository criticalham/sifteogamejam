#ifndef GAMECUBE_H
#define GAMECUBE_H

#include <sifteo.h>

using namespace Sifteo;

class GameCube
{
    public:
        int m_id;

        int m_x, m_y;
        Neighborhood m_nb;
        CubeID m_cube;
        VideoBuffer* m_vid;
        TiltShakeRecognizer* m_motion;
        void initialize(int, VideoBuffer*, TiltShakeRecognizer*);
        void fillBackground(unsigned);
        void highlight();
        void render();
        void shutOff();
        void drawCoord();

        void setPos(int, int);
    private:
        int stuff;
};

#endif