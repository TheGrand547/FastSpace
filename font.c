#include "font.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>

// These can be modified as needed if you want to use my code to implement your own font
#define CHAR_W 3
#define CHAR_H 5

#define CHAR_SIZE CHAR_W * CHAR_H
#define GRAND_CHAR_MIN ' '
#define GRAND_CHAR_MAX '~' + 1
#define CHAR_COUNT GRAND_CHAR_MAX - GRAND_CHAR_MIN + 1
#define DUPLICATE_OFFSET 32 // letter as char(lower) = letter as char(upper) + 32
#define NUM_DUPLICATES 26   // 26 letters

#define CHAR_BOUNDS_CHECK(x) ((x) > GRAND_CHAR_MAX) | ((x) < GRAND_CHAR_MIN)

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
typedef uint16_t DataType;

#define DATA_BIT_COUNT (sizeof(DataType) * 8)

static int tabWidth = 4;
static double charHorizontalSpacing = 1.25;
static double charVerticalSpacing = 1.25;
static const double sizeConst = ((double) CHAR_W) / ((double) (CHAR_H));
static uint32_t *CharDataPointers[CHAR_COUNT];
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
    0x57DA, // A
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
    0xF7C8, // P
    0xF792, // Q
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

static int ProperSpacing(int normal, int scaled);
static int GetXDelta(size_t scale);
static int GetYDelta(size_t scale);
static int LoadCharacter(unsigned char ch);
static char FontTransformChar(unsigned char ch);
static SDL_Surface *CharSurface(unsigned char ch);
static size_t modifiedStrlen(const char *string);
static size_t lineStrlen(const char *string);
static uint32_t *DataToPixelArray(const DataType source);

