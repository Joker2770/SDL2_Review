/*************************************************************************
    > File Name: config.h
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: 2021年12月03日 星期五 14时10分10秒
 ************************************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include<iostream>
using namespace std;

#ifdef _MSC_VER
//Windows
extern "C"
{
#include "SDL.h"
#include "SDL_ttf.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C" {
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#ifdef __cplusplus
};
#endif
#endif

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;
//Maximum number of supported recording devices
const int MAX_RECORDING_DEVICES = 10;

//Maximum recording time
const int MAX_RECORDING_SECONDS = 5;

//Maximum recording time plus padding
const int RECORDING_BUFFER_SECONDS = MAX_RECORDING_SECONDS + 1;

//The various recording actions we can take
enum RecordingState
{
    SELECTING_DEVICE,
    STOPPED,
    RECORDING,
    RECORDED,
    PLAYBACK,
    ERROR
};

#endif

