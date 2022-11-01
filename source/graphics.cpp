#include "main.hpp"

void gfx_Init()
{
	gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);

	gfxSetScreenFormat(GFX_TOP, GSP_RGB565_OES);
	gfxSetDoubleBuffering(GFX_TOP, true);
	gfxSetDoubleBuffering(GFX_BOTTOM, false);
	
	gfxSet3D(true);
}

void gfx_Exit()
{
	gfxExit();
}

void gfx_BlitFrame(gfxScreen_t screen, gfx3dSide_t side, u16* img, u16 filter)
{
	if (!img)
		return;

	u16* fb = (u16*)gfxGetFramebuffer(screen, side, NULL, NULL);

	for (int y = 0; y < SCREEN_H; y++)
	{
		for (int x = 0; x < SCREEN_W; x++)
		{
			fb[(x*SCREEN_H) + (SCREEN_H-y-1)] = *img++ & filter;
		}
	}
}