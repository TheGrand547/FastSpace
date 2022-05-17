#pragma once
#ifndef GRAND_FONT_H
#define GRAND_FONT_H
#include <SDL2/SDL.h>

int FontInit(); // Load the alphabet characters and nothing else
int FontQuit(); // Free all memory allocated via the font system
int FontLoadCharacters(); // Force the program to load all of the characters into memory

SDL_Point FontGetSizeFromLength(size_t length, size_t scale);
SDL_Point FontGetTextSize(const char *string, size_t scale);
SDL_Point FontGetCharSize(size_t scale);

SDL_Texture *FontRenderChar(SDL_Renderer *renderer, unsigned char ch);
SDL_Texture *FontRenderText(SDL_Renderer *renderer, const char *string, size_t size);
SDL_Texture *FontRenderTextSize(SDL_Renderer *renderer, const char *string, size_t size, SDL_Rect *rect);
#endif // GRAND_FONT_H
