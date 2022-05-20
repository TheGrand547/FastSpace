#include "misc.h"
#include <string.h>
#include <stdio.h>
#include "super_header.h"

char **StrSplit(const char *string, const char *delimiters)
{
    const size_t size = strlen(string);
    // A run time modifiable copy of the input string
    char *copy = strdup(string);
    // Could be up to string length number of substrings
    char **strs = calloc(size, sizeof(char*));
    if (strs)
    {
        size_t substrings = 0;
        char *data = strtok(copy, delimiters);
        do
        {
            strs[substrings++] = strdup(data);
        } while ((data = strtok(NULL, delimiters)));
        // Reduce size to the number substrings plus the null terminator
        strs = realloc(strs, (substrings + 1) * sizeof(char*));
    }
    free(copy);
    return strs;
}

void StrSplitCleanup(char **strings)
{
    char **original = strings;
    for (; *strings; strings++)
        free(*strings);
    free(original);
}


void OutputImage(SDL_Surface *surf)
{
#ifndef RELEASE
    FILE *f = fopen("img.out", "w");
    if (f)
    {
        surf = SDL_ConvertSurface(surf, DisplayPixelFormat, 0);
        Uint32 *arry = (Uint32*) surf->pixels;
        for (int y = 0; y < surf->h; y++)
        {
            for (int x = 0; x < surf->w; x++)
            {
                Uint32 gamers = arry[x + y * surf->w];
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

