#include "field.h"
#include "super_header.h"

void DrawField(Field *field)
{
    if (!field)
        return;
    SDL_SetRenderDrawColor(GameRenderer, 0x00, 0x48, 0xCF, 0xFF);
    SDL_Rect rect = {0, 0, field->rectWidth, field->rectHeight};
    for (unsigned int x = 0; x < field->width; x++)
    {
        rect.x = x * (field->rectWidth + field->spacing);
        for (unsigned int y = 0; y < field->height; y++)
        {
            rect.y = y * (field->rectHeight + field->spacing);
            SDL_RenderFillRect(GameRenderer, &rect);
        }
    }
}
