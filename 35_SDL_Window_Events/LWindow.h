/*************************************************************************
    > File Name: LWindow.h
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: 2022年03月10日 星期四 14时00分42秒
 ************************************************************************/

#ifndef __LWINDOW_H__
#define __LWINDOW_H__

#include "config.h"
#include<iostream>
using namespace std;

class LWindow
{
    public:
        //Intializes internals
        LWindow();

        //Creates window
        bool init();

        //Creates renderer from internal window
        SDL_Renderer* createRenderer();

        //Handles window events
        void handleEvent( SDL_Event& e, SDL_Renderer* renderer );

        //Deallocates internals
        void free();

        //Window dimensions
        int getWidth();
        int getHeight();

        //Window focii
        bool hasMouseFocus();
        bool hasKeyboardFocus();
        bool isMinimized();

    private:
        //Window data
        SDL_Window* mWindow;

        //Window dimensions
        int mWidth;
        int mHeight;

        //Window focus
        bool mMouseFocus;
        bool mKeyboardFocus;
        bool mFullScreen;
        bool mMinimized;
};

#endif

