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
    Uint8 x : 4, y : 4;
    // Full Uint

    Uint8 facing : 2;
    Uint8 type : 4;
    Uint8 counter : 2;
    // Full Uint

    Color color; // 4 bytes
    void *data; // 8 bytes
} Ship;

/** Function typedefs **/
typedef void (*ShipFunc)(Ship *ship);

/** Static Data Map Functions **/
void ActivateShip(void *data);
void CleanupShip(void *data);

/** Standard Functions **/
void DestroyShip(Ship *ship);
void VerifyShip(Ship *ship);
void CopyShip(Ship *ship, Ship *ships);

void MoveShip(void *data);
void TurnRight(Ship *ship);
void TurnLeft(Ship *ship);
void UpdateShips(Ship *ship);
void DrawShip(void *data);

void ColorShip(Ship *ship, Uint32 color);

Ship* CreateGenericShip(Uint8 x, Uint8 y, Facing facing);
SDL_Point ShipNextTile(Ship *ship);

#endif // SHIP_H
