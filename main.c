#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include "array.h"
#include "bullet.h"
#include "button.h"
#include "constants.h"
#include "draw.h"
#include "flags.h"
#include "player.h"
#include "ship.h"
#include "ship_types.h"
#include "super_header.h"

#define WIDTH 10
#define HEIGHT 10

#define RECT_X 50
#define RECT_Y 50

#define SPACING 5

static SDL_Renderer *renderer;
static SDL_Window *window;
static Field field = {WIDTH, HEIGHT, RECT_X, RECT_Y, 0, 0, SPACING};

// TODO: THIS IS HORRIFIC PLEASE FIX IT WHEN YOU GET TO MENUS AND STUFF
static struct
{
    const Uint16 turnDelay;
} userSettings = {250}; // 250 is what it was before, that felt a tad slow

typedef enum {PLAYER, AI, MISC} Turn;
unsigned int WindowSizeX();
unsigned int WindowSizeY();


static SDL_atomic_t frames;

#ifndef RELEASE
Uint32 fps_timer_callback(Uint32 interval, void *data);
#endif // RELEASE

int init();

int main(int argc, char **argv)
{
    int loop = 1;
    printf(">>");
    for (int i = 0; i < argc; i++)
        printf("%s ", argv[i]);
    printf("\n");

    if (init())
    {
        printf("SDL failed to initialize: %s\n", SDL_GetError());
        return -1;
    }
#ifndef UNLIMITED_FPS
    Uint32 time;
#endif // UNLIMITED_FPS
    Array* ships = ArrayNew();
    Ship *player = CreatePlayer();
    player->type = USER;
    Ship *s; // Arbitrary temp ship
    Button *button = ButtonCreate((SDL_Rect) {400, 400, 50, 50}, VoidButton);

    printf("player\n");
    ArrayAppend(ships, CreateCircleShip(5, 6, LEFT));
    ArrayAppend(ships, CreateCircleShip(4, 3, RIGHT));

    // This is bad
    Bullet *zoop = CreateGenericShip(9, 5, RIGHT);
    ColorShip((Ship*) zoop, SDL_MapRGB(GetPixelFormat(), 0xFF, 0xFF, 0x00));

    ColorShip(player, SDL_MapRGB(GetPixelFormat(), 0xFF, 0x00, 0x00));
    SDL_Event e;

    Turn turn = PLAYER;
    Action selection = NONE;
    Uint32 turnTimer = 0;
    unsigned int turnIndex = 0;

    struct
    {
        Uint8 switchTurn : 1; // 5 unused
        Uint8 windowSize : 1;
        Uint8 bufferState : 1;
    } flags;
    flags.windowSize = 1;

    #ifndef RELEASE
    SDL_AddTimer(2000, fps_timer_callback, NULL);
    #endif // RELEASE

    LoadShipImages(); // HACKY
    SDL_Texture *t = Gamer();

    SDL_Vertex lists[4] = {{{50, 40}, {0xFF, 0x00, 0x00, 0xFF}, {0, 0}},
                            {{120, 50}, {0x00, 0xFF, 0x00, 0xFF}, {1, 0}},
                            {{50, 150}, {0xFF, 0x00, 0x00, 0xFF}, {0, 1}},
                            {{200, 150}, {0x00, 0x00, 0xFF, 0xFF}, {1, 1}}};
    while (loop)
    {
#ifndef UNLIMITED_FPS
        time = SDL_GetTicks();
#endif // UNLIMITED_FPS
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                loop = 0;
            }
            // TODO: Fix this, it's sloppy as fuck
            if (e.type == SDL_MOUSEBUTTONDOWN && turn == PLAYER && selection != NONE && ButtonCheck(button, &e))
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
                    case SDL_SCANCODE_LEFT:
                        selection = TURNLEFT;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        selection = TURNRIGHT;
                        break;
                    case SDL_SCANCODE_SPACE:
                        flags.switchTurn = (turn == PLAYER);
                        break;
                    case SDL_SCANCODE_UP:
                        field.spacing++;
                        flags.windowSize = 1;
                        break;
                    case SDL_SCANCODE_DOWN:
                        field.spacing--;
                        flags.windowSize = 1;
                        break;
                    default:
                        break;
                }
            }
        }
        // Flag handling
        if (flags.windowSize)
        {
            if (!field.spacing)
                field.spacing = 1;
            flags.windowSize = 0;
            SDL_SetWindowSize(window, WindowSizeX(), WindowSizeY());
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
            selection = NONE;

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

        // AI turn handling
        if (turn == AI)
        {
            if (!flags.bufferState && SDL_GetTicks() - turnTimer > userSettings.turnDelay)
            {
                if (turnIndex < ArrayLength(ships))
                {
                    s = ArrayElement(ships, turnIndex);
                    MoveShip(s);
                    ActivateShip(s); // Wowie it's ai timer
                    // TODO: Maybe verify they're not in the same tile as something else
                    turnIndex++;
                    turnTimer = SDL_GetTicks();
                }
                else
                {
                    flags.bufferState = 1;
                    turn = PLAYER;
                }
            }
        }

        // Drawing
        if (turn == PLAYER)
        {
            for (unsigned int i = 0; i < ArrayLength(ships); i++)
            {
                if ((s = (Ship*) ArrayElement(ships, i)))
                {
                    SDL_Point point = ShipNextTile(s);
                    OutlineTile(point.x, point.y);
                }
            }
        }
        DrawField(&field);
        DrawBlankShip(player);
        ArrayIterate(ships, DrawShip);
        DrawBullet(zoop);
        DrawButton(button);

        SDL_Rect rr = {300, 0, 200, 200};
        int rs[] = {0, 1, 2, 2, 1, 3};
        SDL_RenderCopy(renderer, t, NULL, &rr);
        SDL_RenderGeometry(renderer, t, lists, 4, rs, 6);

        // End of frame stuff
        SDL_RenderPresent(renderer);
        SDL_AtomicAdd(&frames, 1);
#ifndef UNLIMITED_FPS
        if (SDL_GetTicks() - time <= FPS_LIMIT_THRESHOLD)
            SDL_Delay(FPS_LIMIT_RATE);
#endif // UNLIMITED_FPS
    }
    FreeShipImages();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    ArrayAnnihilate(&ships, CleanupShip);
    return 0;
}

