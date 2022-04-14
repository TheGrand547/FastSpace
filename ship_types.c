#include "ship_types.h"


/** Generic ship **/
Ship *CreateNoneShip(Uint8 x, Uint8 y, Facing facing)
{
    return CreateGenericShip(x, y, facing);
}

Action NoneShip(Ship *ship)
{
    UNUSED(ship);
    SDL_Log("NoneShip %p was activated\n", (void*) ship);
    return NONE;
}

void FreeShip(Ship *ship)
{
    // If any information
    free(ship);
}

/** Circle Ship **/
Ship *CreateCircleShip(Uint8 x, Uint8 y, Facing facing)
{
    Ship *ship = CreateGenericShip(x, y, facing);
    if (ship)
    {
        ship->type = CIRCLE;
        ship->data = NULL;
    }
    return ship;
}

Action CircleShip(Ship *ship)
{
    Action value = TURNRIGHT;
    if (ship->counter)
    {
        ship->counter--;
    }
    else
    {
        ship->counter = 1;
        value = SHOOT;
    }
    return value;
}
