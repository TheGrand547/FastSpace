#include "button.h"
#include <SDL2/SDL.h>

Button *ButtonCreate(SDL_Rect rect, ButtonFunc func)
{
    Button *button = calloc(1, sizeof(Button));
    if (button)
    {
        button->rect = rect;
        button->func = func;
    }
    return button;
}

int ButtonCheck(Button *button, SDL_Event *e)
{
    if (!button || !e)
        return 0;
    SDL_Point p = {e->button.x, e->button.y};
    int result = SDL_PointInRect(&p, &button->rect);
    if (result)
        button->func();
    return result;
}

void DrawButton(Button* button)
{
    if (!button)
        return;
    SDL_SetRenderDrawColor(GameRenderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(GameRenderer, &button->rect);
}

void VoidButton() {}


