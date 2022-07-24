#include "ship_types.h"
#include <SDL2/SDL_log.h>
#include <stdio.h>
#include <stdlib.h>
#include "draw.h"
#include "misc.h"
#include "player.h"
#include "ship_data.h"

// TODO: Make logger with freopen() on stderr

static void DrawShipType(Ship *ship);

struct ShipData
{
    const ShipCreateFunc create;
    const ShipActionFunc action;
    const ShipFreeFunc free;
    const ShipDrawFunc draw;
    const ShipDataFunc imageData;
    SDL_Texture *texture;
};

static struct ShipData ShipsData[LAST_SHIP] = {
    {CreateNoneShip,   ActivateNoneShip,   FreeShip,       DrawShipType, NoneImageData, NULL}, // None ship
    {CreateCircleShip, ActivateCircleShip, FreeCircleShip, DrawShipType, CircleImageData, NULL}, // Circle ship
    {CreatePlayer,     ActivatePlayer,     FreePlayerShip, DrawShipType, PlayerImageData, NULL}, // Player ship
    {CreateBullet,     ActivateBullet,     FreeBullet,     DrawShipType, BulletImageData, NULL}  // Generic Bullet
};

#define NUM_SHIP_TYPES LAST_SHIP

static void LoadShipimage(SDL_Renderer *renderer, unsigned int index)
{
    const struct ShipImageData *data = ShipsData[index].imageData();
    if (data)
    {
        if (ShipsData[index].texture)
        {
            SDL_DestroyTexture(ShipsData[index].texture);
            ShipsData[index].texture = NULL;
        }
        uint32_t *pixels = Uint8PixelsToUint32Pixels(data->pixels, data->width, data->height);
        SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(pixels, data->width, data->height,
                                                     sizeof(uint32_t) * 8, sizeof(uint32_t) * data->width,
                                                     R_MASK, G_MASK, B_MASK, A_MASK);
        if (surf)
        {
            SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0x00, 0x00, 0x00));
            ShipsData[index].texture = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_SetTextureBlendMode(ShipsData[index].texture, SDL_BLENDMODE_BLEND);
        }
        else
        {
            // TODO: Log something about it somewhere
        }
        SDL_FreeSurface(surf);
        free(pixels);
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
        case TURN_AROUND: // Intentional fallthrough to double turn
        {
            TurnLeft(ship);
            TurnLeft(ship);
            break;
        }
        case TURN_LEFT:
        {
            TurnLeft(ship);
            break;
        }
        case TURN_RIGHT:
        {
            TurnRight(ship);
            break;
        }
        case NO_ACTION:
        {
            printf("Ship %p returned an invalid action result.\n", data);
            break;
        }
        default:
            break;
    }
}

void CleanupShip(void *data)
{
    NULL_CHECK(data);
    Ship *ship = (Ship*) data;
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
Ship *CreateNoneShip(uint8_t x, uint8_t y, Facing facing)
{
    return CreateGenericShip(x, y, facing);
}

Action ActivateNoneShip(Ship *ship)
{
    if (ship->type == NONE_SHIP)
        SDL_Log("NoneShip %p was activated\n", (void*) ship);
    return NO_ACTION;
}

// TODO: Investigate the counter, something fishy is going on
void FreeShip(Ship *ship)
{
    NULL_CHECK(ship);
    if (ship->data)
        fprintf(stderr, "Non-null pointer %p leaked\n", ship->data); // TODO: Logger
    DestroyShip(ship);
}

void DrawBlankShip(Ship *ship)
{
    SDL_Rect rect = GetTile(ship->x, ship->y);
    SDL_SetRenderDrawColor(GameRenderer, ship->color.r,
                           ship->color.g, ship->color.b, ship->color.a);
    SDL_RenderFillRect(GameRenderer, &rect);
    DrawArrow(ship->x, ship->y, ship->facing);
}

/** Circle Ship **/
Ship *CreateCircleShip(uint8_t x, uint8_t y, Facing facing)
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

Action ActivateCircleShip(Ship *ship)
{
    Action value = TURN_RIGHT;
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
Ship *CreateBullet(uint8_t x, uint8_t y, Facing facing)
{
    Ship *bullet = CreateGenericShip(x, y, facing);
    if (bullet)
        bullet->type = BULLET;
    return bullet;
}

Action ActivateBullet(Ship *ship)
{
    if (!ship->counter)
        ship->counter = 1;
    return NO_GENERIC_ACTION;
}

void DrawBullet(Ship *ship)
{
    if (ship->counter)
    {
        DrawShipType(ship);
    }
}

//** Internal Drawing Methods **//

static void DrawShipType(Ship *ship)
{
    SDL_Rect rect = GetTile(ship->x, ship->y);
    SDL_Texture *texture = ShipsData[ship->type].texture;
    if (!texture)
    {
        if (ShipsData[ship->type].imageData)
            LoadShipimage(GameRenderer, ship->type);
        DrawBlankShip(ship);
        return;
    }
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    Facing facing = ship->facing;
    double angle = 0;
    if (facing == LEFT)
        flip = SDL_FLIP_HORIZONTAL;
    // Angles are based on screen relative angles
    if (facing == UP)
        angle = 270;
    if (facing == DOWN)
        angle = 90;
    SDL_SetTextureColorMod(texture, ship->color.r, ship->color.g, ship->color.b);
    SDL_RenderCopyEx(GameRenderer, texture, NULL, &rect, angle, NULL, flip);
}
