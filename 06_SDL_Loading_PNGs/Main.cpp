#ifdef _WIN32
//Windows
extern "C"
{
#include "SDL.h"
#include "SDL_image.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C" {
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#ifdef __cplusplus
};
#endif
#endif

#include <stdio.h>
#include <iostream>

//Key press surfaces constants


//´°¿Ú¿í¸ß
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void closeAll();

//Loads individual image
SDL_Surface* loadSurface(std::string path);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//Current displayed image
SDL_Surface* gCurrentSurface = NULL;


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

			//While application is running
			while (!quit)
			{//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
				}
				//Apply the current image
				//SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);

				//Apply the image stretched
				SDL_Rect stretchRect;
				stretchRect.x = SCREEN_WIDTH / 4;
				stretchRect.y = SCREEN_HEIGHT / 4;
				stretchRect.w = SCREEN_WIDTH / 2;
				stretchRect.h = SCREEN_HEIGHT / 2;
				SDL_BlitScaled(gCurrentSurface, NULL, gScreenSurface, &stretchRect);

				//Update the surface
				SDL_UpdateWindowSurface(gWindow);
			}
		}
	}
	closeAll();

	return 0;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Initialize PNG loading
			int imgFlags = IMG_INIT_PNG;
			if (!(IMG_Init(imgFlags) & imgFlags))
			{
				printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
				success = false;
			}
			else
			{
				//Get window surface
				gScreenSurface = SDL_GetWindowSurface(gWindow);
			}
		}
	}

	return success;
}

SDL_Surface* loadSurface(std::string path)
{
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
	}
	else
	{
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, NULL);
		if (optimizedSurface == NULL)
		{
			printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return optimizedSurface;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load default surface
	gCurrentSurface = loadSurface("./preview.png");
	if (gCurrentSurface == NULL)
	{
		printf("Failed to load default image!\n");
		success = false;
	}

	return success;
}

void closeAll()
{

	if (gScreenSurface != NULL)
	{
		SDL_FreeSurface(gScreenSurface);
		gScreenSurface = NULL;
	}

	if (gCurrentSurface != NULL)
	{
		SDL_FreeSurface(gCurrentSurface);
		gCurrentSurface = NULL;
	}

	//Destroy window
	if (gWindow != NULL)
	{
		SDL_DestroyWindow(gWindow);
		gWindow = NULL;
	}

	//Quit SDL subsystems
	SDL_Quit();
}