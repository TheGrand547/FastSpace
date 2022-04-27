#pragma once
#ifndef SHIP_TYPES_H
#define SHIP_TYPES_H
#include "ship.h"

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
} ShipType;

void LoadShipImages();
void FreeShipImages();

typedef Action (*ShipActionFunc)(Ship *ship); // Can assume ship will always be non-NULL
typedef void (*ShipFreeFunc)(Ship *ship);
typedef void (*ShipDrawFunc)(Ship *ship);

/** Static Data Map Functions **/
Action ActivateShip(void *data);
void CleanupShip(void *data);
void DrawShip(void *data);

/** None Ship **/
Ship *CreateNoneShip(Uint8 x, Uint8 y, Facing facing);
Action NoneShip(Ship *ship);
void FreeShip(Ship *ship);
void DrawBlankShip(Ship *ship);

/** Circle Ship **/
Ship *CreateCircleShip(Uint8 x, Uint8 y, Facing facing);
Action CircleShip(Ship *ship);
#define FreeCircleShip FreeShip

SDL_Texture *Gamer();

#endif // SHIP_TYPES_H
