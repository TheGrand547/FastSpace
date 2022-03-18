#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include "bullet.h"
#include "button.h"
#include "constants.h"
#include "ship.h"
#include "super_header.h"
#include "array.h"

#define WIDTH 10
#define HEIGHT 10

#define RECT_X 50
#define RECT_Y 50

#define SPACING 5

// Gamin
#define WINDOW_SIZE_X WIDTH * (RECT_X + SPACING) - SPACING + 200
#define WINDOW_SIZE_Y HEIGHT * (RECT_Y + SPACING) - SPACING

static SDL_Renderer *renderer;
static SDL_Window *window;
static Field field = {WIDTH, HEIGHT, RECT_X, RECT_Y, 0, 0, SPACING};

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
    Uint32 time;
    Array* ships = ArrayNew();
    Ship *player = CreateShip(0, 0, RIGHT);
    Button *button = ButtonCreate((SDL_Rect) {400, 400, 50, 50}, VoidButton);
    ArrayAppend(ships, player);

    printf("%p\n", (void*)player);
    printf("%p\n", (**(void***) ships));

    printf("player\n");
    ArrayAppend(ships, CreateShip(1, 3, LEFT));
    ArrayAppend(ships, CreateShip(2, 3, RIGHT));
    /*
    ArrayAppend(ships, CreateShip(3, 3, RIGHT));
    ArrayAppend(ships, CreateShip(4, 3, RIGHT));
    ArrayAppend(ships, CreateShip(5, 3, RIGHT));
    ArrayAppend(ships, CreateShip(6, 3, RIGHT));
    //free(ArrayElement(ships, 2));
    void** a = ArrayRemoveRange(ships, 3, 5);
    for (;*a;a++)
        free(*a);
    //ArrayRemove(ships, 1);*/

    // This is bad
    Bullet *zoop = CreateShip(9, 5, RIGHT);
    ColorShip((Ship*) zoop, SDL_MapRGB(GetPixelFormat(), 0xFF, 0xFF, 0x00));

    ColorShip(player, SDL_MapRGB(GetPixelFormat(), 0xFF, 0x00, 0x00));
    SDL_Event e;
    while (loop)
    {
        time = SDL_GetTicks();
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            {
                loop = 0;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && ButtonCheck(button, &e))
                ArrayIterate(ships, MoveShip);
            if (e.type == SDL_KEYDOWN)
            {
                // keyboard
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
                        ArrayIterate(ships, MoveShip);
                        //TurnRight(player);
                        //TurnLeft(player->next);
                        //player = VerifyShip(player);
                        break;
                    case SDL_SCANCODE_LEFT:
                        player->x--;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        player->x++;
                        break;
                    case SDL_SCANCODE_UP:
                        player->y--;
                        break;
                    case SDL_SCANCODE_DOWN:
                        player->y++;
                        break;
                    default:
                        break;
                }
            }
        }

        for (unsigned int i = 0; i < ArrayLength(ships); i++)
        {
            Ship *s = (Ship*) ArrayElement(ships, i);
            if (s)
            {
                OutlineTile(s->x + FacingX(s->facing), s->y + FacingY(s->facing));
            }
        }
        DrawField(&field);
        ArrayIterate(ships, DrawShip);
        DrawBullet(zoop);
        DrawButton(button);
        SDL_RenderPresent(renderer);
#if LIMITED_FPS == 1
        if (SDL_GetTicks() - time <= 2)
            SDL_Delay(1);
#endif // UNLIMITED_FPS
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    ArrayClean(ships);
    ArrayPurge(&ships);
    return 0;
}

int init()
{
    int result = SDL_Init(SDL_INIT_EVERYTHING);
    if (!result)
    {
        result = SDL_CreateWindowAndRenderer(WINDOW_SIZE_X, WINDOW_SIZE_Y,
                                              SDL_RENDERER_ACCELERATED, &window, &renderer);
        SDL_SetWindowTitle(window, "Fast Space Thing");
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
