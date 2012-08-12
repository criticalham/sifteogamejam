#ifndef _MAPGEN_CPP
#define _MAPGEN_CPP

/**
 * Map generation
 */

#include <sifteo.h>
#include "assets.gen.h"
#include "gamecube.h"
#include "game.h"
using namespace Sifteo;

namespace MapGen
{
  // Prime numbers from http://www.bigprimes.net/archive/prime/
  int smallSeeds[] = {14731, 14939, 15193, 15391, 14737, 14947, 15199, 15401};
  int mediumSeeds[] = {881249, 881591, 882019, 882367, 881269, 881597, 882029, 882377};
  int largeSeeds[] = {1169603341,1169603777,1169604217,1169604781,1169603359,1169603783,1169604221,1169604791};

  int seed1 = 15731;
  int seed2 = 789221;
  int seed3 = 1376312589;
  Random random;
  void randomize()
  {
    seed1 = smallSeeds[random.randint(0,7)];
    seed2 = mediumSeeds[random.randint(0,7)];
    seed3 = largeSeeds[random.randint(0,7)];
  }

  /**
	 * Integer-noise function: returns a floating-point value that ranges from -1 to +1.
	 * http://libnoise.sourceforge.net/noisegen/index.html
	 * http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
	 */
	float noise2(int x, int y)
	{
		unsigned n = x + y * 57;
		n = (n<<13) ^ n;
		// The large integers are primes, which may be modified as long as they remain prime; non-prime numbers may introduce discernible patterns to the output.
		return ( 1.0 - ( (n * (n * n * seed1 + seed2) + seed3) & 0x7fffffff) / 1073741824.0);
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
        float noise = smoothNoise((x + MAPSIZE) % MAPSIZE, (y + MAPSIZE) % MAPSIZE);
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

    PinnedAssetImage intToAsset(int i)
    {
        switch(i)
        {
            case 1:
                return Boulder;
            case 2:
                return FlowerRed;
            case 3:
                return FlowerBlue;
            case 4:
            	return Turtle;
           	case 5:
           		return Snake;
           	case 6:
           		return Frog;
            case 7:
            	return Ladybug;
            default:
                return Boulder;
        }
    }


    PinnedAssetImage intToAssetSmall(int i)
    {
        switch(i)
        {
            case 1:
                return BoulderSmall;
                break;
            case 2:
                return FlowerRedSmall;
                break;
            case 3:
                return FlowerBlueSmall;
                break;
            default:
                return BoulderSmall;
        }
    }
}
#endif
