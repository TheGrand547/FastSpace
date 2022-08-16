#include "ship.h"
#include <stdio.h>
#include <stdlib.h>
#include "draw.h"
#include "font.h"
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

int InTileShip(uint8_t x, uint8_t y, Ship *ship)
{
    NULL_CHECK_RETURN(ship, 0);
    return (ship->x == x) && (ship->y == y);
}

int VerifyShipsFreed()
{
    return shipCount;
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

void TurnAround(Ship *ship)
{
    ship->facing -= 2;
}

void ColorShip(Ship *ship, uint32_t color)
{
    SDL_GetRGBA(color, DisplayPixelFormat, &ship->color.r,
                &ship->color.g, &ship->color.b, &ship->color.a);
}

void DrawNumbers(Ship *ship)
{
    SDL_Texture *texture;
    SDL_Rect rect;
    SDL_Rect tile = GetTile(ship->x, ship->y);
    char buffer[4];
    // TODO: This is sloppy and bad
    if (ship->toughness)
    {
        sprintf(buffer, "%X", ship->toughness);
        // TODO: make the number not be magic
        texture = FontRenderTextSize(GameRenderer, buffer, 10, &rect);
        if (texture)
        {
            rect.x = tile.x;
            rect.y = tile.y + tile.h - rect.h;
            SDL_SetTextureColorMod(texture, 0x00, 0xFF, 0x80);
            SDL_RenderCopy(GameRenderer, texture, NULL, &rect);
        }
        DESTROY_SDL_TEXTURE(texture);
    }
    if (ship->shields)
    {
        sprintf(buffer, "%X", ship->shields);
        // TODO: make the number not be magic
        texture = FontRenderTextSize(GameRenderer, buffer, 10, &rect);
        if (texture)
        {
            rect.x = tile.x + tile.w - rect.w;
            rect.y = tile.y + tile.h - rect.h;
            SDL_SetTextureColorMod(texture, 0x00, 0xFF, 0xFF);
            SDL_RenderCopy(GameRenderer, texture, NULL, &rect);
        }
        DESTROY_SDL_TEXTURE(texture);
    }
}

SDL_Point ShipNextTile(Ship *ship)
{
    return (SDL_Point) {ship->x + FacingX((Facing) ship->facing),
                        ship->y + FacingY((Facing) ship->facing)};

}

#define DEFAULT_TOUGHNESS 5
#define DEFAULT_SHIELDS   0
#define DEFAULT_COUNTER   0
#define DEFAULT_DUMMY     0
#define DEFAULT_COLLISION 0
#define DATA_NULL NULL
#define NAME_NULL NULL

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
    *ship = (Ship) {x, y, facing, NONE_SHIP, DEFAULT_COUNTER, DEFAULT_TOUGHNESS, DEFAULT_SHIELDS,
                    DEFAULT_DUMMY, DEFAULT_COLLISION, NO_ACTION,
                    (SDL_Color) {0xFF, 0x00, 0xFF, 0xFF}, DATA_NULL, NAME_NULL};
    return ship;
}
