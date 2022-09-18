#include "misc.h"
#include <string.h>
#include <stdio.h>
#include "super_header.h"

void PrintSDLError()
{
    const char *error = SDL_GetError();
    if (error)
    {
        printf("SDL_Error: %s\n", error);
        SDL_ClearError();
    }
}
