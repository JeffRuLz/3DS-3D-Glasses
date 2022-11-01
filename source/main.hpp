#pragma once

#include <3ds.h>

#define SCREEN_W 400
#define SCREEN_H 240

void gfx_Init();
void gfx_Exit();
void gfx_BlitFrame(gfxScreen_t screen, gfx3dSide_t side, u16* img, u16 filter = 0xFFFF);

void cam_Init();
void cam_Exit();
void cam_Update();
void* cam_GetFrame(gfx3dSide_t cam);