#include "font.h"
#include <string.h>
#include <limits.h>

#define CHAR_W 3
#define CHAR_H 5
#define CHAR_SIZE CHAR_W * CHAR_H
#define GRAND_CHAR_MIN ' '
#define GRAND_CHAR_MAX '~' + 1
#define CHAR_COUNT GRAND_CHAR_MAX - GRAND_CHAR_MIN + 1
#define CHAR_SPACING 1.25
#define DUPLICATE_OFFSET 32 // letter as char(lower) = letter as char(upper) + 32
#define NUM_DUPLICATES 26  // 26 letters

#define CHAR_BOUNDS_CHECK(x) (x > GRAND_CHAR_MAX) || (x < GRAND_CHAR_MIN)

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

// In case someone wants to make a larger(or smaller) version they just need to change
// this line, CHAR_W, CHAR_H and the CompressedFontData array
typedef Uint16 DataType;

static const double sizeConst = ((double) CHAR_W) / ((double) (CHAR_H));
static Uint32 *CharDataPointers[CHAR_COUNT];
static SDL_Surface *CharSurfaces[CHAR_COUNT];
static SDL_Texture *CharTextures[CHAR_COUNT];
static const DataType CompressedFontData[CHAR_COUNT - NUM_DUPLICATES] =
{
    0x0000, //  <- space char
    0x4904, // !
    0xB400, // "
    0x147A, // # - :( face
    0x1470, // $ - :| face
    0xA54A, // %
    0x145E, // & - :) face
    0x4800, // '
    0x5244, // (
    0x4494, // )
    0xAA80, // *
    0x0BA0, // +
    0x0028, // ,
    0x0380, // -
    0x0008, // .
    0x2548, // /
    0xF6DE, // 0
    0x592E, // 1
    0x554E, // 2
    0xC51C, // 3
    0x2F92, // 4
    0xF11C, // 5
    0xF3DE, // 6
    0xE548, // 7
    0xF7DE, // 8
    0xF792, // 9
    0x0820, // :
    0x0828, // ;
    0x2A22, // <
    0x1C70, // =
    0x88A8, // >
    0xC504, // ?
    0xFFFF, // @ <- Need to find something for this
    0xF7DA, // A
    0xD75C, // B
    0xF24E, // C
    0xD6DC, // D
    0xF34E, // E
    0xF348, // F
    0xF25E, // G
    0xB7DA, // H
    0xE92E, // I
    0x24DE, // J
    0xB75A, // K
    0x924E, // L
    0xFEDB, // M
    0xBEDB, // N
    0xF6DE, // O
    0xFE48, // P
    0xFC92, // Q
    0xF75B, // R
    0xF39E, // S
    0xE924, // T
    0xB6DE, // U
    0xB6D4, // V
    0xB6FE, // W
    0xBD7A, // X
    0xB7A4, // Y
    0xE54E, // Z
    0xD24C, // [
    0x9112, // \ //
    0x6496, // ]
    0x5400, // ^
    0x000E, // _
    0x8800, // `
    0x2B22, // {
    0x4924, // |
    0x89A8, // }
    0x07C0  // ~
};

static int LoadCharacter(unsigned char ch);
static char FontTransformChar(unsigned char ch);
static SDL_Surface *CharSurface(unsigned char ch);
static Uint32 *DataToPixelArray(const DataType based);

static Uint32 *DataToPixelArray(const DataType based)
{
    #define DATA_BIT_COUNT (sizeof(DataType) * 8)
    Uint32 *array = calloc(CHAR_SIZE, sizeof(Uint32));
    if (array)
        for (int i = 0; i < CHAR_SIZE; i++)
        {
            Uint32 current = ((based >> (DATA_BIT_COUNT - 1 - i)) & 1) ? 0xFF : 0x00;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            array[i] = (current << 24) + (current << 16) + (current << 8) + 0xFF;
#else // Little endian
            array[i] = (0xFF << 24) + (current << 16) + (current << 8) + current;
#endif // SDL_BYTEORDER == SDL_BIG_ENDIAN
        }
    return array;
}

