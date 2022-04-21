#include "ship_types.h"
#include "player.h"
#include <stdio.h>

static ActionFunc ActionMap[] = {NoneShip, CircleShip};
static ShipFreeFunc FreeMap[] = {FreeShip, FreeCircleShip, FreePlayerShip};

Action ActivateShip(void *data)
{
    if (!data)
        return NONE;
    Ship *ship = (Ship*) data;
    // This could be converted to a array with the index being the cases hmmm
    // values would be function pointers, unsure if the call stack cost would
    // be advantageous but idk
    Action action = ActionMap[ship->type](ship);
    switch (action)
    {
        case SHOOT:
        {
            SDL_Point s = ShipNextTile(ship);
            printf("ZOOPING %i %i\n", s.x, s.y);
            break;
        }
        case TURNLEFT:
            TurnLeft(ship);
            break;
        case TURNRIGHT:
            TurnRight(ship);
            break;
        default:
            break;
    }
    return action;
}

void CleanupShip(void *data)
{
    Ship *ship = (Ship*) data;
    if (data)
        FreeMap[ship->type](ship);
}

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
