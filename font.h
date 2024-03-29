#pragma once
#ifndef GRAND_FONT_H
#define GRAND_FONT_H
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>
#include <stdint.h>

char **StrSplit(const char *string, const char *delimiters);
void StrSplitCleanup(char **strings);

int FontInit(); // Load the alphabet characters and nothing else
int FontQuit(); // Free all memory allocated via the font system
int FontLoadAllCharacters(); // Force the program to load all of the characters into memory

SDL_Point FontGetTextSize(const char *string, size_t scale);
SDL_Point FontGetCharSize(size_t scale);
SDL_Point FontGetCharSizeWithPadding(size_t scale);

SDL_Texture *FontRenderChar(SDL_Renderer *renderer, unsigned char ch);
SDL_Texture *FontRenderText(SDL_Renderer *renderer, const char *string, size_t size);
SDL_Texture *FontRenderTextSize(SDL_Renderer *renderer, const char *string, size_t size, SDL_Rect *rect);
SDL_Texture *FontRenderTextWrapped(SDL_Renderer *renderer, const char *string, size_t size, size_t maxWidth);
SDL_Texture *FontRenderTextWrappedSize(SDL_Renderer *renderer, const char *string, size_t size,
                                       size_t maxWidth, SDL_Rect *rect);

void FontSetHorizontalSpacing(double spacing);
void FontSetTabWidth(int width);
void FontSetVerticalSpacing(double spacing);
#endif // GRAND_FONT_H
