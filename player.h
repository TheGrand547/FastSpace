#pragma once
#ifndef PLAYER_H
#define PLAYER_H
#include "ship.h"
#include "ship_types.h"

Ship *CreatePlayer();
Action PlayerShip(Ship *Ship);
void FreePlayerShip(Ship *ship);

#endif // PLAYER_H
