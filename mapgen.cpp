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
		return ( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
	}

	float smoothNoise(int x, int y)
	{
#if 1	// no smoothing
		float n = noise2(x,y);
#else
		float n = ( noise2(x-1, y-1)+noise2(x+1, y-1)+noise2(x-1, y+1)+noise2(x+1, y+1) ) * 0.0625; // corners
		n+= ( noise2(x-1, y)  +noise2(x+1, y)  +noise2(x, y-1)  +noise2(x, y+1) ) * 0.125; // sides
		n+=  noise2(x, y) * 0.25; // center
#endif
		return n + 0.5f;	// the .5 normalizes it to [0,1]
	}

	/**
	 * Get noise value at (x,y) and map it to terrain image
	 */
	AssetImage getImage(int x, int y)
	{
		float noise = smoothNoise((x + 64) % 64, (y + 64) % 64);
		if (noise < 0.15)
		{
			return GrassLight;//Water;
		}
		else if (noise < 0.3)
		{
			return Sand;
		}
		else if (noise < 0.45)
		{
			return Dirt;
		}
		else if (noise < 0.6)
		{
			return GrassLight;
		}
		else if (noise < 0.75)
		{
			return GrassDark;
		}
		else
		{
			return Rocks;
		}
	}

	void drawMap(GameCube* gc)
	{
		BG0Drawable& draw = gc->m_vid.bg0;

		draw.image(vec(0,0), vec(4,4), getImage(gc->m_x-1, gc->m_y-1), vec(4,4));	// top left
		draw.image(vec(0,4), vec(4,8), getImage(gc->m_x-1, gc->m_y), vec(4,0));	// mid left
		draw.image(vec(0,12), vec(4,4), getImage(gc->m_x-1, gc->m_y+1), vec(4,0));	// bottom left

		draw.image(vec(4,0), vec(8,4), getImage(gc->m_x, gc->m_y-1), vec(0,4));	// top center
		draw.image(vec(4,4), vec(8,8), getImage(gc->m_x, gc->m_y), vec(0,0));	// center
		draw.image(vec(4,12), vec(8,4), getImage(gc->m_x, gc->m_y+1), vec(0,0));	// bottom center

		draw.image(vec(12,0), vec(4,4), getImage(gc->m_x+1, gc->m_y-1), vec(0,4));	// top right
		draw.image(vec(12,4), vec(4,8), getImage(gc->m_x+1, gc->m_y), vec(0,0));		// center right
		draw.image(vec(12,12), vec(4,4), getImage(gc->m_x+1, gc->m_y+1), vec(0,0));	// bottom right
	}

    void drawMiniMap(GameCube* gc)
    {
        BG0Drawable &draw = gc->m_vid.bg0;
        draw.fill(vec(0,0), vec(1,16), Highlight);
        draw.fill(vec(0,0), vec(16,1), Highlight);
        draw.fill(vec(15,0), vec(1,16), Highlight);
        draw.fill(vec(0,15), vec(16,1), Highlight);
    }
}
#endif

