#include "font.h"
#include <string.h>

// TODO: Move this helper elsewhere
Uint32 *Uint8PixelsToUint32Pixels(Uint8 *pointer, int width, int height)
{
    Uint32 *array = calloc(width * height, sizeof(Uint32));
    if (array)
        for (int i = 0; i < width * height; i++)
        {
            Uint32 current = pointer[i];
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            array[i] = (current << 24) + (current << 16) + (current << 8) + 0xFF;
#else // Little endian
            array[i] = (0xFF << 24) + (current << 16) + (current << 8) + current;
#endif // SDL_BYTEORDER == SDL_BIG_ENDIAN
        }
    return array;
}

#define CHAR_W 3
#define CHAR_H 5
#define CHAR_SIZE CHAR_W * CHAR_H
#define GRAND_CHAR_MIN 65 // A
#define GRAND_CHAR_MAX 90 // Z
#define CHAR_COUNT GRAND_CHAR_MAX - GRAND_CHAR_MIN + 1

static void *CharDataPointers[CHAR_COUNT];
static SDL_Surface *CharSurfs[CHAR_COUNT];
static SDL_Texture *CharTextures[CHAR_COUNT];

static int LoadCharacters();
static int LoadCharacter();

/* 0 is good, nothing bad happened */
int FontInit()
{
    int result = 0;
    result |= !(SDL_WasInit(0) & SDL_INIT_VIDEO);
    if (!result)
        result |= LoadCharacters();
    return result;
}

int FontQuit()
{
    for (int i = 0; i < CHAR_COUNT; i++)
    {
        if (CharSurfs[i])
            SDL_FreeSurface(CharSurfs[i]);
        if (CharTextures[i])
            SDL_DestroyTexture(CharTextures[i]);
        if (CharDataPointers[i])
            free(CharDataPointers[i]);
        CharSurfs[i] = NULL;
        CharTextures[i] = NULL;
        CharDataPointers[i] = NULL;
    }
    return 0;
}

// TODO: make these Uint8's, cause 3 * 5 = 15 < 16 so we have the bits
static Uint8 RawChars[CHAR_COUNT][CHAR_SIZE] = {
    {0x00, 0x00, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00}, // A

    {0x00, 0x00, 0xFF,
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0xFF,
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0xFF}, // B

    {0x00, 0x00, 0x00,
     0x00, 0xFF, 0xFF,
     0x00, 0xFF, 0xFF,
     0x00, 0xFF, 0xFF,
     0x00, 0xFF, 0x00}, // C

    {0x00, 0x00, 0xFF,
     0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0xFF}, // D

    {0x00, 0x00, 0x00,
     0x00, 0xFF, 0xFF,
     0x00, 0x00, 0x00,
     0x00, 0xFF, 0xFF,
     0x00, 0x00, 0x00}, // E

    {0x00, 0x00, 0x00,
     0x00, 0xFF, 0xFF,
     0x00, 0x00, 0xFF,
     0x00, 0xFF, 0xFF,
     0x00, 0xFF, 0xFF}, // F

    {0x00, 0x00, 0x00,
     0x00, 0xFF, 0xFF,
     0x00, 0xFF, 0xFF,
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0x00}, // G

    {0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00}, // H

    {0x00, 0x00, 0x00,
     0xFF, 0x00, 0xFF,
     0xFF, 0x00, 0xFF,
     0xFF, 0x00, 0xFF,
     0x00, 0x00, 0x00}, // I

    {0xFF, 0xFF, 0x00,
     0xFF, 0xFF, 0x00,
     0xFF, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0x00}, // J

    {0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0xFF,
     0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00}, // K

    {0x00, 0xFF, 0xFF,
     0x00, 0xFF, 0xFF,
     0x00, 0xFF, 0xFF,
     0x00, 0xFF, 0xFF,
     0x00, 0x00, 0x00}, // L

    {0x00, 0x00, 0x00,
     0x00, 0x00, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00}, // M

    {0x00, 0xFF, 0x00,
     0x00, 0x00, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00}, // N

    {0x00, 0x00, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0x00}, // O

    {0x00, 0x00, 0x00,
     0x00, 0x00, 0x00,
     0x00, 0xFF, 0xFF,
     0x00, 0xFF, 0xFF,
     0x00, 0xFF, 0xFF}, // P

    {0x00, 0x00, 0x00,
     0x00, 0x00, 0x00,
     0xFF, 0xFF, 0x00,
     0xFF, 0xFF, 0x00,
     0xFF, 0xFF, 0x00}, // Q

    {0x00, 0x00, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0xFF,
     0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00}, // R

    {0x00, 0x00, 0x00,
     0x00, 0xFF, 0xFF,
     0x00, 0x00, 0x00,
     0xFF, 0xFF, 0x00,
     0x00, 0x00, 0x00}, // S

    {0x00, 0x00, 0x00,
     0xFF, 0x00, 0xFF,
     0xFF, 0x00, 0xFF,
     0xFF, 0x00, 0xFF,
     0xFF, 0x00, 0xFF}, // T

    {0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0x00}, // U

    {0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0xFF, 0x00, 0xFF}, // V

    {0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0x00,
     0x00, 0x00, 0x00}, // W

    {0x00, 0xFF, 0x00,
     0xFF, 0x00, 0xFF,
     0xFF, 0x00, 0xFF,
     0xFF, 0x00, 0xFF,
     0x00, 0xFF, 0x00}, // X

    {0x00, 0xFF, 0x00,
     0x00, 0xFF, 0x00,
     0x00, 0x00, 0x00,
     0xFF, 0x00, 0xFF,
     0xFF, 0x00, 0xFF}, // Y

    {0x00, 0x00, 0x00,
     0xFF, 0xFF, 0x00,
     0xFF, 0x00, 0xFF,
     0x00, 0xFF, 0xFF,
     0x00, 0x00, 0x00} // Z
};

