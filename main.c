#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include "array.h"
#include "button.h"
#include "constants.h"
#include "debug_display.h"
#include "draw.h"
#include "font.h"
#include "misc.h"
#include "names.h"
#include "player.h"
#include "setup.h"
#include "ship.h"
#include "ship_types.h"
#include "super_header.h"
// TODO: put these in their proper places


// TODO: Find an appropriate place for these
#define WIDTH 10
#define HEIGHT 10

#define RECT_X 50
#define RECT_Y 50

#define SPACING 5

SDL_Renderer *GameRenderer;
SDL_Window *GameWindow;
Field GameField = {WIDTH, HEIGHT, RECT_X, RECT_Y, SPACING, SPACING, SPACING};

// TODO: THIS IS HORRIFIC PLEASE FIX IT WHEN YOU GET TO MENUS AND STUFF
static struct
{
    const Uint16 turnDelay;
} userSettings = {250}; // 250 is what it was before, that felt a tad slow

static Array *badBullets;
static Array *collisionHolder;
static Array *goodBullets;
// TODO: Have array of all the different arrays that hold ships so it can be easily tracked

int main(int argc, char **argv)
{
    int loop = 1;
    InitDebugDisplay(argc, argv);

    if (InitializeLibraries())
    {
        printf("SDL failed to initialize: %s\n", SDL_GetError());
        return -1;
    }
    SDL_version version;
    SDL_VERSION(&version);
    printf("SDL VERSION: %i %i %i\n", version.major, version.minor, version.patch);

    Array* ships = ArrayNew();
    Ship *player = CreatePlayer(0, 0, RIGHT);

    Button *button = ButtonCreate((SDL_Rect) {400, 400, 50, 50}, VoidButton);

    collisionHolder = ArrayNew();
    badBullets = ArrayNew();
    goodBullets = ArrayNew();

    printf("player\n");
    ArrayAppend(ships, CreateCircle(5, 6, LEFT));
    ArrayAppend(ships, CreateCircle(4, 3, RIGHT));

    ColorShip(player, SDL_MapRGB(DisplayPixelFormat, 0xFF, 0x00, 0x00));
    SDL_Event e;

    Turn turn = PLAYER;
    Action selection = NO_ACTION;
    uint32_t turnTimer = 0;

    // TODO: Move this externally
    struct
    {
        uint8_t switchTurn : 1; // 5 unused
        uint8_t windowSize : 1;
        uint8_t bufferState : 1;
        uint8_t doCollision : 1;
        uint8_t goob : 1;
    } flags;
    flags.windowSize = 1;
    flags.switchTurn = 0;
    flags.bufferState = 0;
    flags.doCollision = 0;

    EnableDebugDisplay(SHOW_FPS, TOP_RIGHT, NULL);
    EnableDebugDisplay(SHOW_TURN, TOP_RIGHT, &turn);

    LoadShipImages(); // HACKY

    void *selected_ship = NULL; // Currently selected ship
    SDL_Texture *selected_texture = NULL;
    SDL_Rect selected_rect;

    // Throwaway variables
    SDL_Rect rect;
    Ship *s = NULL;
    void **dp = NULL; // Dummy double pointer(no immature jokes)
    printf("%p\n", &goodBullets);
    while (loop)
    {
        const uint32_t frameStartTick = SDL_GetTicks();

        SDL_SetRenderDrawColor(GameRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(GameRenderer);

        unsigned int turnAdvance = (turn == PLAYER) && (selection != NO_ACTION);

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                loop = 0;
                break;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) // All mouse events
            {
                // This is a hack
                if (turnAdvance && ButtonCheck(button, &e))
                    flags.switchTurn = 1;
                else if (e.button.button == SDL_BUTTON_LEFT)
                {
                    SDL_Point mouse_pos = (SDL_Point) {e.button.x, e.button.y};
                    selected_ship = NULL;
                    for (size_t i = 0; i < ArrayLength(ships); i++)
                    {
                        s = (Ship*) ArrayElement(ships, i);
                        if (s) {
                            rect = GetTile(s->x, s->y);
                            if (SDL_PointInRect(&mouse_pos, &rect))
                            {
                                selected_ship = s;
                                DESTROY_SDL_TEXTURE(selected_texture);
                            }
                        }
                    }
                }
            }
            if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.scancode)
                {
                    case SDL_SCANCODE_Q:
                        loop = 0;
                        break;
                    case SDL_SCANCODE_G:
                    {
                        Ship *bullet = CreateGenericShip(1, 6, RIGHT);
                        bullet->type = BULLET;
                        ColorShip(bullet, SDL_MapRGB(DisplayPixelFormat, 0x00, 0x80, 0xFF));
                        ArrayAppend(goodBullets, bullet);
                        flags.goob = ~flags.goob;
                        break;
                    }
                    case SDL_SCANCODE_A:
                    case SDL_SCANCODE_LEFT:
                        selection = TURN_LEFT;
                        break;
                    case SDL_SCANCODE_D:
                    case SDL_SCANCODE_RIGHT:
                        selection = TURN_RIGHT;
                        break;
                    case SDL_SCANCODE_SPACE:
                        selection = SHOOT;
                        break;
                    case SDL_SCANCODE_RETURN:
                    case SDL_SCANCODE_RETURN2:
                        flags.switchTurn = turnAdvance;
                        break;
                    case SDL_SCANCODE_O:
                        GameField.spacing += 2;
                        flags.windowSize = 1;
                        break;
                    case SDL_SCANCODE_L:
                        GameField.spacing -= 2;
                        flags.windowSize = 1;
                        break;
                    default:
                        break;
                }
            }
        }
        // Flag handling
        {
            if (flags.windowSize)
            {
                if (!GameField.spacing)
                    GameField.spacing = 1;
                flags.windowSize = 0;
                SDL_SetWindowSize(GameWindow, WindowSizeX(), WindowSizeY());
                button->rect.x = WindowSizeX() - 100 - button->rect.w / 2;
                button->rect.y = WindowSizeY() - 100 - button->rect.h / 2;
            }
            if (flags.switchTurn)
            {
                MoveShip(player);
                if (selection == TURN_RIGHT)
                    TurnRight(player);
                else if (selection == TURN_LEFT)
                    TurnLeft(player);
                else if (selection == SHOOT)
                {
                    Ship *bullet = CreateBullet(1, 6, RIGHT);
                    if (bullet)
                        ArrayAppend(goodBullets, bullet);
                }
                ArrayIterate(goodBullets, ActivateShip);
                selection = NO_ACTION;

                // Switch turn
                turn = AI_BUFFER;
                turnTimer = SDL_GetTicks();
                flags.switchTurn = 0;
                flags.doCollision = 1;
            }
        }
        if (frameStartTick - turnTimer > userSettings.turnDelay)
        {
            turnTimer = 0;
            if (turn == AI_BUFFER)
                turn = AI;
            if (turn == PLAYER_BUFFER)
                turn = PLAYER;
            if (turn == MISC_BUFFER)
                turn = MISC;
        }
        // AI turn handling
        if (turn == AI)
        {
            // Move all units
            // Build a table of their positions, killing duplicates

            turn = PLAYER_BUFFER;
            turnTimer = frameStartTick;

            ArrayIterate(ships, ActivateShip);      // Activate enemies
            ArrayIterate(badBullets, ActivateShip); // Activate bullets

            ArrayClear(collisionHolder);
            ArrayReserve(collisionHolder, NumTiles());
            // TODO: Make this not bad
            // There is heap corruption afoot
            for (unsigned int i = 0; i < ArrayLength(badBullets); i++)
            {
                s = (Ship*) ArrayElement(badBullets, i);
                if (!s)
                    continue;
                size_t location = (size_t) IndexFromLocation(s->x, s->y);
                Ship *s2 = ArrayElement(collisionHolder, location);
                if (s2)
                {
                    printf("There was a collision at %u %u\n", s->x, s->y);
                }
                else
                {
                    if ((dp = ArrayReference(collisionHolder, location)))
                        *dp = (void*) s;
                }
            }
            for (unsigned int y = 0; y < GameField.height; y++)
            {
                for (unsigned int x = 0; x < GameField.width; x++)
                {
                    printf("%8p ", ArrayElement(collisionHolder, IndexFromLocation(x, y)));
                }
                printf("\n");
            }
            for (unsigned int i = 0; i < ArrayLength(badBullets); i++)
            {
                s = (Ship*) ArrayElement(badBullets, i);
                int collision = 0;
                if (!s)
                    break;
                if (s->x > GameField.width || s->y > GameField.height)
                {
                    printf("%p is out of bounds\n", (void*) s);
                    CleanupShip(*ArrayRemove(badBullets, i));
                    i--;
                    continue;
                }
                for (unsigned int j = i + 1; j < ArrayLength(badBullets); j++)
                {
                    Ship *s2 = (Ship*) ArrayElement(badBullets, j);
                    if (s != s2)
                    {
                        SDL_Point p = {s->x, s->y};
                        SDL_Point p2 = {s2->x, s2->y};
                        if (p.x == p2.x && p.y == p2.y)
                        {
                            collision = 1;
                            CleanupShip(*ArrayRemove(badBullets, j--));
                        }
                    }
                }
                if (collision)
                {
                    CleanupShip(*ArrayRemove(badBullets, i--));
                }
            }
        }

        // Drawing
        DrawField(&GameField);
        if (turn == PLAYER)
        {
            // Outline each ships next position
            for (unsigned int i = 0; i < ArrayLength(ships); i++)
            {
                if ((s = (Ship*) ArrayElement(ships, i)))
                {
                    SDL_Point point = ShipNextTile(s);
                    OutlineTile(point.x, point.y);
                }
            }
            if ((s = ArrayFind(ships, selected_ship)))
            {
                // TODO: Also display information about the ship
                SDL_SetRenderDrawColor(GameRenderer, 0xFF, 0xFF, 0x00, 0xFF);
                OutlineTileBufferColor(s->x, s->y);
                if (!selected_texture)
                {
                    char buffer[100];
                    sprintf(buffer, "%s\nAction: %s", GetNameShip(s), HumanReadableStringFrom(s->previous));
                    selected_texture = FontRenderTextSize(GameRenderer, buffer, 15, &selected_rect);
                    selected_rect.x = WindowSizeX() - selected_rect.w;
                    selected_rect.y = 200;
                    SDL_SetTextureColorMod(selected_texture, 0xFF, 0x00, 0x00);
                }
                SDL_RenderCopy(GameRenderer, selected_texture, NULL, &selected_rect);
            }
        }

        // TODO: Clean up misc stuff <- I don't know what this is referring to
        DrawShip(player);
        OutlineTile(player->x, player->y);
        if (flags.goob)
            OutlineTile(3, 1);
        ArrayIterate(ships, DrawShip);
        ArrayIterate(badBullets, DrawShip);
        //ArrayIterate(goodBullets, DrawShip);
        DrawButton(button);

        DebugDisplayDraw();

        // End of frame stuff
        SDL_RenderPresent(GameRenderer);
#ifndef UNLIMITED_FPS
        if (SDL_GetTicks() - frameStartTick <= FPS_LIMIT_THRESHOLD)
            SDL_Delay(FPS_LIMIT_RATE);
#endif // UNLIMITED_FPS
    }
    printf("Cleanup has started.\n");
    FreeShipImages();
    CleanupLibraries();
    ArrayAnnihilate(&ships, CleanupShip);
    ArrayAnnihilate(&badBullets, CleanupShip);
    ArrayAnnihilate(&goodBullets, CleanupShip);
    DestroyShip(player);
    return VerifyShipsFreed();
}

/** ANYTHING BELOW THIS LINE IS TEMPORARY AND SHOULD NOT REMAIN HERE **/

// Bullet hackiness is here
void ShootGamer(Ship *ship)
{
    Ship *bullet = CreateBullet(ship->x, ship->y, ship->facing);
    NULL_CHECK(bullet);
    // I know this is a temp function, but you really need to always check your damn pointers
    ColorShip(bullet, SDL_MapRGB(DisplayPixelFormat, 0x00, 0x80, 0xFF));
    ArrayAppend(badBullets, bullet);
}