/* 0 is good, nothing bad happened */
int FontInit()
{
    int result = 0;
    result |= !(SDL_WasInit(0) & SDL_INIT_VIDEO);
    if (!result)
        for (char ch = 'A'; ch <= 'Z' && !result; ch++)
            result |= LoadCharacter(ch);
    return result;
}

int FontQuit()
{
    for (int i = 0; i < CHAR_COUNT; i++)
    {
        if (CharSurfaces[i])
            SDL_FreeSurface(CharSurfaces[i]);
        if (CharTextures[i])
            SDL_DestroyTexture(CharTextures[i]);
        if (CharDataPointers[i])
            free(CharDataPointers[i]);
        CharSurfaces[i] = NULL;
        CharTextures[i] = NULL;
        CharDataPointers[i] = NULL;
    }
    return 0;
}

static char FontTransformChar(unsigned char ch)
{
    if (ch >= GRAND_CHAR_MAX)
        return 0;
    if (ch >= 'a' && ch <= 'z')
    {
        return FontTransformChar(ch - DUPLICATE_OFFSET);
    }
    if (ch >= 'a')
        return ch - 26 - GRAND_CHAR_MIN;
    return ch - GRAND_CHAR_MIN;
}

int FontLoadCharacters()
{
    int result = 0;
    for (char i = GRAND_CHAR_MIN; i < GRAND_CHAR_MAX && !result; i++)
        result |= LoadCharacter(i);
    return result;
}

static int LoadCharacter(unsigned char index)
{
    index = FontTransformChar(index);
    if (CharSurfaces[(int) index])
        return 0;
    Uint32 *pointer = DataToPixelArray(CompressedFontData[(int) index]);
    SDL_Surface *s = SDL_CreateRGBSurfaceFrom(pointer, CHAR_W, CHAR_H, 32, 4 * CHAR_W,
                                              R_MASK, G_MASK, B_MASK, A_MASK);
    SDL_SetColorKey(s, SDL_TRUE, 0x00000000);
    CharDataPointers[(int) index] = pointer;
    CharSurfaces[(int) index] = s;
    CharTextures[(int) index] = NULL;
    return s == NULL;
}

SDL_Point FontGetSizeFromLength(size_t len, size_t scale)
{
    SDL_Point size = FontGetCharSize(scale);
    if (len == 0)
        return (SDL_Point) {0, 0};
    if (len == 1)
        return size;
    return (SDL_Point) {size.x * len * CHAR_SPACING - (CHAR_SPACING - 1) * size.x, size.y};
}

SDL_Point FontGetTextSize(const char *string, size_t scale)
{
    return FontGetSizeFromLength(strlen(string), scale);
}

SDL_Point FontGetCharSize(size_t scale)
{
    return (SDL_Point) {scale * sizeConst, scale};
}

static SDL_Surface *CharSurface(unsigned char ch)
{
    if (CHAR_BOUNDS_CHECK(ch))
        return NULL;
    size_t index = FontTransformChar(ch);
    if (!CharSurfaces[index])
        LoadCharacter(ch);
    return CharSurfaces[index];
}

SDL_Texture *FontRenderChar(SDL_Renderer *renderer, unsigned char ch)
{
    if (CHAR_BOUNDS_CHECK(ch))
        return NULL;
    size_t index = FontTransformChar(ch);
    if (CharTextures[index])
        return CharTextures[index];
    CharTextures[index] = SDL_CreateTextureFromSurface(renderer, CharSurface(ch));
    return CharTextures[index];
}

SDL_Texture *FontRenderText(SDL_Renderer *renderer, const char *string, size_t size)
{
    return FontRenderTextSize(renderer, string, size, NULL);
}

SDL_Texture *FontRenderTextSize(SDL_Renderer *renderer, const char *string, size_t size, SDL_Rect *rect)
{
    const int width  = size * sizeConst;
    const int height = size;
    const int len    = strlen(string);
    if (len == 0)
        return NULL;
    if (len == 1)
        return FontRenderChar(renderer, *string);
    SDL_Point realSize = FontGetSizeFromLength(len, size);
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
    if (rect)
    {
        rect->x = 0;
        rect->y = 0;
        rect->w = realSize.x;
        rect->h = realSize.y;
    }
    return result;
}

