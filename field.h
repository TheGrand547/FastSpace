#pragma once
#ifndef FIELD_H
#define FILED_H
#include <stdint.h>
#include <SDL2/SDL_rect.h>

typedef struct
{
    uint8_t width : 4, height : 4;
    uint8_t rectWidth, rectHeight;
    uint8_t basePointX, basePointY;
    uint8_t spacing;
} Field;

SDL_Rect GetTile(uint8_t x, uint8_t y);

unsigned int WindowSizeX();
unsigned int WindowSizeY();

void DrawField(Field *GameField);
#endif // FIELD_H
