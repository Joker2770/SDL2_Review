/*************************************************************************
    > File Name: LTimer.h
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: 2021年11月03日 星期三 13时13分21秒
 ************************************************************************/

#ifndef _LTIMER_H_
#define _LTIMER_H_

#ifdef _MSC_VER
//Windows
extern "C"
{
#include "SDL.h"
//#include "SDL_image.h"
#include "SDL_ttf.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C" {
#endif

#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#ifdef __cplusplus
};
#endif
#endif

#include<iostream>
using namespace std;

//The application time based timer
class LTimer
{
    public:
        //Initializes variables
        LTimer();

        //The various clock actions
        void start();
        void stop();
        void pause();
        void unpause();

        //Gets the timer's time
        uint32_t getTicks();

        //Checks the status of the timer
        bool isStarted();
        bool isPaused();

    private:
        //The clock time when the timer started
        uint32_t mStartTicks;

        //The ticks stored when the timer was paused
        uint32_t mPausedTicks;

        //The timer status
        bool mPaused;
        bool mStarted;
};

#endif