static uint32_t *DataToPixelArray(const DataType source)
{
    uint32_t *array = calloc(CHAR_SIZE, sizeof(uint32_t));
    if (array)
        for (unsigned int i = 0; i < CHAR_SIZE; i++)
        {
            uint32_t current = ((source >> (DATA_BIT_COUNT - 1 - i)) & 1) ? 0xFF : 0x00;
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
    if (CHAR_SIZE > DATA_BIT_COUNT)
        fprintf(stderr, "Font: Loading char data from a storage type that is too small, visual issues may arise.");
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

int FontLoadAllCharacters()
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
    uint32_t *pointer = DataToPixelArray(CompressedFontData[(int) index]);
    SDL_Surface *s = SDL_CreateRGBSurfaceFrom(pointer, CHAR_W, CHAR_H, 32, sizeof(uint32_t) * CHAR_W,
                                              R_MASK, G_MASK, B_MASK, A_MASK);
    SDL_SetColorKey(s, SDL_TRUE, A_MASK); // Black = transparent
    CharDataPointers[(int) index] = pointer;
    CharSurfaces[(int) index] = s;
    CharTextures[(int) index] = NULL;
    return s == NULL;
}

static int ProperSpacing(int normal, int scaled)
{
    return (scaled > normal) ? scaled : normal + 1;
}

static int GetXDelta(size_t scale)
{
    int adjusted = scale * sizeConst;
    return ProperSpacing(adjusted, adjusted * charHorizontalSpacing);
}

static int GetYDelta(size_t scale)
{
    return ProperSpacing(scale, scale * charVerticalSpacing);
}

static size_t modifiedStrlen(const char *string)
{
    size_t size = 0;
    for (; *string; string++)
    {
        if (*string == '\t')
            size += tabWidth;
        else
            size++;
    }
    return size;
}

// length in characters of the line, with trailing whitespace removed
static size_t lineStrlen(const char *string)
{
    size_t realSize = strlen(string);
    size_t size = modifiedStrlen(string);
    for (; strchr(" \t", string[realSize - 1]); realSize--, size--)
    {
        if (string[realSize - 1] == '\t')
            size -= tabWidth - 1;
    }
    return size;
}

SDL_Point FontGetTextSize(const char *string, size_t scale)
{
    SDL_Point size = {0, 0};
    const int length = modifiedStrlen(string);
    if (strchr(string, '\n'))
    {
        char *buffer = strdup(string);
        char *delimited = strtok(buffer, "\n");
        for (; delimited; delimited = strtok(NULL, "\n"))
        {
            size.y++;
            int len = lineStrlen(delimited);
            if (len > size.x)
                size.x = len;
        }
        free(buffer);
    }
    else
    {
        size.x = length;
        size.y = 1;
    }
    // Subtract the trailing spacing
    size.x = (size.x * GetXDelta(scale)) - scale * sizeConst * (charHorizontalSpacing - 1);
    size.y = (size.y * GetYDelta(scale)) - scale * (charVerticalSpacing - 1);
    return size;
}

SDL_Point FontGetCharSize(size_t scale)
{
    return (SDL_Point) {scale * sizeConst, scale};
}

SDL_Point FontGetCharSizeWithPadding(size_t scale)
{
    return (SDL_Point) {GetXDelta(scale), GetYDelta(scale)};
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
    const int width       = size * sizeConst;
    const int height      = size;
    const int len         = strlen(string);
    const int widthDelta  = GetXDelta(size);
    const int heightDelta = GetYDelta(size);
    if (rect)
    {
        rect->x = 0;
        rect->y = 0;
        rect->w = 0;
        rect->h = 0;
    }
    if ((len == 0) | (!string))
        return NULL;
    SDL_Point realSize = FontGetTextSize(string, size);
    if (rect)
    {
        rect->w = realSize.x;
        rect->h = realSize.y;
    }
    if (len == 1)
        return FontRenderChar(renderer, *string);
    SDL_Surface *surf = SDL_CreateRGBSurface(0, realSize.x, realSize.y, 32,
                                             R_MASK, G_MASK, B_MASK, A_MASK);
    if (surf)
    {
        SDL_Rect dimension = {0, 0, width, height};
        SDL_Surface *s;
        SDL_SetColorKey(surf, SDL_TRUE, A_MASK);
        for (; *string; string++)
        {
            char current = *string;
            if (current == '\n')
            {
                dimension.y += heightDelta;
                dimension.x = 0;
                continue;
            }
            if (current == '\t')
            {
                dimension.x += widthDelta * tabWidth;
                continue;
            }
            s = CharSurface(current);
            if (s)
                SDL_BlitScaled(s, NULL, surf, &dimension);
            dimension.x += widthDelta;
        }
    }
    SDL_Texture *result = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_SetTextureColorMod(result, 0x00, 0x00, 0x00);
    SDL_FreeSurface(surf);
    return result;
}

SDL_Texture *FontRenderTextWrapped(SDL_Renderer *renderer, const char *string, size_t size, size_t maxWidth)
{
    return FontRenderTextWrappedSize(renderer, string, size, maxWidth, NULL);
}

SDL_Texture *FontRenderTextWrappedSize(SDL_Renderer *renderer, const char *string, size_t size,
                                       size_t maxWidth, SDL_Rect *rect)
{
    SDL_Point unmodified = FontGetTextSize(string, size);
    if ((size_t) unmodified.x <= maxWidth)
        return FontRenderTextSize(renderer, string, size, rect);
    size_t currentWidth = 0;
    const size_t len = strlen(string);
    const size_t xDelta = GetXDelta(size);
    char *newstring = calloc(2 * len, sizeof(char));
    size_t index = 0;

    // Split the input into chunks that cannot be separated, based on white space
    char **datum = StrSplit(string, " ");
    for (unsigned int i = 0; datum[i]; i++)
    {
        char *current = datum[i];
        const size_t subSize = FontGetTextSize(current, size).x;
        const size_t subLen = strlen(current);
        const char *newline = strstr(current, "\n");
        if (subSize > maxWidth) // TODO: Some funky things with vertical tabs
            printf("This behavior has not been defined\n");
        if (newline)
        {
            // If it's just the new line token, then place it and continue
            if (subLen == 1)
            {
                currentWidth = 0;
                newstring[index++] = '\n';
                continue;
            }
            // Otherwise we just trust them to know what they're doing
            const char **currentSplit = StrSplit(current, "\n");
            char **copy = currentSplit;
            for (; *copy; copy++);
            currentWidth = FontGetTextSize(copy - 1, size).x;
            StrSplitCleanup(currentSplit);
        }
        else if ((currentWidth + subSize >= maxWidth) && index > 0)
        {
            currentWidth = 0;
            newstring[index - 1] = '\n';
        }
        currentWidth += subSize + xDelta;
        strcpy(newstring + index, current);
        index += subLen;
        if (!newline)
            newstring[index++] = ' ';
    }
    newstring[index - 1] = '\0'; // Null terminate
    StrSplitCleanup(datum);
    newstring = realloc(newstring, index);
    SDL_Texture *result = FontRenderTextSize(renderer, newstring, size, rect);
    free(newstring);
    return result;
}

void FontSetHorizontalSpacing(double spacing)
{
    charHorizontalSpacing = spacing;
}

void FontSetTabWidth(int width)
{
    tabWidth = width;
}

void FontSetVerticalSpacing(double spacing)
{
    charVerticalSpacing = spacing;
}

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
