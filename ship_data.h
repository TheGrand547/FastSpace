#pragma once
#ifndef SHIP_DATA_H
#define SHIP_DATA_H
#include <inttypes.h>
#define ShipImageDataFunction(name) const struct ShipImageData * name () { return &_##name; }

struct ShipImageData
{
    const uint32_t width, height;
    const uint8_t pixels[100]; // TODO: Make this some kind of constant, very cringe this exists
};

typedef const struct ShipImageData *(*const ShipDataFunc)();

const struct ShipImageData *NoneImageData();
const struct ShipImageData *CircleImageData();
const struct ShipImageData *PlayerImageData();
const struct ShipImageData *BulletImageData();

#endif // SHIP_DATA_H
