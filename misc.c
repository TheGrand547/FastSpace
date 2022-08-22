#include "misc.h"
#include <string.h>
#include <stdio.h>
#include "super_header.h"

void PrintSDLError()
{
    printf("SDL_Error: %s\n", SDL_GetError());
    SDL_ClearError();
}

void OutputImage(SDL_Surface *surf)
{
#ifndef RELEASE
    FILE *f = fopen("img.out", "w");
    if (f)
    {
        surf = SDL_ConvertSurface(surf, DisplayPixelFormat, 0);
        uint32_t *arry = (uint32_t*) surf->pixels;
        for (int y = 0; y < surf->h; y++)
        {
            for (int x = 0; x < surf->w; x++)
            {
                uint32_t gamers = arry[x + y * surf->w];
                fprintf(f, "0x%02X, ", (gamers & 0xFF00) >> 8);
            }
            fprintf(f, "\n");
        }
    }
    fclose(f);
#else
    UNUSED(surf);
#endif // RELEASE
}