int init()
{
    int result = SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_TIMER);
    if (!result)
    {
        window = SDL_CreateWindow("Fast Space Thing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  WindowSizeX(), WindowSizeY(), WINDOW_FLAGS);
        renderer = SDL_CreateRenderer(window, -1, RENDERER_FLAGS);
        result = !window && !renderer;
        SDL_Surface *image = SDL_LoadBMP("nerd.bmp");
        if (image)
            SDL_SetWindowIcon(window, image);
        SDL_FreeSurface(image);
    }
    return result;
}

SDL_PixelFormat* GetPixelFormat()
{
    return SDL_GetWindowSurface(window)->format;
}

SDL_Renderer* GetRenderer()
{
    return renderer;
}

SDL_Window* GetWindow()
{
    return window;
}

Field* GetField()
{
    return &field;
}

unsigned int WindowSizeX()
{
    return field.width * (field.rectWidth + field.spacing) - field.spacing + 200;
}

unsigned int WindowSizeY()
{
    return field.height * (field.rectHeight + field.spacing) - field.spacing;
}

#ifndef RELEASE
/* From https://wiki.libsdl.org/SDL_atomic_t */
/* Calculate and display the average framerate over the set interval */
Uint32 fps_timer_callback(Uint32 interval, void *data)
{
    UNUSED(data);
    const float f = SDL_AtomicGet(&frames);
    const float iv = interval * 0.001f;

    /* Note: the thread safety of printf is ambiguous across platforms */
    printf("%.2f\tfps\n", f / iv);

    /* Reset frame counter */
    SDL_AtomicSet(&frames, 0);

    return interval;
}
#endif // RELEASE
