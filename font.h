#pragma once
#ifndef GRAND_FONT_H
#define GRAND_FONT_H
#include <SDL2/SDL.h>

// External helper function
Uint32 *Uint8PixelsToUint32Pixels(const Uint8 *pointer, int width, int height);
int FontInit();
int FontQuit();

SDL_Point GetSizeFromLength(size_t length, size_t scale);
SDL_Point GetTextSize(const char *string, size_t scale);
SDL_Point GetCharSize(size_t scale);

SDL_Texture *CharTexture(SDL_Renderer *renderer, char ch);
SDL_Surface *CharSurface(char ch);
// TODO: Rename
SDL_Texture *GimmeTexture(SDL_Renderer *renderer, const char *string, size_t size);

#endif // GRAND_FONT_H
