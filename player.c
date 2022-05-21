#include "player.h"
#include <stdio.h>

Ship *CreatePlayer(uint8_t x, uint8_t y, Facing facing)
{
    Ship *ship = CreateGenericShip(x, y, facing);
    if (ship)
    {
        // TODO: there'll be a user struct thingy
        ship->type = USER;
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
