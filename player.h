#pragma once
#ifndef PLAYER_H
#define PLAYER_H
#include "ship.h"
#include "ship_types.h"

Ship *CreatePlayer(uint8_t x, uint8_t y, Facing facing);
Action ActivatePlayer(Ship *Ship);
void FreePlayer(Ship *ship);

#endif // PLAYER_H
