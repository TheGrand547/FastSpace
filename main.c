#include <math.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include "array.h"
#include "button.h"
#include "constants.h"
#include "draw.h"
#include "font.h"
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

typedef enum {PLAYER, AI, MISC} Turn;
static const char *turnNames[] = {STR(PLAYER), STR(AI), STR(MISC)};

static Array *bullets;

static SDL_atomic_t frames;

#ifndef RELEASE
Uint32 fps_timer_callback(Uint32 interval, void *data);
#endif // RELEASE

int main(int argc, char **argv)
{
    int loop = 1;
    printf(">>");
    for (int i = 0; i < argc; i++)
        printf("%s ", argv[i]);
    printf("\n");

    if (InitializeLibraries())
    {
        printf("SDL failed to initialize: %s\n", SDL_GetError());
        return -1;
    }
    SDL_version version;
    SDL_VERSION(&version);
    printf("SDL VERSION: %i %i %i\n", version.major, version.minor, version.patch);
#ifndef UNLIMITED_FPS
    Uint32 time;
#endif // UNLIMITED_FPS
    Array* ships = ArrayNew();
    Ship *player = CreatePlayer(0, 0, RIGHT);
    player->type = USER;
    Ship *s; // Arbitrary temp ship
    Button *button = ButtonCreate((SDL_Rect) {400, 400, 50, 50}, VoidButton);
    bullets = ArrayNew();

    CONST_STR(PLAYER);

    printf("player\n");
    ArrayAppend(ships, CreateCircleShip(5, 6, LEFT));
    ArrayAppend(ships, CreateCircleShip(4, 3, RIGHT));

    ColorShip(player, SDL_MapRGB(DisplayPixelFormat, 0xFF, 0x00, 0x00));
    SDL_Event e;

    Turn turn = PLAYER;
    Action selection = NO_ACTION;
    Uint32 turnTimer = 0;
    unsigned int turnIndex = 0;

    struct
    {
        Uint8 switchTurn : 1; // 5 unused
        Uint8 windowSize : 1;
        Uint8 bufferState : 1;
    } flags;
    flags.windowSize = 1;

    // This should probably be wrapped up somewhere nicely
    float fps = 1;
    float oldfps = 0;
    char fpsText[13]; // FPS: 0000.00 -> 4 + 2 + 4 + 1 + 2 + 1 for null -> 13
    SDL_Texture *fpsTexture;
    SDL_Rect fpsRect;

#ifndef RELEASE
    SDL_AddTimer(20, fps_timer_callback, &fps);
#endif // RELEASE

    LoadShipImages(); // HACKY
    const char *message = "Question";
    printf("MESSAGE: %s\n", message);
    SDL_Rect sizer;
    SDL_Texture *t = FontRenderTextSize(GameRenderer, message, 20, &sizer);
    SDL_SetTextureColorMod(t, 0x00, 0xFF, 0x00);

    SDL_Vertex lists[4] = {{{30, 50}, {0xFF, 0x00, 0x00, 0xFF}, {0, 0}},
                            {{800, 50}, {0x00, 0x00, 0xFF, 0xFF}, {1, 0}},
                            {{50, 130}, {0xFF, 0x00, 0x00, 0xFF}, {0, 1}},
                            {{800, 100}, {0x00, 0x00, 0xFF, 0xFF}, {1, 1}}};
    while (loop)
    {
#ifndef UNLIMITED_FPS
        time = SDL_GetTicks();
#endif // UNLIMITED_FPS
        SDL_SetRenderDrawColor(GameRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(GameRenderer);
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                loop = 0;
            }
            // TODO: Fix this, it's sloppy as fuck
            if (e.type == SDL_MOUSEBUTTONDOWN && turn == PLAYER && selection != NO_ACTION && ButtonCheck(button, &e))
            {
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
                        ArrayAppend(bullets, bullet);
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
                        flags.switchTurn = (turn == PLAYER) && (selection != NO_ACTION);
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
                    s = ArrayElement(ships, turnIndex);
                    ActivateShip(s); // Wowie it's ai time
                    // TODO: Maybe verify they're not in the same tile as something else
                    turnIndex++;
                    turnTimer = SDL_GetTicks();
                }
                else
                {
                    ArrayIterate(bullets, ActivateShip);
                    for (unsigned int i = 0; i < ArrayLength(bullets); i++)
                    {
                        s = (Ship*) ArrayElement(bullets, i);
                        int collision = 0;
                        if (!s)
                            break;
                        if (s->x > GameField.width || s->y > GameField.height)
                        {
                            printf("%p is out of bounds\n", (void*) s);
                            CleanupShip(*ArrayRemove(bullets, i));
                            i--;
                            continue;
                        }
                        for (unsigned int j = i + 1; j < ArrayLength(bullets); j++)
                        {
                            Ship *s2 = (Ship*) ArrayElement(bullets, j);
                            if (s != s2)
                            {
                                SDL_Point p = {s->x, s->y};
                                SDL_Point p2 = {s2->x, s2->y};
                                if (p.x == p2.x && p.y == p2.y)
                                {
                                    collision = 1;
                                    CleanupShip(*ArrayRemove(bullets, j));
                                    j--;
                                }
                            }
                        }
                        if (collision)
                        {
                            CleanupShip(*ArrayRemove(bullets, i));
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
        ArrayIterate(bullets, DrawShip);
        DrawButton(button);

        int rs[] = {0, 1, 2, 2, 1, 3};
        SDL_RenderCopy(GameRenderer, t, NULL, &sizer);
        SDL_RenderGeometry(GameRenderer, t, lists, 4, rs, 6);

        if (FLOAT_EQUAL(fps, oldfps))
        {
            oldfps = fps;
            sprintf(fpsText, "FPS: %4.2f", oldfps);
            SDL_DestroyTexture(fpsTexture);
            fpsTexture = FontRenderTextSize(GameRenderer, fpsText, 15, &fpsRect);
            SDL_SetTextureColorMod(fpsTexture, 0xFF, 0x00, 0x00);
            fpsRect.x = WindowSizeX() - fpsRect.w;
            fpsRect.y = WindowSizeY() - fpsRect.h; // Bottom RIGHT
        }
        {
            SDL_Rect turnRect;
            SDL_Texture *turnText = FontRenderTextSize(GameRenderer, turnNames[turn], 15, &turnRect);
            turnRect.x = WindowSizeX() - turnRect.w;
            SDL_RenderCopy(GameRenderer, turnText, NULL, &turnRect);
            SDL_DestroyTexture(turnText);
        }
        SDL_RenderCopy(GameRenderer, fpsTexture, NULL, &fpsRect);

        // End of frame stuff
        SDL_RenderPresent(GameRenderer);
        SDL_AtomicAdd(&frames, 1);
#ifndef UNLIMITED_FPS
        if (SDL_GetTicks() - time <= FPS_LIMIT_THRESHOLD)
            SDL_Delay(FPS_LIMIT_RATE);
#endif // UNLIMITED_FPS
    }
    FreeShipImages();
    CleanupLibraries();
    ArrayAnnihilate(&ships, CleanupShip);
    ArrayAnnihilate(&bullets, CleanupShip);
    return 0;
}

/** ANYTHING BELOW THIS LINE IS TEMPORARY AND SHOULD NOT REMAIN HERE **/

void ShootGamer(Ship *ship)
{
    Ship *bullet = CreateBullet(ship->x, ship->y, ship->facing);
    ColorShip(bullet, SDL_MapRGB(DisplayPixelFormat, 0x00, 0x80, 0xFF));
    ArrayAppend(bullets, bullet);
}

#ifndef RELEASE
Uint32 fps_timer_callback(Uint32 interval, void *data)
{
    const float f = SDL_AtomicGet(&frames);
    const float iv = interval * 0.001f;

    *(float*) data = f / iv;

    /* Reset frame counter */
    SDL_AtomicSet(&frames, 0);
    return interval;
}
#endif // RELEASE
