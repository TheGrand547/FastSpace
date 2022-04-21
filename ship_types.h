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
} ShipTypes;

typedef Action (*ActionFunc)(Ship *ship); // Can assume ship will always be non-NULL
typedef void (*ShipFreeFunc)(Ship *ship);

/** Static Data Map Functions **/
Action ActivateShip(void *data);
void CleanupShip(void *data);

/** None Ship **/
Ship *CreateNoneShip(Uint8 x, Uint8 y, Facing facing);
Action NoneShip();
void FreeShip();

/** Circle Ship **/
Ship *CreateCircleShip(Uint8 x, Uint8 y, Facing facing);
Action CircleShip();
#define FreeCircleShip FreeShip

#endif // SHIP_TYPES_H
