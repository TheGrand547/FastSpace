#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include "bullet.h"
#include "button.h"
#include "constants.h"
#include "draw.h"
#include "ship.h"
#include "super_header.h"
#include "array.h"
#include "flags.h"

#define WIDTH 10
#define HEIGHT 10

#define RECT_X 50
#define RECT_Y 50

#define SPACING 5

static SDL_Renderer *renderer;
static SDL_Window *window;
static Field field = {WIDTH, HEIGHT, RECT_X, RECT_Y, 0, 0, SPACING};

typedef enum {PLAYER, AI} Turn;
unsigned int WindowSizeX();
unsigned int WindowSizeY();

SDL_atomic_t frames;

/* From https://wiki.libsdl.org/SDL_atomic_t */
/* Calculate and display the average framerate over the set interval */
Uint32 fps_timer_callback(Uint32 interval, void *data)
{
    const float f = SDL_AtomicGet(&frames);
    const float iv = interval * 0.001f;

    /* Note: the thread safety of printf is ambiguous across platforms */
    printf("%.2f\tfps w/0x%p data\n", f / iv, data);

    /* Reset frame counter */
    SDL_AtomicSet(&frames, 0);

    return interval;
}

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
#ifdef LIMITED_FPS
    Uint32 time;
#endif // LIMITED_FPS
    Array* ships = ArrayNew();
    Ship *player = CreateShip(0, 0, RIGHT);
    player->type = USER;
    Ship *s; // Arbitrary temp ship
    Button *button = ButtonCreate((SDL_Rect) {400, 400, 50, 50}, VoidButton);


    // I don't know why these are here
    printf("%zu\n", sizeof(Ship));
    printf("%p\n", (void*)player);
    printf("%p\n", (**(void***) ships));

    printf("player\n");
    ArrayAppend(ships, CreateShip(1, 3, LEFT));
    ArrayAppend(ships, CreateShip(4, 3, RIGHT));
    for (unsigned int i = 0; i < ArrayLength(ships); i++)
        ((Ship*)ArrayElement(ships, i))->type = CIRCLE;

    // This is bad
    Bullet *zoop = CreateShip(9, 5, RIGHT);
    ColorShip((Ship*) zoop, SDL_MapRGB(GetPixelFormat(), 0xFF, 0xFF, 0x00));

    ColorShip(player, SDL_MapRGB(GetPixelFormat(), 0xFF, 0x00, 0x00));
    SDL_Event e;

    Turn turn = PLAYER;
    Uint32 turnTimer = 0;
    unsigned int turnIndex = 0;

    struct
    {
        Uint8 switchTurn : 1; // 6 unused
        Uint8 windowSize : 1;
    } flags;

    SDL_AddTimer(2000, fps_timer_callback, NULL);
    while (loop)
    {
#ifdef LIMITED_FPS
        time = SDL_GetTicks();
#endif // LIMITED_FPS
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                loop = 0;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && turn == PLAYER && ButtonCheck(button, &e))
            {
                flags.switchTurn = 1;
            }
            if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.scancode)
                {
                    case SDL_SCANCODE_H:
                        ArrayPop(ships);
                        break;
                    case SDL_SCANCODE_Q:
                        loop = 0;
                        break;
                    case SDL_SCANCODE_F:
                        TurnRight(player);
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
            flags.switchTurn = 0;
            MoveShip(player);
            // TODO: get the players choice thingy
            turn = AI;
            turnTimer = SDL_GetTicks();
            turnIndex = 0;
        }
        // TODO: This should be placed elsewhere
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
        else
        {
            if (SDL_GetTicks() - turnTimer > 250)
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
                    turn = PLAYER;
                }
            }
        }
        DrawField(&field);
        DrawShip(player);
        ArrayIterate(ships, DrawShip);
        DrawBullet(zoop);
        DrawButton(button);
        SDL_RenderPresent(renderer);
        SDL_AtomicAdd(&frames, 1);
#ifdef LIMITED_FPS
        if (SDL_GetTicks() - time <= 2)
            SDL_Delay(1);
#endif // LIMITED_FPS
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    ArrayPurge(&ships);
    return 0;
}

int init()
{
    int result = SDL_Init(SDL_INIT_EVERYTHING);
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
