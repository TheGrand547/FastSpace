#include "draw.h"
#include "super_header.h"
#include "ship.h"

#define FACTOR (1.0 / 2.0)

void DrawArrow(Uint8 x, Uint8 y, Uint8 facing)
{
    Field copy = *GetField();
    SDL_Renderer *renderer = GetRenderer();
    Uint8 w = copy.rectWidth * 1.0 / 2.0;
    Uint8 h = copy.rectHeight * 1.0 / 2.0;
    SDL_FPoint center = {copy.basePointX + (x * (copy.rectWidth + copy.spacing) + w),
                        copy.basePointX + (y * (copy.rectHeight + copy.spacing) + h)};
    Color color = {0x00, 0x7F, 0xFF, 0xFF};
    SDL_Vertex points[3] = {{center, color, {0, 0}}, {center, color, {0, 0}}, {center, color, {0, 0}}};
    points[0].position.x += FacingX(facing - 1) * w * FACTOR;
    points[0].position.y += FacingY(facing - 1) * h * FACTOR;
    points[1].position.x += FacingX(facing) * w;
    points[1].position.y += FacingY(facing) * h;
    points[1].color.b = 0x00;
    points[2].position.x += FacingX(facing + 1) * w * FACTOR;
    points[2].position.y += FacingY(facing + 1) * h * FACTOR;
    SDL_RenderGeometry(renderer, NULL, points, 3, NULL, 0);
}

void OutlineTile(Uint8 x, Uint8 y)
{
    Field copy = *GetField();
    SDL_Renderer *renderer = GetRenderer();
    if (x >= copy.width || y >= copy.height)
        return;
    Uint8 w = copy.rectWidth;
    Uint8 h = copy.rectHeight;
    Uint8 s = copy.spacing;
    SDL_Rect rect = {copy.basePointX + x * (w + s) - s,
                    copy.basePointY + y * (h + s) - s, w + 2 * s, h + 2 * s};
    SDL_SetRenderDrawColor(renderer, 0x00, 0x80, 0x80, 0xFF);
    SDL_RenderFillRect(renderer, &rect);
}

Uint32 *Uint8PixelsToUint32Pixels(Uint8 *pointer, int width, int height)
{
    Uint32 *array = calloc(width * height, sizeof(Uint32));
    if (array)
        for (int i = 0; i < width * height; i++)
        {
            Uint32 current = pointer[i];
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            array[i] = (current << 24) + (current << 16) + (current << 8) + 0xFF;
#else // Little endian
            array[i] = (0xFF << 24) + (current << 16) + (current << 8) + current;
#endif // SDL_BYTEORDER == SDL_BIG_ENDIAN
        }
    return array;
}
