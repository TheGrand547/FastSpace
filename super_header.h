#pragma once
#ifndef SUPER_HEADER_H
#define SUPER_HEADER_H
#include <SDL2/SDL.h>
#include "field.h"

typedef enum
{
    UP, RIGHT, DOWN, LEFT
} Facing;

typedef SDL_Color Color;

SDL_PixelFormat* GetPixelFormat();
SDL_Renderer* GetRenderer();
SDL_Window* GetWindow();
Field* GetField();

#endif // SUPER_HEADER_H

