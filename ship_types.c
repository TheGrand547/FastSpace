#include "ship_types.h"
#include <SDL2/SDL_log.h>
#include <stdio.h>
#include <stdlib.h>
#include "draw.h"
#include "misc.h"
#include "names.h"
#include "player.h"
#include "ship_data.h"

const char *HumanReadableStringFrom(Action action)
{
    switch (action)
    {
        case TURN_LEFT: return "Turning Left";
        case TURN_RIGHT: return "Turning Right";
        case TURN_AROUND: return "Turning Around";
        case SHOOT: return "Firing Weapons";
        case NO_ACTION: return "Developer Messed Up";
        case NO_GENERIC_ACTION: return "Coasting";
        case OVERRIDE: return "Gravitationally Stable";
        default: return "Developer Messed Up";
    }
}

// TODO: Make logger with freopen() on stderr

static void DrawShipImage(Ship *ship, SDL_Texture *texture);
static void DrawShipType(Ship *ship);

struct ShipData
{
    const ShipCreateFunc create;
    const ShipActionFunc action;
    const ShipFreeFunc free;
    const ShipDrawFunc draw;
    const ShipDataFunc imageData;
    SDL_Texture *texture;
    const char *name;
};

// TODO: Move the image data here
static struct ShipData ShipsData[LAST_SHIP] = {
    {
        CreateNone,
        ActivateNone,
        FreeShip,
        DrawShipType,
        NoneImageData,
        NULL,
        "Wreckage"
    }, // None ship
    {
        CreateCircle,
        ActivateCircle,
        FreeCircle,
        DrawShipType,
        CircleImageData,
        NULL,
        "Patroller"
    }, // Circle ship
    {
        CreatePlayer,
        ActivatePlayer,
        FreePlayer,
        DrawShipType,
        PlayerImageData,
        NULL,
        "You, goober"
    }, // Player ship
    {
        CreateBullet,
        ActivateBullet,
        FreeBullet,
        DrawShipType,
        BulletImageData,
        NULL,
        "Bullet"
    },  // Generic Bullet
    {
        CreateNone,
        ActivateNone,
        FreeShip,
        DrawShipType,
        NoneImageData,
        NULL,
        "Dizzyman"
    }, // Going to be the looper
    {
        CreateExplosion,
        ActivateExplosion,
        FreeExplosion,
        DrawExplosion,
        NoneImageData,
        NULL,
        NULL,
    }
};

#define NUM_SHIP_TYPES STATIC_ARRAY_LENGTH(ShipsData)

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
char *GetNameShip(void *data)
{
    NULL_CHECK_RETURN(data, NULL);
    Ship *ship = (Ship*) data;
    if (ship->name)
        return ship->name;
    ship->name = GetName(ShipsData[ship->type].name);
    return ship->name;
}

