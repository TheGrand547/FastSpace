#include "ship.h"
#include <stdio.h>
#include "draw.h"

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

// Destroy ships that are no longer valid
/*
Ship* VerifyShip(Ship *head)
{
    if (!head)
        return NULL;
    int destroy = 0;
    Ship *ship = head;
    Ship *pointer;
    // Sloppy, even by my standards
    while (ship)
    {
        destroy = 0;
        pointer = ship->next;
        while (pointer)
        {
            if (ship != pointer &&
                ship->x == pointer->x && ship->y == pointer->y)
            {
                pointer = DestroyShip(pointer);
                destroy = 1;
            }
            else
            {
                pointer = pointer->next;
            }
        }
        if (destroy)
        {
            if (ship == head)
            {
                head = DestroyShip(ship);
                ship = head;
            }
            else
            {
                ship = DestroyShip(ship);
            }
        }
        if (ship)
            ship = ship->next;
    }
    return head;
}
*/

void MoveShip(void *data)
{
    if (!data)
        return;
    Ship *ship = data;
    ship->x += FacingX((Facing) ship->facing);
    ship->y += FacingY((Facing) ship->facing);
    ship->x %= GetField()->width;
    ship->y %= GetField()->height;
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

Ship* CreateShip(Uint8 x, Uint8 y, Facing facing)
{
    Ship *ship = (Ship*) calloc(1, sizeof(Ship));
    SDL_Log("%p Created\n", (void*) ship);
    if (!ship)
    {
        fprintf(stderr, "Failure allocating ship.\n");
        return NULL;
    }
    *ship = (Ship){x, y, facing, (SDL_Color){0xFF, 0x00, 0xFF, 0xFF}};
    // No need to check non NULL
    return ship;
}
