#ifndef GAMECUBE_H
#define GAMECUBE_H
#include <sifteo.h>

using namespace Sifteo;

#define NORTH 0
#define EAST  1
#define SOUTH 2
#define WEST  3

class GameCube
{
    public:
        int m_id;
        bool m_isOn;

        Side m_north;
        bool m_isMiniMap;
        Rotation m_rotation;
        int m_x, m_y;
        CubeID m_cube;
        VideoBuffer m_vid;
        TiltShakeRecognizer m_motion;
        void initialize(int, VideoBuffer&, TiltShakeRecognizer&);
        void reset();
        void fillBackground();
        void highlight();
        void undoHighlight();
        void render();
        void turnOn(int referenceCubeID);
        void shutOff();
        void visitAndDrawItems();
        void drawCoord();
        void updateRotation(Rotation);
        Rotation getRotation();
        void setPos(int, int);
        bool isConnectedTo(int cubeID);
        int clusterSize();

        int directionTo(int cubeID);

    protected:
        bool busy;
        void drawLeftBorder();
        void drawRightBorder();
        void drawBottomBorder();
        void drawTopBorder();
        int clusterSizeRecursive(int &total, BitArray<CUBE_ALLOCATION> &seenCubes);
        void highlightRecursive(BitArray<CUBE_ALLOCATION> &seenCubes);
        void turnOnRecursive(int referenceCubeID, BitArray<CUBE_ALLOCATION> &seenCubes);
        void shutOffRecursive(BitArray<CUBE_ALLOCATION> &seenCubes);
        bool isConnectedToRecursive(int cubeID, BitArray<CUBE_ALLOCATION> &seenCubes);

    private:
        int stuff;
};

extern GameCube gameCubes[CUBE_ALLOCATION];

#endif