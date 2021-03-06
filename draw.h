#pragma once
#ifndef DRAW_H
#define DRAW_H
#include <SDL2/SDL.h>

void DrawArrow(uint8_t x, uint8_t y, uint8_t facing);
void OutlineTile(uint8_t x, uint8_t y);
void OutlineTileBufferColor(uint8_t x, uint8_t y);

uint32_t *Uint8PixelsToUint32Pixels(const uint8_t *pointer, size_t width, size_t height);

#endif // DRAW_H
