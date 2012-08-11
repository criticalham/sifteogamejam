/*
 * Sifteo SDK Example.
 */

#include <sifteo.h>
#include "assets.gen.h"
#include "gamecube.h"
using namespace Sifteo;

static Metadata M = Metadata()
    .title("Explorathon")
    .package("com.popcapsf.Explorathon", "0.1a")
    .icon(Icon)
    .cubeRange(3);

static VideoBuffer vid[CUBE_ALLOCATION];
static TiltShakeRecognizer motion[CUBE_ALLOCATION];
static GameCube gameCubes[CUBE_ALLOCATION];

class SensorListener {
public:
    struct Counter {
        unsigned touch;
        unsigned neighborAdd;
        unsigned neighborRemove;
    } counters[CUBE_ALLOCATION];

    int mainCube;

    void install()
    {
        Events::neighborAdd.set(&SensorListener::onNeighborAdd, this);
        Events::neighborRemove.set(&SensorListener::onNeighborRemove, this);
        //Events::cubeAccelChange.set(&SensorListener::onAccelChange, this);
        //Events::cubeTouch.set(&SensorListener::onTouch, this);
        //Events::cubeBatteryLevelChange.set(&SensorListener::onBatteryChange, this);
        Events::cubeConnect.set(&SensorListener::onConnect, this);

        // Handle already-connected cubes
        for (CubeID cube : CubeSet::connected())
            onConnect(cube);
    }

private:
    void onConnect(unsigned id)
    {
        CubeID cube(id);

        uint64_t hwid = cube.hwID();

        bzero(counters[id]);
        LOG("Cube %d connected\n", id);

        GameCube gCube = gameCubes[id];
        gameCubes[id].initialize(id, &vid[id], &motion[id]);
        // Draw initial state for all sensors
        //onAccelChange(cube);
        //onBatteryChange(cube);
        //onTouch(cube);
        //drawNeighbors(cube);

        //gCube.fillBackground(0);

        if (id == 0)
        {
            mainCube = 0;
            gameCubes[id].render();
            gameCubes[id].highlight();
        }
        else
        {
            gameCubes[id].shutOff();
        }

    }

    void onBatteryChange(unsigned id)
    {
        CubeID cube(id);
        String<32> str;
        str << "bat:   " << FixedFP(cube.batteryLevel(), 1, 3) << "\n";
        //vid[cube].bg0rom.text(vec(1,13), str);
    }

    void onTouch(unsigned id)
    {
        CubeID cube(id);
        counters[id].touch++;
        LOG("Touch event on cube #%d, state=%d\n", id, cube.isTouching());

        String<32> str;
        str << "touch: " << cube.isTouching() <<
            " (" << counters[cube].touch << ")\n";
        //vid[cube].bg0rom.text(vec(1,9), str);
    }

    void onAccelChange(unsigned id)
    {
        CubeID cube(id);
        auto accel = cube.accel();

        String<64> str;
        str << "acc: "
            << Fixed(accel.x, 3)
            << Fixed(accel.y, 3)
            << Fixed(accel.z, 3) << "\n";

        unsigned changeFlags = motion[id].update();
        if (changeFlags) {
            // Tilt/shake changed

            LOG("Tilt/shake changed, flags=%08x\n", changeFlags);

            auto tilt = motion[id].tilt;
            str << "tilt:"
                << Fixed(tilt.x, 3)
                << Fixed(tilt.y, 3)
                << Fixed(tilt.z, 3) << "\n";

            str << "shake: " << motion[id].shake;
        }

        //vid[cube].bg0rom.text(vec(1,10), str);
    }

    void onNeighborRemove(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        LOG("Neighbor Remove: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);

        if (firstID < arraysize(counters)) {
            counters[firstID].neighborRemove++;
            drawNeighbors(firstID);
        }
        if (secondID < arraysize(counters)) {
            counters[secondID].neighborRemove++;
            drawNeighbors(secondID);
        }

        if (mainCube == firstID || mainCube == secondID)
        {
            if (secondID != mainCube)
            {
                gameCubes[secondID].shutOff();
            }
            else
            {
                gameCubes[firstID].shutOff();
            }
        }
    }

    void onNeighborAdd(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        LOG("Neighbor Add: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);
/*
        if (firstID < arraysize(counters)) {
            counters[firstID].neighborAdd++;
            drawNeighbors(firstID);
        }
        if (secondID < arraysize(counters)) {
            counters[secondID].neighborAdd++;
            drawNeighbors(secondID);
        }*/

        gameCubes[mainCube].render();
        if (firstID == mainCube)
        {
            if (gameCubes[secondID].m_nb.neighborAt(TOP) == firstID)
            {
                gameCubes[secondID].setPos(gameCubes[mainCube].m_x, gameCubes[mainCube].m_y+1);
            }
            else if (gameCubes[secondID].m_nb.neighborAt(LEFT) == firstID)
            {
                gameCubes[secondID].setPos(gameCubes[mainCube].m_x+1, gameCubes[mainCube].m_y);
            }
            else if (gameCubes[secondID].m_nb.neighborAt(RIGHT) == firstID)
            {
                gameCubes[secondID].setPos(gameCubes[mainCube].m_x-1, gameCubes[mainCube].m_y);
            }
            else if (gameCubes[secondID].m_nb.neighborAt(BOTTOM) == firstID)
            {
                gameCubes[secondID].setPos(gameCubes[mainCube].m_x, gameCubes[mainCube].m_y-1);
            }
            mainCube = secondID;
            gameCubes[secondID].render();
        }
        else if (secondID == mainCube)
        {
            if (gameCubes[secondID].m_nb.neighborAt(TOP) == firstID)
            {
                gameCubes[secondID].setPos(gameCubes[mainCube].m_x, gameCubes[mainCube].m_y+1);
            }
            else if (gameCubes[secondID].m_nb.neighborAt(LEFT) == firstID)
            {
                gameCubes[secondID].setPos(gameCubes[mainCube].m_x+1, gameCubes[mainCube].m_y);
            }
            else if (gameCubes[secondID].m_nb.neighborAt(RIGHT) == firstID)
            {
                gameCubes[secondID].setPos(gameCubes[mainCube].m_x-1, gameCubes[mainCube].m_y);
            }
            else if (gameCubes[secondID].m_nb.neighborAt(BOTTOM) == firstID)
            {
                gameCubes[secondID].setPos(gameCubes[mainCube].m_x, gameCubes[mainCube].m_y-1);
            }
            gameCubes[firstID].setPos(gameCubes[mainCube].m_x, gameCubes[mainCube].m_y+1);
            mainCube = firstID;
            gameCubes[firstID].render();
        }
        gameCubes[mainCube].highlight();
    }

    void drawNeighbors(CubeID cube)
    {
        Neighborhood nb(cube);

        String<64> str;
        str << "nb "
            << Hex(nb.neighborAt(TOP), 2) << " "
            << Hex(nb.neighborAt(LEFT), 2) << " "
            << Hex(nb.neighborAt(BOTTOM), 2) << " "
            << Hex(nb.neighborAt(RIGHT), 2) << "\n";

        str << "   +" << counters[cube].neighborAdd
            << ", -" << counters[cube].neighborRemove
            << "\n\n";

        BG0ROMDrawable &draw = vid[cube].bg0rom;
        //draw.text(vec(1,6), str);
    }
};


void main()
{
    static SensorListener sensors;

    sensors.install();

    // We're entirely event-driven. Everything is
    // updated by SensorListener's event callbacks.
    while (1)
        System::paint();
}
