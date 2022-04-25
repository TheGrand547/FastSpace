#include "ship_types.h"
#include <stdio.h>
#include "draw.h"
#include "player.h"

static SDL_Rect GetDrawArea(Ship *ship)
{
    Field field = *(GetField());
    SDL_Rect rect = {field.basePointX + ship->x *
                    (field.rectWidth + field.spacing),
                     field.basePointX + ship->y *
                    (field.rectHeight + field.spacing),
                     field.rectWidth, field.rectHeight};
    return rect;
}

void DrawShipType(Ship *ship);

static ActionFunc ActionMap[] = {NoneShip, CircleShip, NoneShip};
static ShipFreeFunc FreeMap[] = {FreeShip, FreeCircleShip, FreePlayerShip};
static ShipDrawFunc DrawMap[] = {DrawBlankShip, DrawShipType, DrawBlankShip};
static SDL_Texture *ShipTextures[1];

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
            // TODO: Shoot thingy here
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

// I would make a macro for these but even i'm not that tacky
// Definitely didn't forget how to create a macro like that
void CleanupShip(void *data)
{
    Ship *ship = (Ship*) data;
    if (ship)
        FreeMap[ship->type](ship);
}

void DrawShip(void *data)
{
    Ship *ship = (Ship*) data;
    if (ship)
        DrawMap[ship->type](ship);
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
    // If any information <- What does this mean
    free(ship);
}

void DrawBlankShip(Ship *ship)
{
    SDL_Rect rect = GetDrawArea(ship);
    SDL_SetRenderDrawColor(GetRenderer(), ship->color.r,
                           ship->color.g, ship->color.b, ship->color.a);
    SDL_RenderFillRect(GetRenderer(), &rect);
    DrawArrow(ship->x, ship->y, ship->facing);
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

void DrawShipType(Ship *ship)
{
    SDL_Rect rect = GetDrawArea(ship);
    // TODO: GET BACK TO THIS WHEN THERE IS MORE THAN ONE TYPE
    SDL_Texture *texture = ShipTextures[ship->type - 1]; // VERY VERY VERY BAD, YOU SUCK
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    Facing facing = ship->facing;
    double angle = 0;
    if (facing == LEFT)
        flip = SDL_FLIP_HORIZONTAL;
    if (facing == UP)
        angle = 90;
    if (facing == DOWN)
        angle = 270;
    SDL_RenderCopyEx(GetRenderer(), texture, NULL, &rect, angle, NULL, flip);
}

void LoadShipImages()
{
    // TODO: Make this not sloppy as shit
    SDL_Surface *surf = SDL_LoadBMP("ship.bmp");
    if (surf)
    {
        SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0xFF, 0xFF, 0xFF));
        ShipTextures[0] = SDL_CreateTextureFromSurface(GetRenderer(), surf);
        SDL_SetTextureBlendMode(ShipTextures[0], SDL_BLENDMODE_BLEND);
    }
}




