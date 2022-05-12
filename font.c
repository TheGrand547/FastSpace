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
#define GRAND_CHAR_MIN 'A' // A
#define GRAND_CHAR_MAX 'z' // Z
#define CHAR_COUNT GRAND_CHAR_MAX - GRAND_CHAR_MIN + 1
#define CHAR_SPACING 1.25

#define CHAR_BOUNDS_CHECK(x) (x > GRAND_CHAR_MAX) | (x < GRAND_CHAR_MIN)

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define R_MASK 0xFF000000
#define G_MASK 0x00FF0000
#define B_MASK 0x0000FF00
#define A_MASK 0x000000FF
#else // Little endian
#define R_MASK 0x000000FF
#define G_MASK 0x0000FF00
#define B_MASK 0x00FF0000
#define A_MASK 0xFF000000
#endif // SDL_BYTEORDER

static const double sizeConst = ((double) CHAR_W) / ((double) (CHAR_H));
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
     0x00, 0x00, 0x00}, // C

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
    Uint32 *pointer = Uint8PixelsToUint32Pixels(RawChars[index], CHAR_W, CHAR_H);
    // TODO: This is a fix for something stupid that I did
    for (int i = 0; i < CHAR_H; i++)
    {
        for (int j = 0; j < CHAR_W; j++)
        {
            pointer[i * CHAR_W + j] ^= 0xFFFFFFFF;
            pointer[i * CHAR_W + j] |= A_MASK;
        }
    }
    SDL_Surface *s = SDL_CreateRGBSurfaceFrom(pointer, CHAR_W, CHAR_H, 32, 4 * CHAR_W,
                                              R_MASK, G_MASK, B_MASK, A_MASK);
    SDL_SetColorKey(s, SDL_TRUE, 0x00000000);
    CharDataPointers[index] = pointer;
    CharSurfs[index] = s;
    CharTextures[index] = NULL;
    return s == NULL;
}

static char FontTransformChar(char ch)
{
    if (ch >= 'a' && ch <= 'z')
        return FontTransformChar(ch - 32);
    return ch - GRAND_CHAR_MIN;
}

SDL_Point GetSizeFromLength(size_t len, size_t scale)
{
    SDL_Point size = GetCharSize(scale);
    printf("size: %i %i\n", size.x, size.y);
    if (len == 0)
        return (SDL_Point) {0, 0};
    if (len == 1)
        return size;
    return (SDL_Point) {size.x * len * CHAR_SPACING - (CHAR_SPACING - 1) * size.x, size.y};
}

SDL_Point GetTextSize(char *string, size_t scale)
{
    return GetSizeFromLength(strlen(string), scale);
}

SDL_Point GetCharSize(size_t scale)
{
    return (SDL_Point) {scale * sizeConst, scale};
}

SDL_Surface *CharSurface(char ch)
{
    if (CHAR_BOUNDS_CHECK(ch))
        return NULL;
    size_t index = FontTransformChar(ch);
    if (!CharSurfs[index])
        LoadCharacter(ch);
    return CharSurfs[index];
}

// TODO: Fix this, it seems sloppy but I don't know why
SDL_Texture *CharTexture(SDL_Renderer *renderer, char ch)
{
    if (CHAR_BOUNDS_CHECK(ch))
        return NULL;
    size_t index = FontTransformChar(ch);
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
    const int width  = size * sizeConst;
    const int height = size;
    const int len    = strlen(string);
    if (len == 0)
        return NULL;
    if (len == 1)
        return CharTexture(renderer, *string);
    SDL_Point realSize = GetSizeFromLength(len, size);
    SDL_Surface *surf = SDL_CreateRGBSurface(0, realSize.x, realSize.y, 32,
                                             R_MASK, G_MASK, B_MASK, A_MASK);
    if (surf)
    {
        SDL_Rect dimension = {0, 0, width, height};
        SDL_Surface *s;
        SDL_SetColorKey(surf, SDL_TRUE, 0x00000000);
        for (; *string; string++)
        {
            s = CharSurface(*string);
            if (s)
                SDL_BlitScaled(s, NULL, surf, &dimension);
            dimension.x += width * CHAR_SPACING;
        }
    }
    SDL_Texture *result = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    return result;
}

