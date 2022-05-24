#include "player.h"
#include <stdio.h>
#include "ship_data.h"

const struct ShipImageData _PlayerImageData = {
    4, 4,
    {
        0xFF, 0x80, 0x80, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x80, 0x80, 0xFF
    }
};

ShipImageDataFunction(PlayerImageData)

Ship *CreatePlayer(uint8_t x, uint8_t y, Facing facing)
{
    Ship *ship = CreateGenericShip(x, y, facing);
    if (ship)
    {
        // TODO: there'll be a user struct thingy
        ship->type = USER;
        ColorShip(ship, SDL_MapRGBA(DisplayPixelFormat, 0xFF, 0x00, 0x00, 0xFF));
    }
    return ship;
}

void FreePlayerShip(Ship *ship)
{
    free(ship);
}

Action PlayerShip(Ship *ship)
{
    UNUSED(ship);
    // TODO: Log this
    printf("ActivateShip called on player\n");
    return NO_ACTION;
}
