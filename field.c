#include "field.h"
#include "super_header.h"

SDL_Rect GetTile(uint8_t x, uint8_t y)
{
    SDL_Rect rect = {GameField.basePointX + x *
                    (GameField.rectWidth + GameField.spacing),
                     GameField.basePointX + y *
                    (GameField.rectHeight + GameField.spacing),
                     GameField.rectWidth, GameField.rectHeight};
    return rect;
}

unsigned int WindowSizeX()
{
    return GameField.width * (GameField.rectWidth + GameField.spacing)
            - GameField.spacing + 300;
}

unsigned int WindowSizeY()
{
    return GameField.height * (GameField.rectHeight + GameField.spacing)
            - GameField.spacing;
}

void DrawField(Field *field)
{
    NULL_CHECK(field);
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
