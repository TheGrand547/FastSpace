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
#include "player.h"
#include "setup.h"
#include "ship.h"
#include "ship_types.h"
#include "super_header.h"


// TODO: Find an appropriate place for these
#define WIDTH 10
#define HEIGHT 10

#define RECT_X 50
#define RECT_Y 50

#define SPACING 5

SDL_Renderer *GameRenderer;
SDL_Window *GameWindow;
Field GameField = {WIDTH, HEIGHT, RECT_X, RECT_Y, 0, 0, SPACING};

// TODO: THIS IS HORRIFIC PLEASE FIX IT WHEN YOU GET TO MENUS AND STUFF
static struct
{
    const Uint16 turnDelay;
} userSettings = {250}; // 250 is what it was before, that felt a tad slow

static Array *badBullets;
static Array *goodBullets;

int main(int argc, char **argv)
{
    UNUSED(argc);
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
#ifndef UNLIMITED_FPS
    uint32_t time;
#endif // UNLIMITED_FPS
    Array* ships = ArrayNew();
    Ship *player = CreatePlayer(0, 0, RIGHT);
    player->type = USER;
    Ship *s; // Arbitrary temp ship
    Button *button = ButtonCreate((SDL_Rect) {400, 400, 50, 50}, VoidButton);
    badBullets = ArrayNew();

    printf("player\n");
    ArrayAppend(ships, CreateCircleShip(5, 6, LEFT));
    ArrayAppend(ships, CreateCircleShip(4, 3, RIGHT));

    ColorShip(player, SDL_MapRGB(DisplayPixelFormat, 0xFF, 0x00, 0x00));
    SDL_Event e;

    Turn turn = PLAYER;
    Action selection = NO_ACTION;
    uint32_t turnTimer = 0;
    unsigned int turnIndex = 0;

    // TODO: Move this externally
    struct
    {
        uint8_t switchTurn : 1; // 5 unused
        uint8_t windowSize : 1;
        uint8_t bufferState : 1;
    } flags;
    flags.windowSize = 1;


    EnableDebugDisplay(SHOW_FPS, TOP_RIGHT, NULL);
    EnableDebugDisplay(SHOW_TURN, TOP_RIGHT, &turn);
    EnableDebugDisplay(SHOW_COUNTDOWN, TOP_RIGHT, NULL);

    LoadShipImages(); // HACKY


    const char *message = "twofewafewfweafewfweafewfwea\nlines";
    printf("MESSAGE: %s\n", message);
    SDL_Rect sizer;
    SDL_Texture *t = FontRenderTextWrappedSize(GameRenderer, message, 20, 300, &sizer);
    SDL_SetTextureColorMod(t, 0x00, 0xFF, 0x00);
    printf("%i %i\n", sizer.w, sizer.h);

    SDL_Vertex lists[4] = {
        {{600, 200}, {0xFF, 0xFF, 0xFF, 0xFF}, {0, 0}},
        {{800, 200}, {0xFF, 0xFF, 0xFF, 0xFF}, {1, 0}},
        {{600, 400}, {0xFF, 0xFF, 0xFF, 0xFF}, {0, 1}},
        {{800, 400}, {0xFF, 0xFF, 0xFF, 0xFF}, {1, 1}}
    };
    while (loop)
    {
#ifndef UNLIMITED_FPS
        time = SDL_GetTicks();
#endif // UNLIMITED_FPS
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
                if (turnAdvance && ButtonCheck(button, &e))
                    flags.switchTurn = 1;
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
                        ArrayAppend(badBullets, bullet);
                        break;
                    }
                    case SDL_SCANCODE_A:
                    case SDL_SCANCODE_LEFT:
                        selection = TURNLEFT;
                        break;
                    case SDL_SCANCODE_D:
                    case SDL_SCANCODE_RIGHT:
                        selection = TURNRIGHT;
                        break;
                    case SDL_SCANCODE_SPACE:
                        flags.switchTurn = turnAdvance;
                        break;
                    case SDL_SCANCODE_O:
                        GameField.spacing++;
                        flags.windowSize = 1;
                        break;
                    case SDL_SCANCODE_L:
                        GameField.spacing--;
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
                if (selection == TURNRIGHT)
                    TurnRight(player);
                else if (selection == TURNLEFT)
                    TurnLeft(player);
                /*
                else
                    // Shoot
                */
                selection = NO_ACTION;

                // Switch turn
                turn = AI;
                turnTimer = SDL_GetTicks();
                flags.bufferState = 1;
                flags.switchTurn = 0;
                turnIndex = 0;
            }
            if (flags.bufferState)
            {
                flags.bufferState = (SDL_GetTicks() - turnTimer) < userSettings.turnDelay;
                if (!flags.bufferState)
                    turnTimer = SDL_GetTicks();
            }
        }

        // AI turn handling
        if (turn == AI)
        {
            if (!flags.bufferState && SDL_GetTicks() - turnTimer > userSettings.turnDelay)
            {
                if (turnIndex < ArrayLength(ships))
                {
                    s = (Ship*) ArrayElement(ships, turnIndex);
                    ActivateShip(s); // Wowie it's ai time
                    // TODO: Ensure no invalid ships remain
                    turnIndex++;
                    turnTimer = SDL_GetTicks();
                }
                else
                {
                    ArrayIterate(badBullets, ActivateShip);
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
                                    CleanupShip(*ArrayRemove(badBullets, j));
                                    j--;
                                }
                            }
                        }
                        if (collision)
                        {
                            CleanupShip(*ArrayRemove(badBullets, i));
                            i--;
                        }
                    }
                    flags.bufferState = 1;
                    turn = PLAYER;
                }
            }
        }

        // Drawing
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
        }
        DrawField(&GameField);
        DrawShip(player);
        ArrayIterate(ships, DrawShip);
        ArrayIterate(badBullets, DrawShip);
        DrawButton(button);

        int rs[] = {0, 1, 2, 2, 1, 3};
        SDL_RenderCopy(GameRenderer, t, NULL, &sizer);
        SDL_RenderGeometry(GameRenderer, t, lists, 4, rs, 6);

        SDL_RenderDrawLine(GameRenderer, 300, 0, 300, 300);

        DebugDisplayDraw();

        // End of frame stuff
        SDL_RenderPresent(GameRenderer);
#ifndef UNLIMITED_FPS
        if (SDL_GetTicks() - time <= FPS_LIMIT_THRESHOLD)
            SDL_Delay(FPS_LIMIT_RATE);
#endif // UNLIMITED_FPS
    }
    FreeShipImages();
    CleanupLibraries();
    ArrayAnnihilate(&ships, CleanupShip);
    ArrayAnnihilate(&badBullets, CleanupShip);
    DestroyShip(player);
    return 0;
}

/** ANYTHING BELOW THIS LINE IS TEMPORARY AND SHOULD NOT REMAIN HERE **/

void ShootGamer(Ship *ship)
{
    Ship *bullet = CreateBullet(ship->x, ship->y, ship->facing);
    ColorShip(bullet, SDL_MapRGB(DisplayPixelFormat, 0x00, 0x80, 0xFF));
    ArrayAppend(badBullets, bullet);
}
