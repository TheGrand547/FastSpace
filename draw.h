#pragma once
#ifndef DRAW_H
#define DRAW_H
#include <SDL2/SDL_render.h>
#include <stdint.h>

#define SetRenderDrawColor(renderer, color) SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a)
#define SetTextureColorMod(texture, color) SDL_SetTextureColorMod(texture, color.r, color.g, color.b)

uint32_t *Uint8PixelsToUint32Pixels(const uint8_t *pointer, size_t width, size_t height);

void DrawArrow(uint8_t x, uint8_t y, uint8_t facing);
void OutlineTile(uint8_t x, uint8_t y);
void OutlineTileBufferColor(uint8_t x, uint8_t y);

#endif // DRAW_H
