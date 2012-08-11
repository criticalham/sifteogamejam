#ifndef _MAPGEN_CPP
#define _MAPGEN_CPP

/**
 * Map generation
 */

#include <sifteo.h>
#include "assets.gen.h"
using namespace Sifteo;

namespace MapGen
{
	// 2D Perlin Noise
	// http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
	float noise2(int x, int y)
	{
		unsigned n = x + y * 57;
		n = (n<<13) ^ n;
		return ( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 2147483647) / 1073741824.0);    
	}

	void drawMap(VideoBuffer* vid, CubeID &cube, int x, int y)
	{
		vid[cube].bg0.image(vec(16,16), Icon);
	}
}
#endif