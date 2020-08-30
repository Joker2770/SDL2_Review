#ifndef __LTEXTURE_H__
#define __LTEXTURE_H__

#ifdef _WIN32
//Windows
extern "C"
{
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C" {
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#ifdef __cplusplus
};
#endif
#endif

#include <iostream>


//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(SDL_Renderer* mRenderer, std::string path);

#if defined(_SDL_TTF_H) || defined(SDL_TTF_H) || defined(SDL_TTF_H_)
	//Creates image from font string
	bool loadFromRenderedText(SDL_Renderer* mRenderer, std::string sFontPath, std::string textureText, SDL_Color textColor);
#endif

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(SDL_Renderer* mRenderer, int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	TTF_Font *mFont;

	//Image dimensions
	int mWidth;
	int mHeight;
};


#endif