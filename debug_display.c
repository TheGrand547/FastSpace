#include "debug_display.h"
#include <stdio.h>
#include "field.h"
#include "font.h"
#include "super_header.h"

static uint8_t InitializedDisplays;
static DisplayLocation DisplaysPlace[] = {TOP_RIGHT, TOP_RIGHT, TOP_RIGHT};

void InitDebugDisplay(char **argv)
{
    InitializedDisplays = 0;
    printf(">>");
    for (; *argv ; argv++)
        printf("%s", *argv);
    printf("\n");
}

static void EnableSpecificDebug(DebugDisplayFlags flags);
static void EnableFpsDisplay();
static void FpsDisplayInternal();
static void DestroyFpsDisplay();

void EnableDebugDisplays(uint8_t flags)
{
    for (unsigned int i = 0; i < 8; i++)
    {
        if ((flags >> i) & 1)
            printf("%i\n", flags >> i);
    }
    EnableFpsDisplay();
}

void DebugDisplayDraw()
{
    FpsDisplayInternal();
}

/** FPS stuff **/
// Must be static or it will be leaked
static SDL_Texture *fpsTexture = NULL;
// Delay
#define FPS_MS_UPDATE_FREQ 50

void EnableFpsDisplay()
{
    // Nothing here, epic
}

void DestroyFpsDisplay()
{
    SDL_DestroyTexture(fpsTexture);
    fpsTexture = NULL;
}

void FpsDisplayInternal()
{
    static double fps = 0;
    static uint32_t counted = 0;
    static uint32_t fpsStart = 0;
    static char fpsText[11]; // FPS: 0000 -> 4 + 2 + 4 + 1 for null -> 11
    static SDL_Rect fpsRect = {0, 0, 0, 0};

    const uint32_t difference = SDL_GetTicks() - fpsStart;
    if (difference > FPS_MS_UPDATE_FREQ)
    {
        fps = counted / (FPS_MS_UPDATE_FREQ * 0.001f);
        SDL_DestroyTexture(fpsTexture);
        sprintf(fpsText, "FPS: %4.0lf", fps);
        fpsTexture = FontRenderTextSize(GameRenderer, fpsText, 15, &fpsRect);
        SDL_SetTextureColorMod(fpsTexture, 0xFF, 0x00, 0x00);
        fpsStart = SDL_GetTicks();
        counted = 0;
    }
    SetLocation(&fpsRect, TOP_RIGHT);
    SDL_RenderCopy(GameRenderer, fpsTexture, NULL, &fpsRect);
    counted++;
}

void SetLocation(SDL_Rect *rect, DisplayLocation location)
{
    static int offsets[] = {0, 0, 0, 0};
    if (!rect)
    {
        for (unsigned int i = 0; i < sizeof(offsets) / sizeof(offsets[0]); i++)
            offsets[i] = 0;
        return;
    }
    int offset = offsets[location];
    if (location == TOP_RIGHT || location == BOTTOM_RIGHT)
        rect->x = WindowSizeX() - rect->w;
    if (location == BOTTOM_LEFT || location == BOTTOM_RIGHT)
        rect->y = WindowSizeY() - rect->h - offset;
    else
        rect->y = offset;
    offsets[location] += rect->h * 1.25;
}

static void EnableSpecificDebug(DebugDisplayFlags flags)
{
    UNUSED(flags);
}
