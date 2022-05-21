#pragma once
#ifndef FIELD_H
#define FILED_H
#include <SDL2/SDL.h>

typedef struct
{
    uint8_t width : 4, height : 4;
    uint8_t rectWidth, rectHeight;
    uint8_t basePointX, basePointY;
    uint8_t spacing;
} Field;

unsigned int WindowSizeX();
unsigned int WindowSizeY();

void DrawField(Field *GameField);
#endif // FIELD_H
