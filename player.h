#pragma once
#ifndef PLAYER_H
#define PLAYER_H
#include "ship.h"
#include "ship_types.h"

Ship *CreatePlayer(uint8_t x, uint8_t y, Facing facing);
Action PlayerShip(Ship *Ship);
void FreePlayerShip(Ship *ship);

#endif // PLAYER_H
