#include "player.h"
#include "ship_types.h"

Ship *CreatePlayer()
{
    Ship *ship = CreateGenericShip(0, 0, RIGHT);
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
