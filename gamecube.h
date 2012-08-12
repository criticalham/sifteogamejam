#ifndef GAMECUBE_H
#define GAMECUBE_H
#include <sifteo.h>

using namespace Sifteo;

class GameCube
{
    public:
        int m_id;

        int m_x, m_y, m_rotation;
        Neighborhood m_nb;
        CubeID m_cube;
        VideoBuffer m_vid;
        TiltShakeRecognizer m_motion;
        void initialize(int, VideoBuffer&, TiltShakeRecognizer&);
        void reset();
        void fillBackground(unsigned);
        void highlight();
        void render();
        void shutOff();
        void visit();
        void drawCoord();
        void updateRotation(Rotation);

        void setPos(int, int);
    private:
        int stuff;
};

#endif