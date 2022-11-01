// Pretty much all of this was copied from the libctru video camera example.
// github.com/devkitPro/3ds-examples/blob/master/camera/video/source/main.c

#include "main.hpp"
#include <malloc.h>

#define WAIT_TIMEOUT 1000000000ULL

#define SCREEN_SIZE SCREEN_W * SCREEN_H * 2
#define BUF_SIZE SCREEN_SIZE * 2

static Handle camReceiveEvent[4] = {0};
static bool captureInterrupted = false;
static u8* camBuf = nullptr;
static u32 bufSize;

void cam_Init()
{
	camInit();
	CAMU_SetSize(SELECT_OUT1_OUT2, SIZE_CTR_TOP_LCD, CONTEXT_A);
	CAMU_SetOutputFormat(SELECT_OUT1_OUT2, OUTPUT_RGB_565, CONTEXT_A);
	CAMU_SetFrameRate(SELECT_OUT1_OUT2, FRAME_RATE_30);
	CAMU_SetNoiseFilter(SELECT_OUT1_OUT2, true);
	CAMU_SetAutoExposure(SELECT_OUT1_OUT2, true);
	CAMU_SetAutoWhiteBalance(SELECT_OUT1_OUT2, true);
	CAMU_SetTrimming(PORT_CAM1, false);
	CAMU_SetTrimming(PORT_CAM2, false);
	camBuf = (u8*)malloc(BUF_SIZE);
	CAMU_GetMaxBytes(&bufSize, SCREEN_W, SCREEN_H);
	CAMU_SetTransferBytes(PORT_BOTH, bufSize, SCREEN_W, SCREEN_H);
	CAMU_Activate(SELECT_OUT1_OUT2);
	CAMU_GetBufferErrorInterruptEvent(&camReceiveEvent[0], PORT_CAM1);
	CAMU_GetBufferErrorInterruptEvent(&camReceiveEvent[1], PORT_CAM2);
	CAMU_ClearBuffer(PORT_BOTH);
	CAMU_SynchronizeVsyncTiming(SELECT_OUT1, SELECT_OUT2);
	CAMU_StartCapture(PORT_BOTH);
	//CAMU_PlayShutterSound(SHUTTER_SOUND_TYPE_MOVIE);
}

void cam_Exit()
{
	CAMU_StopCapture(PORT_BOTH);

	// Close camera event handles
	for (int i = 0; i < 4; i++)	{
		if (camReceiveEvent[i] != 0) {
			svcCloseHandle(camReceiveEvent[i]);
		}
	}

	CAMU_Activate(SELECT_NONE);

	free(camBuf);
	camBuf = nullptr;

	camExit();
}

void cam_Update()
{
	// events 2 and 3 for capture
	if (camReceiveEvent[2] == 0) {
		CAMU_SetReceiving(&camReceiveEvent[2], camBuf, PORT_CAM1, SCREEN_SIZE, (s16)bufSize);
	}
	if (camReceiveEvent[3] == 0) {
		CAMU_SetReceiving(&camReceiveEvent[3], camBuf + SCREEN_SIZE, PORT_CAM2, SCREEN_SIZE, (s16)bufSize);
	}

	if (captureInterrupted) {
		CAMU_StartCapture(PORT_BOTH);
		captureInterrupted = false;
	}

	s32 index = 0;
	svcWaitSynchronizationN(&index, camReceiveEvent, 4, false, WAIT_TIMEOUT);
	switch (index)
	{
		case 0:
			svcCloseHandle(camReceiveEvent[2]);
			camReceiveEvent[2] = 0;
			captureInterrupted = true;
		break;
		
		case 1:
			svcCloseHandle(camReceiveEvent[3]);
			camReceiveEvent[3] = 0;
			captureInterrupted = true;
		break;
		
		case 2:
			svcCloseHandle(camReceiveEvent[2]);
			camReceiveEvent[2] = 0;
		break;
		
		case 3:
			svcCloseHandle(camReceiveEvent[3]);
			camReceiveEvent[3] = 0;
		break;
		
		default:
			break;
	};
}

void* cam_GetFrame(gfx3dSide_t cam)
{
	if (captureInterrupted)
		return nullptr;
	
	return ( (cam == GFX_LEFT)? camBuf: camBuf + SCREEN_SIZE );
}