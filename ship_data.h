#pragma once
#ifndef SHIP_DATA_H
#define SHIP_DATA_H
#include <inttypes.h>
#define ShipImageDataFunction(name) const struct ShipImageData * name () { return &_##name; }
#define SHIP_IMAGE_MAX_WIDTH 10
#define SHIP_IMAGE_MAX_HEIGHT 10
#define SHIP_IMAGE_MAX_SIZE SHIP_IMAGE_MAX_WIDTH * SHIP_IMAGE_MAX_HEIGHT

struct ShipImageData
{
    const uint32_t width, height;
    const uint8_t pixels[SHIP_IMAGE_MAX_SIZE];
};

typedef const struct ShipImageData *(*ShipDataFunc)();

const struct ShipImageData *NoneImageData();
const struct ShipImageData *CircleImageData();
const struct ShipImageData *PlayerImageData();
const struct ShipImageData *BulletImageData();

#endif // SHIP_DATA_H
