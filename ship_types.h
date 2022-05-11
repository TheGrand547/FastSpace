#pragma once
#ifndef SHIP_TYPES_H
#define SHIP_TYPES_H
#include "ship.h"

typedef enum
{
    TURNLEFT, TURNRIGHT, NO_ACTION, SHOOT
    // Arbitrarily many others
} Action;

typedef enum
{
    NONE_SHIP, CIRCLE, USER, BULLET, LAST_SHIP // Up to 16 so it fits in Uint8 : 4
} ShipType;

//** External Setup/Cleanup Methods **//
void LoadShipImages();
void FreeShipImages();

// TODO: Rename ship action functions
typedef Ship *(*ShipCreateFunc)(Uint8 x, Uint8 y, Facing facing);
typedef Action (*ShipActionFunc)(Ship *ship); // Can assume ship will always be non-NULL
typedef void (*ShipFreeFunc)(Ship *ship);
typedef void (*ShipDrawFunc)(Ship *ship);
typedef void (*ShipFunc)(Ship *ship);


/** Generic Activation Functions **/
void ActivateShip(void *data);
void CleanupShip(void *data);
void DrawShip(void *data);

// Internal way of drawing a specific ship with a defined SDL_Texture in the map
// TODO: Determine if this should be static
void DrawShipType(Ship *ship);

//** Ship Function Map Declarations **//

/** None Ship **/
Ship *CreateNoneShip(Uint8 x, Uint8 y, Facing facing);
Action NoneShip(Ship *ship);
void FreeShip(Ship *ship);
void DrawBlankShip(Ship *ship);

/** Circle Ship **/
Ship *CreateCircleShip(Uint8 x, Uint8 y, Facing facing);
Action CircleShip(Ship *ship);
#define FreeCircleShip FreeShip
#define DrawCircleShip DrawShipType

/** Generic Bullet **/
Ship *CreateBullet(Uint8, Uint8 y, Facing facing);
Action GenericBullet(Ship *ship);
#define FreeBullet FreeShip
void DrawBullet(Ship *ship);

// TODO: GET RID OF THESE USELESS THINGS
SDL_Texture *Gamer();
void ShootGamer(Ship *ship);

#endif // SHIP_TYPES_H
