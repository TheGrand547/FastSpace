#pragma once
#ifndef BULLET_H
#define BULLET_H
#include "ship.h"

#define BULLET_X_SIZE (1.0 / 2.0)
#define BULLET_Y_SIZE (1.0 / 2.0)

// Sneaky
typedef Ship Bullet;

void DrawBullet(Bullet *bullet);
#endif // BULLET_H
