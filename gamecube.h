#ifndef GAMECUBE_H
#define GAMECUBE_H
#include <sifteo.h>

using namespace Sifteo;

class GameCube
{
    public:
        int m_id;
        bool m_isOn;

        int m_x, m_y, m_rotation;
        Neighborhood m_nb;
        CubeID m_cube;
        VideoBuffer m_vid;
        TiltShakeRecognizer m_motion;
        void initialize(int, VideoBuffer&, TiltShakeRecognizer&);
        void fillBackground();
        void highlight();
        void undoHighlight();
        void render();
        void turnOn(int referenceCubeID);
        void shutOff();
        void drawCoord();
        void updateRotation(Rotation);
        Rotation getRotation();
        void setPos(int, int);
        bool isConnectedTo(int cubeID);

    protected:
        void turnOnRecursive(int referenceCubeID, BitArray<CUBE_ALLOCATION> &seenCubes);
        void shutOffRecursive(BitArray<CUBE_ALLOCATION> &seenCubes);
        bool isConnectedToRecursive(int cubeID, BitArray<CUBE_ALLOCATION> &seenCubes);

    private:
        int stuff;
};

extern GameCube gameCubes[CUBE_ALLOCATION];

#endif