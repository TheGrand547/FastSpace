#include "ship.h"

#include <stdio.h>
#include "draw.h"
#include "player.h"
#include "ship_types.h"

static ActionFunc ActionMap[] = {NoneShip, CircleShip};
static ShipFreeFunc FreeMap[] = {FreeShip, FreeCircleShip, FreePlayerShip};

void ActivateShip(void *data)
{
    if (!data)
        return;
    Ship *ship = (Ship*) data;
    // This could be converted to a array with the index being the cases hmmm
    // values would be function pointers, unsure if the call stack cost would
    // be advantageous but idk
    switch (ActionMap[ship->type](ship))
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
}

void CleanupShip(void *data)
{
    Ship *ship = (Ship*) data;
    if (data)
        FreeMap[ship->type](ship);
}

int FacingX(Facing facing)
{
    int value = 0;
    if (facing % 4 == RIGHT)
        value = 1;
    if (facing % 4 == LEFT)
        value = -1;
    return value;
}

int FacingY(Facing facing)
{
    int value = 0;
    if (facing % 4 == DOWN)
        value = 1;
    if (facing % 4 == UP)
        value = -1;
    return value;
}

void DestroyShip(Ship *ship)
{
    printf("%p Destroyed\n", (void*)ship);
    if (ship)
        free(ship);
}

void MoveShip(void *data)
{
    if (!data)
        return;
    Ship *ship = data;
    ship->x += FacingX((Facing) ship->facing);
    ship->y += FacingY((Facing) ship->facing);
    // TODO: Some cleanup is required if it moves off the playfield,
    // will be handled in a different function
}

void TurnRight(Ship *ship)
{
    if (!ship)
        return;
    ship->facing++;
}

void TurnLeft(Ship *ship)
{
    if (!ship)
        return;
    ship->facing--;
}

void DrawShip(void *data)
{
    if (!data)
        return;
    Ship *ship = (Ship*) data;
    Field *field = GetField();
    SDL_Rect rect = {field->basePointX + ship->x *
                    (field->rectWidth + field->spacing),
                    field->basePointX + ship->y *
                    (field->rectHeight + field->spacing),
                    field->rectWidth, field->rectHeight};
    SDL_SetRenderDrawColor(GetRenderer(), ship->color.r,
                           ship->color.g, ship->color.b, ship->color.a);
    SDL_RenderFillRect(GetRenderer(), &rect);
    DrawArrow(ship->x, ship->y, ship->facing);
}

void ColorShip(Ship *ship, Uint32 color)
{
    if (ship)
        SDL_GetRGBA(color, GetPixelFormat(), &ship->color.r,
                    &ship->color.g, &ship->color.b, &ship->color.a);
}

SDL_Point ShipNextTile(Ship *ship)
{
    SDL_Point next = (SDL_Point) {0, 0};
    if (ship)
    {
        next = (SDL_Point) {ship->x + FacingX(ship->facing), ship->y + FacingY(ship->facing)};
    }
    return next;
}

Ship *CreateGenericShip(Uint8 x, Uint8 y, Facing facing)
{
    Ship *ship = (Ship*) calloc(1, sizeof(Ship));
    SDL_Log("%p Created\n", (void*) ship);
    if (!ship)
    {
        fprintf(stderr, "Failure allocating ship.\n");
        return NULL;
    }
    *ship = (Ship){x, y, facing, DEFAULT, 0,
                    (SDL_Color){0xFF, 0x00, 0xFF, 0xFF}, NULL};
    // No need to check non NULL
    return ship;
}
