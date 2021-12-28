/*************************************************************************
    > File Name: Dot.h
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: 2021年11月17日 星期三 10时54分17秒
 ************************************************************************/

#ifndef __DOT_H__
#define __DOT_H__

#include "LTexture.h"
#include "config.h"
#include <vector>

//A circle stucture
struct Circle
{
    int x, y;
    int r;
};

//The dot that will move around on the screen
class Dot
{
    public:
        //The dimensions of the dot
        static const int DOT_WIDTH = 20;
        static const int DOT_HEIGHT = 20;

        //Maximum axis velocity of the dot
        static const int DOT_VEL = 1;

        //Initializes the variables
        Dot(int x, int y);

        //Takes key presses and adjusts the dot's velocity
        void handleEvent( SDL_Event& e );

        //Moves the dot and checks collision
        void move( SDL_Rect& square, Circle& circle );

        //Shows the dot on the screen
        void render(SDL_Renderer* Renderer, LTexture *gDotTexture);

        //Gets collision circle
        Circle& getCollider();

    private:
        //The X and Y offsets of the dot
        int mPosX, mPosY;

        //The velocity of the dot
        int mVelX, mVelY;
        
        //Dot's collision circle
        Circle mCollider;

        //Moves the collision circle relative to the dot's offset
        void shiftColliders();
};

#endif

