#pragma once
#ifndef DEBUG_DISPLAY_H
#define DEBUG_DISPLAY_H
#include <SDL2/SDL.h>
#include "super_header.h"

typedef enum {
    SHOW_FPS       = 0x01,
    SHOW_TURN      = 0x02,
    SHOW_COUNTDOWN = 0x04
} DebugDisplayFlags;

typedef enum {
    TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT
} DisplayLocation;

// File for all potentially displayed things regarding debug stuff
// Like AI timer
void InitDebugDisplay(char **argv);
void EnableDebugDisplays(uint8_t flags);

void DebugDisplayDraw();

void SetLocation(SDL_Rect *rect, DisplayLocation location);

#endif // DEBUG_DISPLAY_H
