#pragma once
#ifndef SUPER_HEADER_H
#define SUPER_HEADER_H
#include <SDL2/SDL.h>
#include "field.h"

#define UNUSED(x) (void)(x)

typedef enum
{
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3
} Facing;

typedef SDL_Color Color;

SDL_PixelFormat* GetPixelFormat();
SDL_Renderer* GetRenderer();
SDL_Window* GetWindow();
Field* GetField();

#endif // SUPER_HEADER_H