void ActivateShip(void *data)
{
    NULL_CHECK(data);
    Ship *ship = (Ship*) data;
    Action action = ShipsData[ship->type].action(ship);
    ship->previous = action;
    if (action == OVERRIDE)
        return;
    MoveShip(ship);
    switch (action)
    {
        case SHOOT:
        {
            ShootGamer(ship); // TODO: fix this hack thingy
            break;
        }
        case TURN_AROUND:
        {
            TurnAround(ship);
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
            fprintf(stderr, "Ship %p returned an invalid action result.\n", data);
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
    ShipsData[ship->type].free(ship);
}

void DrawShip(void *data)
{
    NULL_CHECK(data);
    Ship *ship = (Ship*) data;
    ShipsData[ship->type].draw(ship);
}

//** Ship Function Map Implementations **//

/** Generic ship **/
Ship *CreateNone(uint8_t x, uint8_t y, Facing facing)
{
    return CreateGenericShip(x, y, facing);
}

Action ActivateNone(Ship *ship)
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
    DestroyShip(ship);
}

void DrawBlankShip(Ship *ship)
{
    SDL_Rect rect = GetTile(ship->x, ship->y);
    SetRenderDrawColor(GameRenderer, ship->color);
    SDL_RenderFillRect(GameRenderer, &rect);
    DrawArrow(ship->x, ship->y, ship->facing);
}

/** Circle Ship **/
Ship *CreateCircle(uint8_t x, uint8_t y, Facing facing)
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

Action ActivateCircle(Ship *ship)
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
    {
        bullet->counter = 0;
        bullet->type = BULLET;
        bullet->toughness = 14;
    }
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

#define EXPLOSION_NUM 8
#define TRIANGLE 3
#define EXPLOSION_VERTICIES EXPLOSION_NUM * TRIANGLE
#define EXPLOSION_TEXTURE_SIZE 100

/** Explosion **/
Ship *CreateExplosion(uint8_t x, uint8_t y, Facing facing)
{
    Ship *ship = CreateGenericShip(x, y, facing);
    if (ship)
{
        ship->type = EXPLOSION;
        ship->counter = 2;
        ship->color = (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF};
    }
    return ship;
}

Action ActivateExplosion(Ship *ship)
{
    UNUSED(ship);
    return OVERRIDE;
}

void FreeExplosion(Ship *ship)
{
    DESTROY_SDL_TEXTURE(ship->data);
    FreeShip(ship);
}

void DrawExplosion(Ship *ship)
{
    if (ship->data)
        DrawShipImage(ship, (SDL_Texture*) ship->data);
    else
    {
        // 100 x 100 should provide 'good enough' resolution
        SDL_Texture *texture = SDL_CreateTexture(GameRenderer, SDL_PIXELFORMAT_RGBA32,
                                                 SDL_TEXTUREACCESS_TARGET,
                                                 EXPLOSION_TEXTURE_SIZE, EXPLOSION_TEXTURE_SIZE);
        if (texture)
        {
            SDL_Vertex verts[EXPLOSION_VERTICIES];
            SDL_Color colors[3] = {
                {0xFF, 0x00, 0x00, 0x80}, // Red
                {0xFF, 0xFF, 0x00, 0x30}, // Yellow
                {0xFF, 0x7F, 0x00, 0x60} // Orange
            };
            SDL_FPoint empty = {0.0f, 0.0f};
            for (unsigned int i = 0; i < EXPLOSION_VERTICIES; i++)
            {
                verts[i].position = (SDL_FPoint) {rand() % EXPLOSION_TEXTURE_SIZE,
                                                  rand() % EXPLOSION_TEXTURE_SIZE};
                verts[i].color = colors[rand() % 3];
                verts[i].tex_coord = empty;
            }
            SDL_SetRenderTarget(GameRenderer, texture);
            SDL_RenderGeometry(GameRenderer, NULL, verts, EXPLOSION_VERTICIES, NULL, 0);
            SDL_SetRenderTarget(GameRenderer, NULL);

            ship->data = (void*) texture;
        }
    }
}

//** Internal Drawing Methods **//

static void DrawShipImage(Ship *ship, SDL_Texture *texture)
{
    NULL_CHECK(texture);
    SDL_Rect rect = GetTile(ship->x, ship->y);
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    Facing facing = ship->facing;
    double angle = 0;
    if (facing == LEFT)
        flip = SDL_FLIP_HORIZONTAL;
    // Angles are screen relative
    if (facing == UP)
        angle = 270;
    if (facing == DOWN)
        angle = 90;
    SetTextureColorMod(texture, ship->color);
    SDL_RenderCopyEx(GameRenderer, texture, NULL, &rect, angle, NULL, flip);
    DrawNumbers(ship);
}

static void DrawShipType(Ship *ship)
{
    SDL_Texture *texture = ShipsData[ship->type].texture;
    if (!texture)
    {
        if (ShipsData[ship->type].imageData)
            LoadShipimage(GameRenderer, ship->type);
        DrawBlankShip(ship);
        return;
    }
    DrawShipImage(ship, texture);
}
