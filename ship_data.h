#pragma once
#ifndef SHIP_DATA_H
#define SHIP_DATA_H
#include <inttypes.h>
//#define ShipImageDataFunction(name) const struct ShipImageData * name () { return &_##name; }
#define ShipImageDataFunction(name) ShipDataFunc name () { return &_##name; }

struct ShipImageData
{
    const size_t width, height;
    const uint8_t pixels[100]; // TODO: Make this some kind of constant, very cringe this exists
};

typedef struct ShipImageData *(*ShipDataFunc)();

ShipDataFunc NoneImageData;
ShipDataFunc CircleImageData;
ShipDataFunc PlayerImageData;
ShipDataFunc BulletImageData;

#endif // SHIP_DATA_H
