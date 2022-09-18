#include "setup.h"
#include "flags.h"
#include "font.h"
#include "super_header.h"

#define FIRST_DISPLAY 0

int InitializeLibraries()
{
    int result = SDL_Init(SDL_INIT_FLAGS);
    if (!result)
    {
        // TODO: Kill magic number 300
        SDL_Rect rect;
        SDL_GetDisplayUsableBounds(FIRST_DISPLAY, &rect);
        GameWindow = SDL_CreateWindow("Fast Space Thing", SDL_WINDOW_POS, SDL_WINDOW_POS,
                                  rect.h + 300, rect.h, SDL_WINDOW_FLAGS);
        int top, left, bottom, right;
        SDL_GetWindowBordersSize(GameWindow, &top, &left, &bottom, &right);
        SDL_SetWindowSize(GameWindow, rect.h + 300, rect.h - top);
        SDL_SetWindowPosition(GameWindow, SDL_WINDOWPOS_CENTERED, top);
        SDL_ShowWindow(GameWindow);

        int render_num = -1;
        SDL_RendererInfo info;
        for (int i = 0; i < SDL_GetNumRenderDrivers(); i++)
        {
            SDL_GetRenderDriverInfo(i, &info);
            // direct3d on intel some problems with render targets for some reason, so ignore
            // it if other renderers are available
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
