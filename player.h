#pragma once
#ifndef PLAYER_H
#define PLAYER_H
#include "ship.h"

Ship *CreatePlayer();
void FreePlayerShip(Ship *ship);

#endif // PLAYER_H
