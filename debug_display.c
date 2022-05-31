#include "debug_display.h"
#include <math.h>
#include <stdio.h>
#include "field.h"
#include "font.h"
#include "super_header.h"

#define EnumToIndex(x) ((size_t) log2(x))

static uint8_t InitializedDisplays;
static DisplayLocation DisplaysPlace[] = {TOP_RIGHT, TOP_RIGHT, TOP_RIGHT};
static const char *FlagStrs[] = {STR(SHOW_FPS), STR(SHOW_TURN), STR(SHOW_COUNTDOWN)};

// FPS Declarations
static void EnableFpsDisplay(void *data);
static void DestroyFpsDisplay();
static void DrawFpsDisplay();

// Turn Declarations
static void EnableTurnDisplay(void *data);
static void DestroyTurnDisplay();
static void DrawTurnDisplay();

static void SetLocation(SDL_Rect *rect, DisplayLocation location);

void InitDebugDisplay(int argc, char **argv)
{
    InitializedDisplays = 0;
    printf(">>");
    for (int i = 0; i < argc; i++)
    {
        printf("%s", argv[i]);
    }
    printf("\n");
}

void QuitDebugDisplay()
{
    const uint8_t toKill = InitializedDisplays;
    if (!toKill)
        return;
    if (toKill & SHOW_FPS)
        DestroyFpsDisplay();
    if (toKill & SHOW_TURN)
        DestroyTurnDisplay();
    InitializedDisplays = 0;
}

void EnableDebugDisplay(DebugDisplayFlags flag, DisplayLocation location, void *data)
{
    switch (flag)
    {
        case SHOW_FPS:
        {
            EnableFpsDisplay(data);
            break;
        }
        case SHOW_TURN:
        {
            EnableTurnDisplay(data);
            break;
        }
        default:
        {
            printf("Flag '%s' has not be implemented\n", FlagStrs[EnumToIndex(flag)]);
            return;
        }
    }
    DisplaysPlace[EnumToIndex(flag)] = location;
    InitializedDisplays |= flag;
}

void DebugDisplayDraw()
{
    // TODO: There's a way to write this properly
    SetLocation(NULL, TOP_RIGHT);
    if (InitializedDisplays & SHOW_FPS)
        DrawFpsDisplay();
    if (InitializedDisplays & SHOW_TURN)
        DrawTurnDisplay();
}

/** FPS stuff **/
// Must be static or it will be leaked
static SDL_Texture *fpsTexture = NULL;
#define FPS_MS_UPDATE_FREQ 50 // Delay between fps polls

void EnableFpsDisplay(void *data)
{
    UNUSED(data);
    InitializedDisplays |= SHOW_FPS;
}

void DestroyFpsDisplay()
{
    SDL_DestroyTexture(fpsTexture);
    fpsTexture = NULL;
}

void DrawFpsDisplay()
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
    SetLocation(&fpsRect, DisplaysPlace[EnumToIndex(SHOW_FPS)]);
    SDL_RenderCopy(GameRenderer, fpsTexture, NULL, &fpsRect);
    counted++;
}


/** Turn Timer Stuff **/
static SDL_Texture *turnTexture = NULL;
static const Turn *currentTurn = NULL;
static const char *turnNames[] = {
    STR(PLAYER), STR(AI), STR(MISC),
    STR(PLAYER_BUFFER), STR(AI_BUFFER), STR(MISC_BUFFER)
};

static void EnableTurnDisplay(void *data)
{
    currentTurn = (Turn*) data;
}

static void DestroyTurnDisplay()
{
    SDL_DestroyTexture(turnTexture);
    turnTexture = NULL;
    currentTurn = NULL;
}

static void DrawTurnDisplay()
{
    if (!currentTurn)
        return;
    SDL_Rect turnRect;
    turnTexture = FontRenderTextSize(GameRenderer, turnNames[*currentTurn], 15, &turnRect);
    SetLocation(&turnRect, DisplaysPlace[EnumToIndex(SHOW_TURN)]);
    SDL_SetTextureColorMod(turnTexture, 0xFF, 0x00, 0x00);
    SDL_RenderCopy(GameRenderer, turnTexture, NULL, &turnRect);
}

static void SetLocation(SDL_Rect *rect, DisplayLocation location)
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
