/*************************************************************************
    > File Name: main.cpp
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: 2021年11月18日 星期四 13时21分15秒
 ************************************************************************/

#include "LTexture.h"
#include "LTimer.h"
#include "config.h"

#include <iostream>
#include <sstream>

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

// Set text color as black
SDL_Color gTextColor = {0, 0, 0, 255};

//Scene textures
LTexture gPromptTextTexture;

//The text textures that specify recording device names
LTexture gDeviceTextures[ MAX_RECORDING_DEVICES ];

//Number of available devices
int gRecordingDeviceCount = 0;

//Recieved audio spec
SDL_AudioSpec gReceivedRecordingSpec;
SDL_AudioSpec gReceivedPlaybackSpec;

//Recording data buffer
Uint8* gRecordingBuffer = NULL;

//Size of data buffer
Uint32 gBufferByteSize = 0;

//Position in data buffer
Uint32 gBufferBytePosition = 0;

//Maximum position in data buffer for recording
Uint32 gBufferByteMaxPosition = 0;

//Recording/playback callbacks
void audioRecordingCallback( void* userdata, Uint8* stream, int len );
void audioPlaybackCallback( void* userdata, Uint8* stream, int len );

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
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
    gFont = TTF_OpenFont( "34_Audio_Recording/lazy.ttf", 28 );
    if( gFont == NULL )
    {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }
    else
    {
        //Load prompt texture
        if( !gPromptTextTexture.loadFromRenderedText(gRenderer, gFont, "Select your recording device:", gTextColor ) )
        {
            printf( "Unable to render prompt texture!\n" );
            success = false;
        }

        //Get capture device count
        gRecordingDeviceCount = SDL_GetNumAudioDevices( SDL_TRUE );

        //No recording devices
        if( gRecordingDeviceCount < 1 )
        {
            printf( "Unable to get audio capture device! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        //At least one device connected
        else
        {
            //Cap recording device count
            if( gRecordingDeviceCount > MAX_RECORDING_DEVICES )
            {
                gRecordingDeviceCount = MAX_RECORDING_DEVICES;
            }
            // Render device names
            std::stringstream promptText;
            for (int i = 0; i < gRecordingDeviceCount; ++i)
            {
                // Get capture device name
                promptText.str("");
                promptText << i << ": " << SDL_GetAudioDeviceName(i, SDL_TRUE);

                // Set texture from name
                gDeviceTextures[i].loadFromRenderedText(gRenderer, gFont, promptText.str().c_str(), gTextColor);
            }
        }
    }

    return success;
}

void closeAll()
{
	//Free loaded images
	gPromptTextTexture.free();
    for( int i = 0; i < MAX_RECORDING_DEVICES; ++i )
    {
        gDeviceTextures[ i ].free();
    }

	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	//Destroy window    
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

    //Free playback audio
    if( gRecordingBuffer != NULL )
    {
        delete[] gRecordingBuffer;
        gRecordingBuffer = NULL;
    }

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

            //Set the default recording state
            RecordingState currentState = SELECTING_DEVICE;

            //Audio device IDs
            SDL_AudioDeviceID recordingDeviceId = 0;
            SDL_AudioDeviceID playbackDeviceId = 0;

            //While application is running
			while (!quit)
			{
                //Start cap timer
                capTimer.start();

                // Updating recording
                if (currentState == RECORDING)
                {
                    // Lock callback
                    SDL_LockAudioDevice(recordingDeviceId);

                    // Finished recording
                    if (gBufferBytePosition > gBufferByteMaxPosition)
                    {
                        // Stop recording audio
                        SDL_PauseAudioDevice(recordingDeviceId, SDL_TRUE);

                        // Go on to next state
                        gPromptTextTexture.loadFromRenderedText(gRenderer, gFont, "Press 1 to play back. Press 2 to record again.", gTextColor);
                        currentState = RECORDED;
                    }

                    // Unlock callback
                    SDL_UnlockAudioDevice(recordingDeviceId);
                }
                //Updating playback
                else if( currentState == PLAYBACK )
                {
                    //Lock callback
                    SDL_LockAudioDevice( playbackDeviceId );

                    //Finished playback
                    if( gBufferBytePosition > gBufferByteMaxPosition )
                    {
                        //Stop playing audio
                        SDL_PauseAudioDevice( playbackDeviceId, SDL_TRUE );

                        //Go on to next state
                        gPromptTextTexture.loadFromRenderedText(gRenderer, gFont, "Press 1 to play back. Press 2 to record again.", gTextColor );
                        currentState = RECORDED;
                    }

                    //Unlock callback
                    SDL_UnlockAudioDevice( playbackDeviceId );
                }

                //Handle events on queue
                while( SDL_PollEvent( &e ) != 0 )
                {
                    //User requests quit
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    }

                    // On key press
                    switch (currentState)
                    {
                    // User is selecting recording device
                    case SELECTING_DEVICE:
                        if (e.type == SDL_KEYDOWN)
                        {
                            // Handle key press from 0 to 9
                            if (e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9)
                            {
                                // Get selection index
                                int index = e.key.keysym.sym - SDLK_0;
                                // Index is valid
                                if (index < gRecordingDeviceCount)
                                {
                                    // Default audio spec
                                    SDL_AudioSpec desiredRecordingSpec;
                                    SDL_zero(desiredRecordingSpec);
                                    desiredRecordingSpec.freq = 44100;
                                    desiredRecordingSpec.format = AUDIO_F32;
                                    desiredRecordingSpec.channels = 2;
                                    desiredRecordingSpec.samples = 4096;
                                    desiredRecordingSpec.callback = audioRecordingCallback;

                                    // Open recording device
                                    recordingDeviceId = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(index, SDL_TRUE), SDL_TRUE, &desiredRecordingSpec, &gReceivedRecordingSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
                                    // Device failed to open
                                    if (recordingDeviceId == 0)
                                    {
                                        // Report error
                                        printf("Failed to open recording device! SDL Error: %s", SDL_GetError());
                                        gPromptTextTexture.loadFromRenderedText(gRenderer, gFont, "Failed to open recording device!", gTextColor);
                                        currentState = ERROR;
                                    }
                                    // Device opened successfully
                                    else
                                    {
                                        // Default audio spec
                                        SDL_AudioSpec desiredPlaybackSpec;
                                        SDL_zero(desiredPlaybackSpec);
                                        desiredPlaybackSpec.freq = 44100;
                                        desiredPlaybackSpec.format = AUDIO_F32;
                                        desiredPlaybackSpec.channels = 2;
                                        desiredPlaybackSpec.samples = 4096;
                                        desiredPlaybackSpec.callback = audioPlaybackCallback;

                                        // Open playback device
                                        playbackDeviceId = SDL_OpenAudioDevice(NULL, SDL_FALSE, &desiredPlaybackSpec, &gReceivedPlaybackSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
                                        // Device failed to open
                                        if (playbackDeviceId == 0)
                                        {
                                            // Report error
                                            printf("Failed to open playback device! SDL Error: %s", SDL_GetError());
                                            gPromptTextTexture.loadFromRenderedText(gRenderer, gFont, "Failed to open playback device!", gTextColor);
                                            currentState = ERROR;
                                        }
                                        // Device opened successfully
                                        else
                                        {
                                            // Calculate per sample bytes
                                            int bytesPerSample = gReceivedRecordingSpec.channels * (SDL_AUDIO_BITSIZE(gReceivedRecordingSpec.format) / 8);

                                            // Calculate bytes per second
                                            int bytesPerSecond = gReceivedRecordingSpec.freq * bytesPerSample;

                                            // Calculate buffer size
                                            gBufferByteSize = RECORDING_BUFFER_SECONDS * bytesPerSecond;

                                            // Calculate max buffer use
                                            gBufferByteMaxPosition = MAX_RECORDING_SECONDS * bytesPerSecond;

                                            // Allocate and initialize byte buffer
                                            gRecordingBuffer = new Uint8[gBufferByteSize];
                                            memset(gRecordingBuffer, 0, gBufferByteSize);

                                            // Go on to next state
                                            gPromptTextTexture.loadFromRenderedText(gRenderer, gFont, "Press 1 to record for 5 seconds.", gTextColor);
                                            currentState = STOPPED;
                                        }
                                    }
                                }
                            }
                        }
                        break;

                    // User getting ready to record
                    case STOPPED:
                        // On key press
                        if (e.type == SDL_KEYDOWN)
                        {
                            // Start recording
                            if (e.key.keysym.sym == SDLK_1)
                            {
                                // Go back to beginning of buffer
                                gBufferBytePosition = 0;

                                // Start recording
                                SDL_PauseAudioDevice(recordingDeviceId, SDL_FALSE);

                                // Go on to next state
                                gPromptTextTexture.loadFromRenderedText(gRenderer, gFont, "Recording...", gTextColor);
                                currentState = RECORDING;
                            }
                        }
                        break;

                    // User has finished recording
                    case RECORDED:
                        // On key press
                        if (e.type == SDL_KEYDOWN)
                        {
                            // Start playback
                            if (e.key.keysym.sym == SDLK_1)
                            {
                                // Go back to beginning of buffer
                                gBufferBytePosition = 0;

                                // Start playback
                                SDL_PauseAudioDevice(playbackDeviceId, SDL_FALSE);

                                // Go on to next state
                                gPromptTextTexture.loadFromRenderedText(gRenderer, gFont, "Playing...", gTextColor);
                                currentState = PLAYBACK;
                            }
                            // Record again
                            if (e.key.keysym.sym == SDLK_2)
                            {
                                // Reset the buffer
                                gBufferBytePosition = 0;
                                memset(gRecordingBuffer, 0, gBufferByteSize);

                                // Start recording
                                SDL_PauseAudioDevice(recordingDeviceId, SDL_FALSE);

                                // Go on to next state
                                gPromptTextTexture.loadFromRenderedText(gRenderer, gFont, "Recording...", gTextColor);
                                currentState = RECORDING;
                            }
                        }
                        break;
                    }
                }

                //Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( gRenderer );

                //Render text textures
                gPromptTextTexture.render(gRenderer, ( SCREEN_WIDTH - gPromptTextTexture.getWidth() ) / 2, 0 );
                for( int i = 0; i < MAX_RECORDING_DEVICES; ++i )
                {
                    gDeviceTextures[ i ].render(gRenderer,  ( SCREEN_WIDTH - gDeviceTextures[ i ].getWidth() ) / 2, gPromptTextTexture.getHeight() + gDeviceTextures[ 0 ].getHeight() * i );
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

void audioRecordingCallback(void *userdata, Uint8 *stream, int len)
{
    // Copy audio from stream
    memcpy(&gRecordingBuffer[gBufferBytePosition], stream, len);

    // Move along buffer
    gBufferBytePosition += len;
}

void audioPlaybackCallback(void *userdata, Uint8 *stream, int len)
{
    // Copy audio to stream
    memcpy(stream, &gRecordingBuffer[gBufferBytePosition], len);

    // Move along buffer
    gBufferBytePosition += len;
}
