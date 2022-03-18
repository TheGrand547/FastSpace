#include "bullet.h"
#include "super_header.h"

void DrawBullet(Bullet *bullet)
{
    if (!bullet)
        return;
    Field *field = GetField();
    Uint8 width = field->rectWidth * BULLET_X_SIZE;
    Uint8 height = field->rectHeight * BULLET_Y_SIZE;
    SDL_Rect rect = {field->basePointX + bullet->x *
                    (field->rectWidth + field->spacing) + width / 2 + 1,
                    field->basePointY + bullet->y *
                    (field->rectHeight + field->spacing) + height / 2 + 1,
                    width, height};
    SDL_SetRenderDrawColor(GetRenderer(), bullet->color.r,
                           bullet->color.g, bullet->color.b, bullet->color.a);
    SDL_RenderFillRect(GetRenderer(), &rect);

    SDL_SetRenderDrawColor(GetRenderer(), 0xFF, 0x00, 0xFF, 0xFF);
    /*
    SDL_Point point;
    point.x = field->basePointX + bullet->x * (field->rectWidth + field->spacing);// + field->rectWidth * 1.0 / 2.0;
    point.y = field->basePointY + bullet->y * (field->rectHeight + field->spacing);// + field->rectHeight * 1.0 / 2.0;;
    SDL_RenderDrawLine(GetRenderer(), point.x, point.y,
                       point.x + field->rectWidth * 1.0 / 2.0,
                       point.y + field->rectHeight * 1.0 / 2.0);
    */
}
