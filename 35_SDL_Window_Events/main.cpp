/*************************************************************************
    > File Name: main.cpp
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: 2021年11月18日 星期四 13时21分15秒
 ************************************************************************/

#include "config.h"
#include "LWindow.h"
#include "LTexture.h"
#include "LTimer.h"

#include <iostream>
#include <sstream>

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void closeAll();

//Our custom window
LWindow gWindow;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gSceneTexture;

//Scene textures
//LTexture gPromptTextTexture;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

        //Create window
        if( !gWindow.init() )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Create renderer for window
            gRenderer = gWindow.createRenderer();
            if( gRenderer == NULL )
            {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}

		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

    // Load scene texture
    if (!gSceneTexture.loadFromFile("35_Window_Events/window.png", gRenderer))
    {
        printf("Unable to render scene texture!\n");
        success = false;
    }

    return success;
}

void closeAll()
{
    //Free loaded images
    gSceneTexture.free();

    //Destroy window    
    SDL_DestroyRenderer( gRenderer );
	gRenderer = NULL;
    gWindow.free();

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char *argv[])
{
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

            //The frames per second cap timer
            LTimer capTimer;

            //While application is running
			while (!quit)
			{
                //Start cap timer
                capTimer.start();

                //Handle events on queue
                while( SDL_PollEvent( &e ) != 0 )
                {
                    //User requests quit
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    }

                    //Handle window events
                    gWindow.handleEvent( e, gRenderer );
                }

                //Only draw when not minimized
                if( !gWindow.isMinimized() )
                {
                    //Clear screen
                    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                    SDL_RenderClear( gRenderer );

                    //Render text textures
                    gSceneTexture.render( gRenderer, ( gWindow.getWidth() - gSceneTexture.getWidth() ) / 2, ( gWindow.getHeight() - gSceneTexture.getHeight() ) / 2 );

                    //Update screen
                    SDL_RenderPresent( gRenderer );
                }
				
				//If frame finished early
                int frameTicks = capTimer.getTicks();
                if( frameTicks < SCREEN_TICKS_PER_FRAME )
                {
                    //Wait remaining time
                    SDL_Delay( SCREEN_TICKS_PER_FRAME - frameTicks );
                }
			}
		}
	}
	
	closeAll();

	return 0;
}