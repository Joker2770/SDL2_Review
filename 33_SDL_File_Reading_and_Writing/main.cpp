/*************************************************************************
    > File Name: main.cpp
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: 2021年11月18日 星期四 13时21分15秒
 ************************************************************************/

#include "LTexture.h"
#include "LTimer.h"
#include "config.h"

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void closeAll();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//The window renderer
SDL_Renderer* gRenderer = NULL;

//Globally used font
TTF_Font *gFont = NULL;

//Scene textures
LTexture gDataTextures[TOTAL_DATA];
LTexture gPromptTextTexture;

//Data points
Sint32 gData[ TOTAL_DATA ];

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
		gWindow = SDL_CreateWindow("Hello World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
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

    //Open the font
    gFont = TTF_OpenFont( "33_file_reading_and_writing/lazy.ttf", 28 );
    if( gFont == NULL )
    {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }
    else
    {
        //Set text color as black
        SDL_Color textColor = { 0, 0, 0, 255 };
        
        //Load prompt texture
        if( !gPromptTextTexture.loadFromRenderedText(gRenderer, gFont, "Enter Data: ", textColor ) )
        {
            printf( "Unable to render prompt texture!\n" );
            success = false;
        }
    }

    // Open file for reading in binary
    SDL_RWops *file = SDL_RWFromFile("33_file_reading_and_writing/nums.bin", "r+b");
    // File does not exist
    if (file == NULL)
    {
        printf("Warning: Unable to open file! SDL Error: %s\n", SDL_GetError());

        // Create file for writing
        file = SDL_RWFromFile("33_file_reading_and_writing/nums.bin", "w+b");
        if (file != NULL)
        {
            printf("New file created!\n");

            // Initialize data
            for (int i = 0; i < TOTAL_DATA; ++i)
            {
                gData[i] = 0;
                SDL_RWwrite(file, &gData[i], sizeof(Sint32), 1);
            }

            // Close file handler
            SDL_RWclose(file);
        }
        else
        {
            printf("Error: Unable to create file! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
    }
    // File exists
    else
    {
        // Load data
        printf("Reading file...!\n");
        for (int i = 0; i < TOTAL_DATA; ++i)
        {
            SDL_RWread(file, &gData[i], sizeof(Sint32), 1);
        }

        // Close file handler
        SDL_RWclose(file);
    }

    return success;
}

void closeAll()
{
    //Open data for writing
    SDL_RWops* file = SDL_RWFromFile( "33_file_reading_and_writing/nums.bin", "w+b" );
    if( file != NULL )
    {
        //Save data
        for( int i = 0; i < TOTAL_DATA; ++i )
        {
            SDL_RWwrite( file, &gData[ i ], sizeof(Sint32), 1 );
        }

        //Close file handler
        SDL_RWclose( file );
    }
    else
    {
        printf( "Error: Unable to save file! %s\n", SDL_GetError() );
    }

	//Free loaded images
	gPromptTextTexture.free();
    for (size_t i = 0; i < TOTAL_DATA; i++)
    {
        gDataTextures[i].free();
    }
    

	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	//Destroy window    
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
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

            //Text rendering color
            SDL_Color textColor = { 0, 0, 0, 0xFF };
            SDL_Color highlightColor = { 0xFF, 0, 0, 0xFF };

            //Current input point
            int currentData = 0;

            // Initialize data textures
            gDataTextures[0].loadFromRenderedText(gRenderer, gFont, std::to_string(gData[0]), highlightColor);
            for (int i = 1; i < TOTAL_DATA; ++i)
            {
                gDataTextures[i].loadFromRenderedText(gRenderer, gFont, std::to_string(gData[i]), textColor);
            }

            //While application is running
			while (!quit)
			{
                //Start cap timer
                capTimer.start();

				//Handle events on queue
                while( SDL_PollEvent( &e ) != 0 )
                {
                    //User requests quit
                    if( e.type == SDL_QUIT )
                    {
                        quit = true;
                    }
					//Special key input
                    else if( e.type == SDL_KEYDOWN )
                    {
                        switch( e.key.keysym.sym )
                        {
                            //Previous data entry
                            case SDLK_UP:
                            //Rerender previous entry input point
                            gDataTextures[ currentData ].loadFromRenderedText(gRenderer, gFont, std::to_string( gData[ currentData ] ), textColor );
                            --currentData;
                            if( currentData < 0 )
                            {
                                currentData = TOTAL_DATA - 1;
                            }
                            
                            //Rerender current entry input point
                            gDataTextures[ currentData ].loadFromRenderedText(gRenderer, gFont, std::to_string( gData[ currentData ] ), highlightColor );
                            break;
                            
                            //Next data entry
                            case SDLK_DOWN:
                            //Rerender previous entry input point
                            gDataTextures[ currentData ].loadFromRenderedText(gRenderer, gFont, std::to_string( gData[ currentData ] ), textColor );
                            ++currentData;
                            if( currentData == TOTAL_DATA )
                            {
                                currentData = 0;
                            }
                            
                            //Rerender current entry input point
                            gDataTextures[ currentData ].loadFromRenderedText(gRenderer, gFont, std::to_string( gData[ currentData ] ), highlightColor );
                            break;
                            //Decrement input point
                            case SDLK_LEFT:
                            --gData[ currentData ];
                            gDataTextures[ currentData ].loadFromRenderedText(gRenderer, gFont, std::to_string( gData[ currentData ] ), highlightColor );
                            break;
                            
                            //Increment input point
                            case SDLK_RIGHT:
                            ++gData[ currentData ];
                            gDataTextures[ currentData ].loadFromRenderedText(gRenderer, gFont, std::to_string( gData[ currentData ] ), highlightColor );
                            break;
                        }
                    }
                }

                //Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( gRenderer );

                //Render text textures
                gPromptTextTexture.render(gRenderer, ( SCREEN_WIDTH - gPromptTextTexture.getWidth() ) / 2, 0 );
                for( int i = 0; i < TOTAL_DATA; ++i )
                {
                    gDataTextures[ i ].render(gRenderer,  ( SCREEN_WIDTH - gDataTextures[ i ].getWidth() ) / 2, gPromptTextTexture.getHeight() + gDataTextures[ 0 ].getHeight() * i );
                }

                //Update screen
                SDL_RenderPresent( gRenderer );
				
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


