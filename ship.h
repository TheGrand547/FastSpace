#pragma once
#ifndef SHIP_H
#define SHIP_H
#include <SDL2/SDL.h>
#include "super_header.h"

int FacingX(Facing facing);
int FacingY(Facing facing);

typedef enum
{
    TURNLEFT, TURNRIGHT, NONE, SHOOT
    // Arbitrarily many others
} Action;

typedef enum
{
    DEFAULT = 0,
    CIRCLE = 1,
    USER = 2
    // Up to 16 so it fits in Uint8 : 4
} ShipTypes;

// Size of these is somewhat irrelevant as they'll only be passed via pointer
// wish I could have small pointers or something
typedef struct __ship
{
    Uint8 x : 4, y : 4;
    Uint8 facing : 2;
    Uint8 type : 4;
    Uint8 counter : 2;
    // Sizeof(Ship) = 2 * sizeof(unit8) + sizeof(Color)
    Color color;
} Ship;

/** Function typedefs **/
typedef void (*ShipFunc)(Ship *ship);
typedef Action (*ActionFunc)(Ship *ship); // Can assume ship will always be non-NULL


/** Standard Functions **/
void DestroyShip(Ship *ship);
void VerifyShip(Ship *ship);
void CopyShip(Ship *ship, Ship *ships);

void ActivateShip(void *data);
void MoveShip(void *data);
void TurnRight(Ship *ship);
void TurnLeft(Ship *ship);
void UpdateShips(Ship *ship);
void DrawShip(void *data);

void ColorShip(Ship *ship, Uint32 color);

Ship* CreateShip(Uint8 x, Uint8 y, Facing facing);
SDL_Point ShipNextTile(Ship *ship);


/** Action Functions **/
Action NoneShip();
Action CircleShip();

#endif // SHIP_H
