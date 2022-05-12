#pragma once
#ifndef GRAND_FONT_H
#define GRAND_FONT_H
#include <SDL2/SDL.h>

// External helper function
Uint32 *Uint8PixelsToUint32Pixels(Uint8 *pointer, int width, int height);
int FontInit();
int FontQuit();

SDL_Point GetTextSize(char *string, size_t scale);
SDL_Point GetCharSize(size_t scale);
#define GetConstTextSize(string, scale) GetCharSize(scale) * (sizeof(string) / sizeof(char))

SDL_Texture *CharTexture(SDL_Renderer *renderer, char ch);
// TODO: Rename
SDL_Texture *GimmeTexture(SDL_Renderer *renderer, char *string, size_t size);

#endif // GRAND_FONT_H
