#pragma once
#ifndef SHIP_H
#define SHIP_H
#include <SDL2/SDL.h>
#include "super_header.h"

int FacingX(Facing facing);
int FacingY(Facing facing);

// Size of these is somewhat irrelevant as they'll only be passed via pointer
// wish I could have small pointers or something
typedef struct __ship
{
    uint8_t x : 4, y : 4;
    // Full Uint

    uint8_t facing : 2;
    uint8_t type : 4;
    uint8_t counter : 2;
    // Full Uint

    Color color; // 4 bytes
    void *data; // 8 bytes
} Ship;

/** Standard Functions **/
void DestroyShip(Ship *ship);
void VerifyShip(Ship *ship);

// TODO: Determine if this can me made to Ship*
void MoveShip(void *data);
void TurnRight(Ship *ship);
void TurnLeft(Ship *ship);

void ColorShip(Ship *ship, uint32_t color);

Ship* CreateGenericShip(uint8_t x, uint8_t y, Facing facing);
SDL_Point ShipNextTile(Ship *ship);
#endif // SHIP_H
