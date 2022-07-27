#include "ship.h"
#include <stdio.h>
#include <stdlib.h>
#include "draw.h"
#include "player.h"
#include "ship_types.h"

static uint32_t shipCount;

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
    if (ship->name)
        free(ship->name);
    printf("0x%p Destroyed: %u\n", (void*) ship, shipCount--);
    free(ship);
}

void MoveShip(Ship *ship)
{
    ship->x += FacingX((Facing) ship->facing);
    ship->y += FacingY((Facing) ship->facing);
}

void TurnRight(Ship *ship)
{
    ship->facing++;
}

void TurnLeft(Ship *ship)
{
    ship->facing--;
}

void ColorShip(Ship *ship, uint32_t color)
{
    SDL_GetRGBA(color, DisplayPixelFormat, &ship->color.r,
                &ship->color.g, &ship->color.b, &ship->color.a);
}

SDL_Point ShipNextTile(Ship *ship)
{
    return (SDL_Point) {ship->x + FacingX((Facing) ship->facing),
                        ship->y + FacingY((Facing) ship->facing)};

}

Ship *CreateGenericShip(uint8_t x, uint8_t y, Facing facing)
{
    if (x > GameField.width || y > GameField.height)
    {
        fprintf(stderr, "Attempting to create a ship at (%u, %u), out of bounds\n", x, y);
        return NULL;
    }
    Ship *ship = (Ship*) calloc(1, sizeof(Ship));
    printf("0x%p Created: %u\n", (void*) ship, ++shipCount);
    if (!ship)
    {
        fprintf(stderr, "Failure allocating ship.\n"); // TODO: Log
        shipCount--;
        return NULL;
    }
    *ship = (Ship) {x, y, facing, NONE_SHIP, 0,
                    (SDL_Color){0xFF, 0x00, 0xFF, 0xFF}, NULL, NULL};
    return ship;
}
