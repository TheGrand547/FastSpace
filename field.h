#pragma once
#ifndef FIELD_H
#define FILED_H
#include <stdint.h>
#include <SDL2/SDL_rect.h>

typedef struct
{
    uint8_t width : 4, height : 4;
    uint8_t basePointX, basePointY;
    uint8_t spacing;
    uint16_t rectSize;
} Field;

SDL_Rect GetTile(uint8_t x, uint8_t y);

unsigned int IndexFromLocation(uint8_t x, uint8_t y);
unsigned int NumTiles();

unsigned int WindowSizeX();
unsigned int WindowSizeY();

void DrawField(Field *GameField);
void SetupField();
#endif // FIELD_H
