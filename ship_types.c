#include "ship_types.h"
#include <stdio.h>
#include "draw.h"
#include "font.h" // TODO: This is a bad dependency
#include "misc.h"
#include "player.h"

#define NULL_CHECK(x) if (!x) return;

// TODO: Make logger with freopen() on stderr

static SDL_Rect GetDrawArea(Ship *ship);

// TOOD: Add creation functions, and such
struct ShipData
{
    const ShipCreateFunc create;
    const ShipActionFunc action;
    const ShipFreeFunc free;
    const ShipDrawFunc draw;
    const char *filename; // TODO: Replace this with the appropriate function call when you do the greyscale thing
    SDL_Texture *texture;
};

static struct ShipData ShipsData[LAST_SHIP] = {
    {CreateNoneShip,   NoneShip,      FreeShip,       DrawBlankShip, NULL,       NULL}, // None ship
    {CreateCircleShip, CircleShip,    FreeCircleShip, DrawShipType,  "ship.bmp", NULL}, // Circle ship
    {CreatePlayer,     PlayerShip,    FreePlayerShip, DrawShipType,  NULL,       NULL}, // Player ship
    {CreateBullet,     GenericBullet, FreeBullet,     DrawBullet,    NULL,       NULL}  // Generic Bullet
};

/* TODO: Get this thing to work
ShipsData[NONE_SHIP] = {NoneShip,      FreeShip,       DrawBlankShip, NULL,       NULL};
ShipsData[CIRCLE]    = {CircleShip,    FreeCircleShip, DrawShipType,  "ship.bmp", NULL};
ShipsData[PLAYER]    = {PlayerShip,    FreePlayerShip, DrawShipType,  NULL,       NULL};
ShipsData[BULLET]    = {GenericBullet, FreeBullet,     DrawBullet,    NULL,       NULL};
*/

#define NUM_SHIP_TYPES LAST_SHIP


static void LoadShipimage(SDL_Renderer *renderer, unsigned int index)
{
    const char *name = ShipsData[index].filename;
    if (name)
    {
        // TODO: Change all of this when the greyscale thing comes true
        if (ShipsData[index].texture)
            SDL_DestroyTexture(ShipsData[index].texture);
        SDL_Surface *surf = SDL_LoadBMP(name);
        if (surf)
        {
            SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0xFF, 0xFF, 0xFF));
            ShipsData[index].texture = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_SetTextureBlendMode(ShipsData[index].texture, SDL_BLENDMODE_BLEND);
        }
        else
        {
            // TODO: Log something about it somewhere
        }
        SDL_FreeSurface(surf);
    }
}

//** External Setup/Cleanup Methods **//

// Forcefully load all of the ship images
void LoadShipImages()
{
    for (unsigned int i = 0; i < NUM_SHIP_TYPES; i++)
        LoadShipimage(GameRenderer, i);
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

//* Generic Activation Functions *//
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

//** Ship Function Map Implementations **//

/** Generic ship **/
Ship *CreateNoneShip(Uint8 x, Uint8 y, Facing facing)
{
    return CreateGenericShip(x, y, facing);
}

Action NoneShip(Ship *ship)
{
    if (ship->type == NONE_SHIP)
        SDL_Log("NoneShip %p was activated\n", (void*) ship);
    return NO_ACTION;
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
    SDL_SetRenderDrawColor(GameRenderer, ship->color.r,
                           ship->color.g, ship->color.b, ship->color.a);
    SDL_RenderFillRect(GameRenderer, &rect);
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
        ColorShip(ship, SDL_MapRGB(DisplayPixelFormat, 0xFF, 0x00, 0x00));
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

/** Generic Bullet **/
Ship *CreateBullet(Uint8 x, Uint8 y, Facing facing)
{
    Ship *bullet = CreateGenericShip(x, y, facing);
    if (bullet)
        bullet->type = BULLET;
    return bullet;
}

Action GenericBullet(Ship *ship)
{
    if (!ship->counter)
        ship->counter = 1;
    return NO_ACTION;
}

void DrawBullet(Ship *ship)
{
    if (ship->counter)
    {
        DrawShipType(ship);
    }
}

//** Internal Drawing Methods **//

void DrawShipType(Ship *ship)
{
    SDL_Rect rect = GetDrawArea(ship);
    SDL_Texture *texture = ShipsData[ship->type].texture;
    if (!texture)
    {
        if (ShipsData[ship->type].filename)
            LoadShipimage(GameRenderer, ship->type);
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
    SDL_RenderCopyEx(GameRenderer, texture, NULL, &rect, angle, NULL, flip);
}

static SDL_Rect GetDrawArea(Ship *ship)
{
    // This feels astonishingly sloopy, but i'm not sure what this is
    SDL_Rect rect = {GameField.basePointX + ship->x *
                    (GameField.rectWidth + GameField.spacing),
                     GameField.basePointX + ship->y *
                    (GameField.rectHeight + GameField.spacing),
                     GameField.rectWidth, GameField.rectHeight};
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
    SDL_Texture *t = SDL_CreateTextureFromSurface(GameRenderer, s);
    SDL_SetTextureColorMod(t, 0xFF, 0x00, 0x00);
    SDL_FreeSurface(s);
    free(pointer);
    return t;
}


