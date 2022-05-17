#include "player.h"
#include <stdio.h>

Ship *CreatePlayer(Uint8 x, Uint8 y, Facing facing)
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
