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

unsigned int IndexFromLocation(uint8_t x, uint8_t y)
{
    return x + y * GameField.width;
}

unsigned int NumTiles()
{
    return GameField.width * GameField.height;
}

unsigned int WindowSizeX()
{
    return GameField.width * (GameField.rectWidth + GameField.spacing)
            + GameField.spacing + 300;
}

unsigned int WindowSizeY()
{
    return GameField.height * (GameField.rectHeight + GameField.spacing)
            + GameField.spacing;
}

void DrawField(Field *field)
{
    NULL_CHECK(field);
    // TODO: Fix this logic to incorporate the boundary of the shapes
    SDL_SetRenderDrawColor(GameRenderer, 0x00, 0x48, 0xCF, 0xFF);
    SDL_Rect temp = {field->basePointX, field->basePointY,
                    field->width * (field->rectWidth + field->spacing) - field->spacing,
                    field->height * (field->rectHeight + field->spacing) - field->spacing};
    SDL_RenderFillRect(GameRenderer, &temp);
    SDL_SetRenderDrawColor(GameRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_Rect rect = {0, 0, field->rectWidth, field->rectHeight};
    for (unsigned int x = 0; x < field->width; x++)
    {
        rect.x = field->basePointX + x * (field->rectWidth + field->spacing);
        for (unsigned int y = 0; y < field->height; y++)
        {
            rect.y = field->basePointY + y * (field->rectHeight + field->spacing);
            SDL_RenderFillRect(GameRenderer, &rect);
        }
    }
}
