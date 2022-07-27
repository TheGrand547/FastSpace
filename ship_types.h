#pragma once
#ifndef SHIP_TYPES_H
#define SHIP_TYPES_H
#include "ship.h"

const char *HumanReadableStringFrom(Action action);

typedef enum
{
    NONE_SHIP, CIRCLE, USER, BULLET, PATROLLER, LAST_SHIP // Up to 16 so it fits in Uint8 : 4
} ShipType;

//** External Setup/Cleanup Methods **//
void LoadShipImages();
void FreeShipImages();

typedef Ship *(*ShipCreateFunc)(uint8_t x, uint8_t y, Facing facing);
typedef Action (*ShipActionFunc)(Ship *ship);
typedef void (*ShipFreeFunc)(Ship *ship);
typedef void (*ShipDrawFunc)(Ship *ship);
typedef void (*ShipFunc)(Ship *ship);

/** Generic Activation Functions **/
char *GetNameShip(void *data);
void ActivateShip(void *data);
void CleanupShip(void *data);
void DrawShip(void *data);

// I don't know if these should be here
//** Ship Function Map Declarations **//

/** None Ship **/
Ship *CreateNone(uint8_t x, uint8_t y, Facing facing);
Action ActivateNone(Ship *ship);
void FreeShip(Ship *ship);
void DrawBlankShip(Ship *ship);

/** Circle Ship **/
Ship *CreateCircle(uint8_t x, uint8_t y, Facing facing);
Action ActivateCircle(Ship *ship);
#define FreeCircle FreeShip
#define DrawCircle DrawShipType

/** Generic Bullet **/
Ship *CreateBullet(uint8_t, uint8_t y, Facing facing);
Action ActivateBullet(Ship *ship);
#define FreeBullet FreeShip
void DrawBullet(Ship *ship);

// Temporary testing stuff
void ShootGamer(Ship *ship);

#endif // SHIP_TYPES_H
