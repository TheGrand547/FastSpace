#pragma once
#ifndef SUPER_HEADER_H
#define SUPER_HEADER_H
#include <math.h>
#include <SDL2/SDL_bits.h>
#include <SDL2/SDL_render.h>
#include "field.h"

#define STR(x) #x
#define CONST_STR(x) const char * x##_str = STR(x)
#define UNUSED(x) (void)(x)
#define FLOAT_EPSILON 0.0001
#define FLOAT_EQUAL(x, y) (fabs((x) - (y)) < FLOAT_EPSILON)
#define NULL_CHECK(x) if (!x) return;
#define NULL_CHECK_RETURN(x, ret) if (!x) return ret;
#define STATIC_ARRAY_LENGTH(array, type) sizeof(array) / sizeof(type)
#define POINTER_ARRAY_LENGTH(array) STATIC_ARRAY_LENGTH(array, void*)

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

typedef enum
{
    UP, RIGHT, DOWN, LEFT
} Facing;

typedef enum
{
    PLAYER, AI, MISC, PLAYER_BUFFER, AI_BUFFER, MISC_BUFFER
} Turn;

#define HIGH 0xFF
#define MID 0x80
#define LOW 0x00
typedef SDL_Color Color;

extern SDL_Renderer *GameRenderer;
extern SDL_Window *GameWindow;
extern Field GameField;
#define DisplayPixelFormat SDL_GetWindowSurface(GameWindow)->format

#endif // SUPER_HEADER_H

