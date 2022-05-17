#include "setup.h"
#include "flags.h"
#include "font.h"
#include "super_header.h"

int InitializeLibraries()
{
    int result = SDL_Init(SDL_INIT_FLAGS);
    if (!result)
    {
        GameWindow = SDL_CreateWindow("Fast Space Thing", SDL_WINDOW_POS, SDL_WINDOW_POS,
                                  WindowSizeX(), WindowSizeY(), SDL_WINDOW_FLAGS);
        GameRenderer = SDL_CreateRenderer(GameWindow, -1, RENDERER_FLAGS);
        result = !GameWindow && !GameRenderer && !FontInit();
        SDL_Surface *image = NULL; // TODO: Put create a proper logo
        if (image)
            SDL_SetWindowIcon(GameWindow, image);
        SDL_FreeSurface(image);
        FontInit();
    }
    return result;
}

void CleanupLibraries()
{
    FontQuit();
    SDL_DestroyRenderer(GameRenderer);
    SDL_DestroyWindow(GameWindow);
    SDL_Quit();
}
