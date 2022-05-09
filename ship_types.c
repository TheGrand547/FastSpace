#include "ship_types.h"
#include <stdio.h>
#include "draw.h"
#include "misc.h"
#include "player.h"

#define NULL_CHECK(x) if (!x) return;

// TODO: Make logger with freopen() on stderr

static SDL_Rect GetDrawArea(Ship *ship);
void DrawShipType(Ship *ship);

// TOOD: Add creation functions, and such
struct ShipData
{
    const ShipActionFunc action;
    const ShipFreeFunc free;
    const ShipDrawFunc draw;
    const char *filename; // TODO: Replace this with the appropriate function call when you do the greyscale thing
    SDL_Texture *texture;
};

static struct ShipData ShipsData[] = {
    {NoneShip,      FreeShip,          DrawBlankShip, NULL,       NULL}, // None ship
    {CircleShip,    FreeCircleShip,    DrawShipType,  "ship.bmp", NULL}, // Circle ship
    {PlayerShip,    FreePlayerShip,    DrawShipType,  NULL,       NULL}, // Player ship
    {GenericBullet, FreeGenericBullet, DrawBullet,    NULL,       NULL}  // Generic Bullet
};

#define NUM_SHIP_TYPES (sizeof(ShipsData) / sizeof(struct ShipData))

void ActivateShip(void *data)
{
    NULL_CHECK(data);
    Ship *ship = (Ship*) data;
    MoveShip(ship);
    Action action = ShipsData[ship->type].action(ship);
    switch (action)
    {
        case SHOOT:
        {
            ShootGamer(ship); // TODO: fix this hack thingy
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
    NULL_CHECK(data);
    Ship *ship = (Ship*) data;
    SDL_Log("%p Destroyed\n", (void*) ship);
    if (ship)
        ShipsData[ship->type].free(ship);
}

void DrawShip(void *data)
{
    NULL_CHECK(data);
    Ship *ship = (Ship*) data;
    if (ship)
        ShipsData[ship->type].draw(ship);
}

/** Generic ship **/
Ship *CreateNoneShip(Uint8 x, Uint8 y, Facing facing)
{
    return CreateGenericShip(x, y, facing);
}

Action NoneShip(Ship *ship)
{
    if (ship->type == NONE)
        SDL_Log("NoneShip %p was activated\n", (void*) ship);
    return NONE;
}

void FreeShip(Ship *ship)
{
    NULL_CHECK(ship);
    if (ship->data)
        fprintf(stderr, "Non-null pointer %p leaked\n", ship->data); // TODO: Logger
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
        ColorShip(ship, SDL_MapRGB(GetPixelFormat(), 0xFF, 0x00, 0x00));
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
    SDL_Texture *texture = ShipsData[ship->type].texture;
    if (!texture)
    {
        DrawBlankShip(ship);
        return;
    }
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    Facing facing = ship->facing;
    double angle = 0;
    if (facing == LEFT)
        flip = SDL_FLIP_HORIZONTAL;
    if (facing == UP)
        angle = 90;
    if (facing == DOWN)
        angle = 270;
    SDL_SetTextureColorMod(texture, ship->color.r, ship->color.g, ship->color.b);
    SDL_RenderCopyEx(GetRenderer(), texture, NULL, &rect, angle, NULL, flip);
}

void LoadShipImages()
{
    for (unsigned int i = 0; i < NUM_SHIP_TYPES; i++)
    {
        const char *name = ShipsData[i].filename;
        if (name)
        {
            // TODO: Change all of this when the greyscale thing comes true
            if (ShipsData[i].texture)
                SDL_DestroyTexture(ShipsData[i].texture);
            SDL_Surface *surf = SDL_LoadBMP(name);
            if (surf)
            {
                SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0xFF, 0xFF, 0xFF));
                ShipsData[i].texture = SDL_CreateTextureFromSurface(GetRenderer(), surf);
                SDL_SetTextureBlendMode(ShipsData[i].texture, SDL_BLENDMODE_BLEND);
            }
            else
            {
                // TODO: Log something about it somewhere
            }
            SDL_FreeSurface(surf);
        }
    }
}

void FreeShipImages()
{
    for (unsigned int i = 0; i < NUM_SHIP_TYPES; i++)
    {
        if (ShipsData[i].texture)
        {
            SDL_DestroyTexture(ShipsData[i].texture);
            ShipsData[i].texture = NULL;
        }
    }
}

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

SDL_Texture *Gamer()
{
    Uint8 array[10 * 10] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
        0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF,
        0xFF, 0x00, 0xFF, 0x00, 0x00, 0x80, 0x80, 0xFF, 0x00, 0xFF,
        0xFF, 0x00, 0xFF, 0x00, 0x8F, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
        0xFF, 0x00, 0xFF, 0x00, 0x8F, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
        0xFF, 0x00, 0xFF, 0x00, 0x80, 0x00, 0x00, 0xFF, 0x00, 0xFF,
        0xFF, 0x00, 0xFF, 0xFF, 0x8F, 0x8F, 0x8F, 0xFF, 0x00, 0xFF,
        0xFF, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    void *pointer = (void*) Uint8PixelsToUint32Pixels(array, 10, 10);
    SDL_Surface *s = SDL_CreateRGBSurfaceFrom(pointer,
                                              10, 10, 32, 4*10, 0x000000FF, 0x0000FF00,
                                              0x00FF0000, 0xFF000000);
    SDL_Texture *t = SDL_CreateTextureFromSurface(GetRenderer(), s);
    SDL_SetTextureColorMod(t, 0xFF, 0x00, 0x00);
    SDL_FreeSurface(s);
    free(pointer);
    return t;
}


