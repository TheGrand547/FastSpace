#include "field.h"
#include "super_header.h"

SDL_Rect GetTile(uint8_t x, uint8_t y)
{
    SDL_Rect rect = {GameField.basePointX + x *
                    (GameField.rectSize + GameField.spacing),
                     GameField.basePointX + y *
                    (GameField.rectSize + GameField.spacing),
                     GameField.rectSize, GameField.rectSize};
    return rect;
}

unsigned int IndexFromLocation(uint8_t x, uint8_t y)
{
    return (x + y * GameField.width) % NumTiles();
}

unsigned int NumTiles()
{
    return GameField.width * GameField.height;
}

static unsigned int windowX, windowY;

unsigned int WindowSizeX()
{
    // TODO: Figure out this 300 magic number
    return GameField.width * (GameField.rectSize + GameField.spacing)
            + GameField.spacing + GameField.basePointX + 300;
}

unsigned int WindowSizeY()
{
    return GameField.height * (GameField.rectSize + GameField.spacing)
            + GameField.spacing + GameField.basePointY;
}

void DrawField(Field *field)
{
    NULL_CHECK(field);
    // TODO: Fix this logic to incorporate the boundary of the shapes
    SDL_SetRenderDrawColor(GameRenderer, 0x00, 0x48, 0xCF, 0xFF);
    SDL_Rect temp = {field->basePointX, field->basePointY,
                    field->width * (field->rectSize + field->spacing) - field->spacing,
                    field->height * (field->rectSize + field->spacing) - field->spacing};
    SDL_RenderFillRect(GameRenderer, &temp);
    SDL_SetRenderDrawColor(GameRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_Rect rect = {0, 0, field->rectSize, field->rectSize};
    // TODO: Make this not the single least efficient thing in the world
    for (unsigned int x = 0; x < field->width; x++)
    {
        rect.x = field->basePointX +
                    x * (field->rectSize + field->spacing);
        for (unsigned int y = 0; y < field->height; y++)
        {
            rect.y = field->basePointY +
                        y * (field->rectSize + field->spacing);
            SDL_RenderFillRect(GameRenderer, &rect);
        }
    }
}

void SetupField()
{
    // TODO: Something regarding vertically rotated monitors
    /*
    GameField.height * (GameField.rectSize + GameField.spacing)
            + GameField.spacing + GameField.basePointY
    Want this to be equal to some constant C(the screen height)
    Lowercase -> variable, uppercase -> some constant
    let x = rectSize, A = number of vertical tiles, B = spacing, y = basePointY
    y is a dependent variable to ensure an exact match
    A * floor(x) + AB + B + y = C
    A * floor(x) + y = C - AB - B
    if (C - AB - B) / A => integer, x = that integer, y = 0
    other wise x = floor((C - AB - B) / A), y = C - AB - B - Ax
    (well technically y is that over 2 to ensure an even border but shut up)
    */
    int _dummy, _C;
    SDL_GetWindowSize(GameWindow, &_dummy, &_C);
    double C = (double) _C, A = (double) GameField.height, B = (double) GameField.spacing,
        AB = A * (B - 1);
    double equation = (C - AB - B) / A;
    if (FLOAT_EQUAL(equation, floor(equation)))
    {
        GameField.rectSize = (uint16_t) equation;
        GameField.basePointX = 0;
    }
    else
    {
        double difference = C - AB - B - A * floor(equation);
        GameField.rectSize = (uint16_t) floor(equation);
        GameField.basePointX = difference / 2.0f;
    }
    GameField.basePointY = GameField.basePointX;
}
