#include "setup.h"
#include "flags.h"
#include "font.h"
#include "super_header.h"

int InitializeLibraries()
{
    int result = SDL_Init(SDL_INIT_FLAGS);
    if (!result)
    {
        SetupField();
        GameWindow = SDL_CreateWindow("Fast Space Thing", SDL_WINDOW_POS, SDL_WINDOW_POS,
                                  WindowSizeX(), WindowSizeY(), SDL_WINDOW_FLAGS);
        int render_num = -1;
        SDL_RendererInfo info;
        for (int i = 0; i < SDL_GetNumRenderDrivers(); i++)
        {
            SDL_GetRenderDriverInfo(i, &info);
            if ((info.flags & (RENDERER_FLAGS)) && strcmp(info.name, "direct3d"))
            {
                render_num = i;
                break;
            }
        }
        GameRenderer = SDL_CreateRenderer(GameWindow, render_num, RENDERER_FLAGS);
        result = !GameWindow && !GameRenderer && !FontInit();
        SDL_Surface *image = NULL; // TODO: Create a proper logo
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
