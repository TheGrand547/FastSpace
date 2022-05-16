#pragma once
#ifndef FIELD_H
#define FILED_H
#include <SDL2/SDL.h>

typedef struct
{
    Uint8 width : 4, height : 4;
    Uint8 rectWidth, rectHeight;
    Uint8 basePointX, basePointY;
    Uint8 spacing;
} Field;


void DrawField(Field *GameField);
#endif // FIELD_H
