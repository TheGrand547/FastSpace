#include "draw.h"
#include "super_header.h"
#include "ship.h"

void DrawArrow(Uint8 x, Uint8 y, Uint8 facing)
{
    Field copy = *GetField();
    SDL_Renderer *renderer = GetRenderer();
    Uint8 w = copy.rectWidth * 1.0 / 2.0;
    Uint8 h = copy.rectHeight * 1.0 / 2.0;
    SDL_Point center = {copy.basePointX + (x * (copy.rectWidth + copy.spacing) + w),
                        copy.basePointX + (y * (copy.rectHeight + copy.spacing) + h)};
    SDL_Point points[3] = {center, center, center};
    points[0].x += FacingX(facing - 1) * w;
    points[0].y += FacingY(facing - 1) * h;
    points[1].x += FacingX(facing) * w;
    points[1].y += FacingY(facing) * h;
    points[2].x += FacingX(facing + 1) * w;
    points[2].y += FacingY(facing + 1) * h;
    SDL_SetRenderDrawColor(renderer, 0x00, 0x7F, 0xFF, 0xFF);
    SDL_RenderDrawLines(renderer, points, 3);
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
#endif
        }
    return array;
}
