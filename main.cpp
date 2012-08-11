/*
 * Sifteo SDK Example.
 */

#include <sifteo.h>
#include "assets.gen.h"
using namespace Sifteo;

static Metadata M = Metadata()
    .title("Explorathon")
    .package("com.popcapsf.Explorathon", "0.1a")
    .icon(Icon)
    .cubeRange(3);

static VideoBuffer vid[CUBE_ALLOCATION];
static TiltShakeRecognizer motion[CUBE_ALLOCATION];

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
        Events::cubeAccelChange.set(&SensorListener::onAccelChange, this);
        Events::cubeTouch.set(&SensorListener::onTouch, this);
        Events::cubeBatteryLevelChange.set(&SensorListener::onBatteryChange, this);
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

        vid[id].initMode(BG0_ROM);
        vid[id].attach(id);
        motion[id].attach(id);
        /*
        // Draw the cube's identity
        String<128> str;
        str << "I am cube #" << cube << "\nHear me roar\n";
        str << "hwid " << Hex(hwid >> 32) << "\n     " << Hex(hwid) << "\n\n";
        vid[cube].bg0rom.text(vec(1,2), str);
        */
        // Draw initial state for all sensors
        onAccelChange(cube);
        onBatteryChange(cube);
        onTouch(cube);
        drawNeighbors(cube);

        if (id == 0)
        {
            mainCube = 0;
            renderWorld(cube);
        }
        else
        {
            fillBlack(cube);
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

        if (firstID == mainCube || secondID == mainCube)
        {
            if (firstID != mainCube)
            {
                CubeID cube(firstID);
                fillBlack(cube);
            }
            else if (secondID != mainCube)
            {
                CubeID cube(secondID);
                fillBlack(cube);
            }
        }
    }

    void onNeighborAdd(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        LOG("Neighbor Add: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);

        if (firstID < arraysize(counters)) {
            counters[firstID].neighborAdd++;
            drawNeighbors(firstID);
        }
        if (secondID < arraysize(counters)) {
            counters[secondID].neighborAdd++;
            drawNeighbors(secondID);
        }

        CubeID firstCube(firstID);
        CubeID secondCube(secondID);
        if (firstID == mainCube)
        {
            mainCube = secondID;
            //fillBlack(firstCube);
            renderWorld(secondCube);
        }
        else if (secondID == mainCube)
        {
            mainCube = firstID;
            renderWorld(firstCube);
            //fillBlack(secondCube);
        }
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

        drawSideIndicator(draw, nb, vec( 1,  0), vec(14,  1), TOP);
        drawSideIndicator(draw, nb, vec( 0,  1), vec( 1, 14), LEFT);
        drawSideIndicator(draw, nb, vec( 1, 15), vec(14,  1), BOTTOM);
        drawSideIndicator(draw, nb, vec(15,  1), vec( 1, 14), RIGHT);
    }

    static void drawSideIndicator(BG0ROMDrawable &draw, Neighborhood &nb,
        Int2 topLeft, Int2 size, Side s)
    {
        unsigned nbColor = draw.ORANGE;
        //draw.fill(topLeft, size,
        //    nbColor | (nb.hasNeighborAt(s) ? draw.SOLID_FG : draw.SOLID_BG));
    }

    static void drawSelectionSquare(BG0ROMDrawable &draw)
    {
        unsigned nbColor = draw.ORANGE;
        draw.fill(vec(0,0), vec(1,16), nbColor | draw.SOLID_FG);
        draw.fill(vec(0,0), vec(16,1), nbColor | draw.SOLID_FG);
        draw.fill(vec(15,0), vec(1,16), nbColor | draw.SOLID_FG);
        draw.fill(vec(0,15), vec(16,1), nbColor | draw.SOLID_FG);
    }
    static void removeSelectionSquare(BG0ROMDrawable &draw)
    {
        unsigned nbColor = draw.ORANGE;
        draw.fill(vec(0,0), vec(1,16), nbColor | draw.SOLID_BG);
        draw.fill(vec(0,0), vec(16,1), nbColor | draw.SOLID_BG);
        draw.fill(vec(15,0), vec(1,16), nbColor | draw.SOLID_BG);
        draw.fill(vec(0,15), vec(16,1), nbColor | draw.SOLID_BG);
    }

    static void fillBlack(CubeID &cube)
    {
        BG0ROMDrawable &draw = vid[cube].bg0rom;
        draw.erase(draw.BLACK);
        //draw.fill(vec(0,0), vec(16,16), draw.BLACK | draw.SOLID_FG);
    }

    static void renderWorld(CubeID &cube)
    {
        BG0ROMDrawable &draw = vid[cube].bg0rom;

        unsigned nbColor = draw.ORANGE;
        draw.erase();
        draw.fill(vec(0,0), vec(1,16), nbColor | draw.SOLID_FG);
        draw.fill(vec(0,0), vec(16,1), nbColor | draw.SOLID_FG);
        draw.fill(vec(15,0), vec(1,16), nbColor | draw.SOLID_FG);
        draw.fill(vec(0,15), vec(16,1), nbColor | draw.SOLID_FG);
        //draw.fill(vec(1,1), vec(14,14), draw.WHITE | draw.SOLID_FG);
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
