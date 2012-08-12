/*
 * Sifteo SDK Example.
 */

#include <sifteo.h>
#include "assets.gen.h"
#include "game.h"
#include "gamecube.h"
using namespace Sifteo;

static Metadata M = Metadata()
    .title("Explorathon")
    .package("com.popcapsf.Explorathon", "0.1a")
    .icon(Icon)
    .cubeRange(2, 12);

static AssetSlot MainSlot = AssetSlot::allocate()
    .bootstrap(GameAssets);

static VideoBuffer vid[CUBE_ALLOCATION];
static TiltShakeRecognizer motion[CUBE_ALLOCATION];
Game g_game;
GameCube gameCubes[CUBE_ALLOCATION];

class SensorListener {
public:
    struct Counter {
        unsigned touch;
        unsigned neighborAdd;
        unsigned neighborRemove;
    } counters[CUBE_ALLOCATION];

    int mainCube, prevMainCube;

    void install()
    {
        Events::neighborAdd.set(&SensorListener::onNeighborAdd, this);
        Events::neighborRemove.set(&SensorListener::onNeighborRemove, this);
        Events::cubeAccelChange.set(&SensorListener::onAccelChange, this);
        Events::cubeTouch.set(&SensorListener::onTouch, this);
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

        //GameCube gCube = gameCubes[id];
        gameCubes[id].initialize(id, vid[id], motion[id]);

        //vid[id].initMode(BG0);
        //vid[id].attach(cube);
        //vid[id].bg0.image(vec(0,0), Dirt);
        // Draw initial state for all sensors
        //onAccelChange(cube);
        //onBatteryChange(cube);
        //onTouch(cube);
        //drawNeighbors(cube);

        //gameCubes[id].fillBackground();

        if (id == 0)
        {
            mainCube = 0;
            gameCubes[id].m_isOn = true;
            //gameCubes[id].render();
            //gameCubes[id].highlight();
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

        g_game.handleCubeTouch(&gameCubes[id], cube.isTouching());
    }

    void onAccelChange(unsigned id)
    {
//        gameCubes[id].render();
//        CubeID cube(id);
//        auto accel = cube.accel();

        gameCubes[id].m_motion.update();
        gameCubes[id].m_motion.physicalTilt();

//        LOG("Tilts are %d %d ", gameCubes[id].m_motion.physicalTilt().x, gameCubes[id].m_motion.physicalTilt().y);

        if(gameCubes[id].m_motion.physicalTilt().x != 0 || gameCubes[id].m_motion.physicalTilt().y != 0)
        {
            if(!gameCubes[id].m_isMiniMap && !gameCubes[id].m_isOn)
            {
                gameCubes[id].render();
                gameCubes[id].m_isMiniMap = true;
            }
        }
        else
        {
            if(gameCubes[id].m_isMiniMap)
            {
                gameCubes[id].m_isMiniMap = false;
                LOG("Shutting off minimap");
                gameCubes[id].shutOff();
            }
        }
    }

    void updateMiniMaps()
    {
        for (int i = 0; i < CUBE_ALLOCATION; i++) {
            if(gameCubes[i].m_isMiniMap)
            {
                g_game.drawMiniMap(&gameCubes[i]);
            }
        }
    }

    void onNeighborRemove(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        LOG("Neighbor Remove: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);

        // Don't deal with the base station.
        if (firstID > CUBE_ALLOCATION || secondID > CUBE_ALLOCATION)
        {
            return;
        }

        GameCube *cube1 = &gameCubes[firstID];
        GameCube *cube2 = &gameCubes[secondID];

        if (cube1->m_isOn && cube2->m_isOn)
        {
            //if (cube1->isConnectedTo(cube2)) return;
            int cube1ClusterSize = cube1->clusterSize();
            int cube2ClusterSize = cube2->clusterSize();

            if ((cube1ClusterSize > cube2ClusterSize) || ((cube1ClusterSize == cube2ClusterSize && cube1->isConnectedTo(mainCube))))
            {
                if(!cube2->isConnectedTo(mainCube))
                {
                    LOG("Shut off cube %d\n", cube2->m_id);
                    cube2->shutOff();
                }
                cube1->highlight();
            }
            else
            {
                if(!cube1->isConnectedTo(mainCube))
                {
                    LOG("Shut off cube %d\n", cube1->m_id);
                    cube1->shutOff();
                }
                cube2->highlight();
            }
        }
        else if ((cube1->m_isOn && !cube2->m_isOn) || (!cube1->m_isOn && cube2->m_isOn))
        {
            // SHOULD NOT HAPPEN
        }
        else if (!cube1->m_isOn && !cube2->m_isOn)
        {
            // WHO CARES?
        }

        updateMiniMaps();
    }

    void onNeighborAdd(unsigned firstID, unsigned firstSide, unsigned secondID, unsigned secondSide)
    {
        LOG("Neighbor Add: %02x:%d - %02x:%d\n", firstID, firstSide, secondID, secondSide);

        // Don't deal with the base station.
        if (firstID > CUBE_ALLOCATION || secondID > CUBE_ALLOCATION)
        {
            return;
        }

        prevMainCube = mainCube;
        //gameCubes[prevMainCube].undoHighlight();

        GameCube *cube1 = &gameCubes[firstID];
        GameCube *cube2 = &gameCubes[secondID];

        if ((cube1->m_isOn && !cube2->m_isOn) || (!cube1->m_isOn && cube2->m_isOn))
        {
            GameCube *referenceCube;
            GameCube *newCube;

            if (cube1->m_isOn && !cube2->m_isOn)
            {
                referenceCube = cube1;
                newCube = cube2;
            }
            else
            {
                referenceCube = cube2;
                newCube = cube1;
            }

            newCube->turnOn(referenceCube->m_id);

            mainCube = newCube->m_id;
            newCube->highlight();

            LOG("Main cube is now %d\n", mainCube);
        }

        updateMiniMaps();
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
    g_game.initWithCubes(gameCubes);

    SensorListener sensors;
    sensors.install();

    AudioTracker::play(Music);

    // We're entirely event-driven. Everything is
    // updated by SensorListener's event callbacks.
    while (1)
    {
        g_game.run();
    }
}
