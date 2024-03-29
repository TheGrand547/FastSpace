#pragma once
#ifndef BUTTON_H
#define BUTTON_H
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include "super_header.h"

typedef void (*ButtonFunc)();

typedef struct
{
    SDL_Rect rect;
    SDL_Texture *texture;
    ButtonFunc func;
    void *data;
} Button;

void VoidButton();
void DrawButton(Button *button);

Button *ButtonCreate(SDL_Rect rect, ButtonFunc func);
int ButtonCheck(Button *button, SDL_Event *e);

#endif // BUTTON_H
