#pragma once
#ifndef SHIP_H
#define SHIP_H
#include <stdint.h>
#include "super_header.h"

int FacingX(Facing facing);
int FacingY(Facing facing);

typedef enum
{
    TURN_LEFT, TURN_RIGHT, NO_ACTION, SHOOT, TURN_AROUND, NO_GENERIC_ACTION, OVERRIDE,
    // Arbitrarily many others
} Action;

// TODO: Do a thing where you calculate what the next action is right after
// executing the previous turn so you can display their next action on screen

// Size of these is somewhat irrelevant as they'll only be passed via pointer
// wish I could have small pointers or something
typedef struct __ship
{
    uint8_t x : 4, y : 4;
    // Full uint8_t

    uint8_t facing : 2;
    uint8_t type : 4;
    uint8_t counter : 2;
    // Full uint8_t

    uint8_t toughness : 4;
    uint8_t shields : 4;
    // Full uint8_t

    uint8_t dummy; // Unused but required so the uint16 will be on a 2-byte boundary

    uint16_t collision; // toughness is 4 bits, up to 4 collisions in a tile, need 16 bits to store

    Action previous; // 4 bytes, stores next action
    Color color; // 4 bytes
    void *data; // 8 bytes, data pointer
    char *name; // 8 bytes
} Ship;

#define SHIP_INDEX_FROM_LOCATION(ship) IndexFromLocation(ship->x, ship->y)

/** Standard Functions **/
int InTileShip(uint8_t x, uint8_t y, Ship *ship);
int VerifyShipsFreed();

void DestroyShip(Ship *ship);
void VerifyShip(Ship *ship);

void MoveShip(Ship *ship);
void TurnRight(Ship *ship);
void TurnLeft(Ship *ship);
void TurnAround(Ship *ship);

void DrawNumbers(Ship *ship);

void ColorShip(Ship *ship, uint32_t color);

Ship *CreateGenericShip(uint8_t x, uint8_t y, Facing facing);
SDL_Point ShipNextTile(Ship *ship);
#endif // SHIP_H
