#pragma once
#ifndef PLAYER_H
#define PLAYER_H
#include "ship.h"
#include "ship_types.h"

Ship *CreatePlayer(Uint8 x, Uint8 y, Facing facing);
Action PlayerShip(Ship *Ship);
void FreePlayerShip(Ship *ship);

#endif // PLAYER_H
