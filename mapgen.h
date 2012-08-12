#ifndef _MAPGEN_H
#define _MAPGEN_H

#include <sifteo.h>
#include "gamecube.h"
using namespace Sifteo;

namespace MapGen
{
  void randomize();
	void drawMap(GameCube* gc);
	void drawMiniMap(GameCube* gc);
	PinnedAssetImage intToAsset(int i);
};

#endif