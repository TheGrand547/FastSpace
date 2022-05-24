#pragma once
#ifndef SUPER_HEADER_H
#define SUPER_HEADER_H
#include <math.h>
#include <SDL2/SDL.h>
#include "field.h"

#define STR(x) #x
#define CONST_STR(x) const char * x##_str = STR(x)
#define UNUSED(x) (void)(x)
#define FLOAT_EPSILON 0.0001
#define FLOAT_EQUAL(x, y) (fabs((x) - (y)) < FLOAT_EPSILON)
#define NULL_CHECK(x) if (!x) return;
#define NULL_CHECK_RETURN(x, ret) if (!x) return ret;

typedef enum
{
    UP, RIGHT, DOWN, LEFT
} Facing;

typedef enum
{
    PLAYER, AI, MISC
} Turn;

typedef SDL_Color Color;

extern SDL_Renderer *GameRenderer;
extern SDL_Window *GameWindow;
extern Field GameField;
#define DisplayPixelFormat SDL_GetWindowSurface(GameWindow)->format

#endif // SUPER_HEADER_H