static int LoadCharacters()
{
    int result = 0;
    for (int i = 0; i < CHAR_COUNT && !result; i++)
        result |= LoadCharacter(i);
    return result;
}

static int LoadCharacter(int index)
{
    void *pointer = (void*) Uint8PixelsToUint32Pixels(RawChars[index], CHAR_W, CHAR_H);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    SDL_Surface *s = SDL_CreateRGBSurfaceFrom(pointer, CHAR_W, CHAR_H, 32, 4 * CHAR_W,
                                              0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
#else
    SDL_Surface *s = SDL_CreateRGBSurfaceFrom(pointer, CHAR_W, CHAR_H, 32, 4 * CHAR_W,
                                              0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
#endif // SDL_BYTEORDER
    SDL_SetColorKey(s, SDL_TRUE, 0xFFFFFFFF);
    CharDataPointers[index] = pointer;
    CharSurfs[index] = s;
    CharTextures[index] = NULL;
    return s == NULL;
}

// I don't like this
SDL_Texture *CharTexture(SDL_Renderer *renderer, char ch)
{
    if ((ch > GRAND_CHAR_MAX) | (ch < GRAND_CHAR_MIN))
        return NULL;
    size_t index = ch - GRAND_CHAR_MIN;
    if (CharTextures[index])
        return CharTextures[index];
    if (CharSurfs[index])
    {
        CharTextures[index] = SDL_CreateTextureFromSurface(renderer, CharSurfs[index]);
    }
    return CharTextures[index];
}

SDL_Texture *GimmeTexture(SDL_Renderer *renderer, char *string, size_t size)
{
    const int width = size * CHAR_W;
    const int height = size * CHAR_H;
    const int len = strlen(string);
    // Uncomment when it works lol
    /*
    if (len == 1)
        return CharTexture(renderer, *string);*/
    SDL_Texture *old = SDL_GetRenderTarget(renderer);
    SDL_Texture *result = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                            SDL_TEXTUREACCESS_TARGET,
                                            len * width * 1.5, len * height * 1.5);
    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);
    if (result)
    {
        SDL_Rect dimension = {0, 0, width, height};
        SDL_Texture *t;
        printf("%i<-\n", SDL_SetRenderTarget(renderer, result));
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xFF, 0xFF);
        SDL_RenderClear(renderer);
        SDL_RenderDrawRect(renderer, NULL);
        for (; *string; string++)
        {
            t = CharTexture(renderer, *string);
            printf("%c\n", *string);
            printf("%p\n", (void*)t);
            if (t)
            {
                //SDL_SetTextureColorMod(t, 0xFF, 0x00, 0x00);
                printf("%i ->%s<-\n", SDL_RenderCopy(renderer, t, NULL, &dimension), SDL_GetError());
            }
            dimension.x += width * 1.5;
        }
        printf("->%i\n", SDL_RenderDrawLine(renderer, 0, 0, 9, 15));
        SDL_SetRenderTarget(renderer, old);
    }
    printf("DONE: %p\n", (void*) result);
    return result;
}

