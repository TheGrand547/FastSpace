#include "field.h"
#include "super_header.h"

void DrawField(Field *field)
{
    if (!field)
        return;
    SDL_SetRenderDrawColor(GetRenderer(), 0x00, 0x48, 0xCF, 0xFF);
    SDL_Rect rect = {0, 0, field->rectWidth, field->rectHeight};
    for (int x = 0; x < field->width; x++)
    {
        for (int y = 0; y < field->rectHeight; y++)
        {
            rect.x = x * (field->rectWidth + field->spacing);
            rect.y = y * (field->rectHeight + field->spacing);
            SDL_RenderFillRect(GetRenderer(), &rect);
        }
    }
}
