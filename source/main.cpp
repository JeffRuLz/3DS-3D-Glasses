#include "main.hpp"
#include <malloc.h>
#include <stdio.h>

#define C_RED 0xF800
#define C_GREEN 0x07E0
#define C_BLUE 0x001F

typedef struct
{
	u16 left;
	u16 right;
	char lLabel[32];
	char rLabel[32];
} Glasses;

static Glasses glasses[3] = {
	{ C_RED, C_GREEN | C_BLUE, "red", "cyan" },
	{ C_GREEN, C_RED | C_BLUE, "green", "magenta" },
	{ C_BLUE, C_RED | C_GREEN, "blue", "yellow" }
};

static int lens = 0;
static bool useBothCams = false;
static bool flip = false;

static void printStats()
{
	consoleClear();
	printf("3D Glasses 3DS App\n");
	printf("------------------\n");
	printf("D-Pad U/D - lens mode: %s & %s\n", glasses[lens].lLabel, glasses[lens].rLabel);
	printf("D-Pad L/R - camera mode: %s\n", ((useBothCams)? "3D": "2D"));
	printf("A Button - flip lens: %s\n", ((flip)? "flipped": "normal"));
}

int main(int argc, char* argv[])
{
	romfsInit();
	osSetSpeedupEnable(false);
	gfx_Init();
	cam_Init();

	printStats();

	u32 keys;
	u16* buf;

	while (aptMainLoop())
	{
		cam_Update();

		// input
		hidScanInput();
		keys = hidKeysDown();

		if (keys & KEY_UP) {
			if (--lens < 0)
				lens = 2;
			printStats();
		}

		if (keys & KEY_DOWN) {
			lens = (lens+1) % 3;
			printStats();
		}

		if (keys & (KEY_LEFT | KEY_RIGHT)) {
			useBothCams = !useBothCams;
			printStats();
		}

		if (keys & KEY_A) {
			flip = !flip;
			printStats();
		}			

		// camera passthrough
		if (osGet3DSliderState() <= 0.0) {
			buf = (u16*)cam_GetFrame(GFX_LEFT);
			if (buf) {
				gfx_BlitFrame(GFX_TOP, GFX_LEFT, buf);
				gfxSwapBuffers();
			}
		}

		// 3D glasses filter
		else {
			buf = (u16*)cam_GetFrame(GFX_LEFT);
			if (buf) {
				gfx_BlitFrame(GFX_TOP, GFX_LEFT, buf, ((flip)? glasses[lens].right: glasses[lens].left));
				if (useBothCams)
					buf = (u16*)cam_GetFrame(GFX_RIGHT);
				gfx_BlitFrame(GFX_TOP, GFX_RIGHT, buf, ((flip)? glasses[lens].left: glasses[lens].right));
				gfxSwapBuffers();
			}
		}

		gspWaitForVBlank();
	}

	cam_Exit();
	gfx_Exit();

	return 0;
}