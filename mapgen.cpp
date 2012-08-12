#ifndef _MAPGEN_CPP
#define _MAPGEN_CPP

/**
 * Map generation
 */

#include <sifteo.h>
#include "assets.gen.h"
#include "gamecube.h"
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

	/**
	 * Get noise value at (x,y) and map it to terrain image
	 */
	AssetImage getImage(int x, int y)
	{
		float noise = noise2(x,y);
		if (noise < 0.1)
			return Water;
		else if (noise < 0.2)
			return Sand;
		else if (noise < 0.3)
			return Dirt;
		else if (noise < 0.4)
			return GrassLight;
		else if (noise < 0.5)
			return GrassDark;
		else
			return Rocks;
	}

	void drawMap(GameCube* gc)
	{
		BG0Drawable& draw = gc->m_vid.bg0;

		UInt2 destXY = vec(0,0);
		destXY.x = 0;
		destXY.y = 0;
		UInt2 size;
		size.x = 4;
		size.y = 4;
		UInt2 srcXY;
		srcXY.x = 0;
		srcXY.y = 0;
		draw.image(destXY, size, getImage(gc->m_x-1, gc->m_y-1), srcXY);	// top left

		destXY.x = 4;
		destXY.y = 4;
		size.x = 18;
		size.y = 18;
		draw.image(destXY, size, getImage(gc->m_x-1, gc->m_y), srcXY);	// mid left
		/*draw.image(vec(0,90), getImage(gc->m_x-1, gc->m_y+1));	// bottom left

		draw.image(vec(30,0), getImage(gc->m_x, gc->m_y-1));	// top center
		draw.image(vec(30,30), getImage(gc->m_x, gc->m_y));	// center
		draw.image(vec(30,90), getImage(gc->m_x, gc->m_y+1));	// bottom center

		draw.image(vec(90,0), getImage(gc->m_x+1, gc->m_y-1));	// top right
		draw.image(vec(90,30), getImage(gc->m_x+1, gc->m_y));		// center right
		draw.image(vec(90,90), getImage(gc->m_x+1, gc->m_y+1));	// bottom right
*/
		/*
		draw.image(vec(-30,-30), getImage(gc->m_x-1, gc->m_y-1));	// top left
		draw.image(vec(-30,30), getImage(gc->m_x-1, gc->m_y));	// mid left
		draw.image(vec(-30,90), getImage(gc->m_x-1, gc->m_y+1));	// bottom left

		draw.image(vec(30,-30), getImage(gc->m_x, gc->m_y-1));	// top center
		draw.image(vec(30,30), getImage(gc->m_x, gc->m_y));	// center
		draw.image(vec(30,90), getImage(gc->m_x, gc->m_y+1));	// bottom center

		draw.image(vec(90,-30), getImage(gc->m_x+1, gc->m_y-1));	// top right
		draw.image(vec(90,30), getImage(gc->m_x+1, gc->m_y));		// center right
		draw.image(vec(90,90), getImage(gc->m_x+1, gc->m_y+1));	// bottom right
		*/
	}

	void drawMap(VideoBuffer& vid)
	{
		vid.bg0.image(vec(0,0), Water);
	}
}
#endif
