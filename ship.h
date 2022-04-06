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
    Uint8 facing : 2;
    // 6 Uint8 bits left for whatever purpose i need them for
    Color color;
} Ship;

typedef void (*ShipFunc)(Ship *ship);

void DestroyShip(Ship *ship);
void VerifyShip(Ship *ship);
void CopyShip(Ship *ship, Ship *ships);

void MoveShip(void *data);
void TurnRight(Ship *ship);
void TurnLeft(Ship *ship);
void UpdateShips(Ship *ship);
void DrawShip(void *data);

void ColorShip(Ship *ship, Uint32 color);

Ship* CreateShip(Uint8 x, Uint8 y, Facing facing);
SDL_Point ShipNextTile(Ship *ship); // TODO: standardize these names you brainlet
// By convention this should only be used on the "lead" ship
//void FuncOnShips(Ship *ship, ShipFunc func);

#endif // SHIP_H
