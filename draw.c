#include "draw.h"
#include "super_header.h"
#include "ship.h"

#define FACTOR (1.0 / 2.0)

void DrawArrow(uint8_t x, uint8_t y, uint8_t facing)
{
    Field copy = GameField;
    SDL_Renderer *renderer = GameRenderer;
    const uint8_t w = copy.rectWidth * FACTOR;
    const uint8_t h = copy.rectHeight * FACTOR;
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

void OutlineTileBufferColor(uint8_t x, uint8_t y)
{
    SDL_Renderer *renderer = GameRenderer;
    if (x >= GameField.width || y >= GameField.height)
        return;
    const uint8_t w = GameField.rectWidth;
    const uint8_t h = GameField.rectHeight;
    const uint8_t s = GameField.spacing;
    SDL_Rect rect = {GameField.basePointX + x * (w + s) - s,
                     GameField.basePointY + y * (h + s) - s, w + 2 * s, h + 2 * s};
    SDL_RenderFillRect(renderer, &rect);
}

void OutlineTile(uint8_t x, uint8_t y)
{
    SDL_SetRenderDrawColor(GameRenderer, 0x00, 0x80, 0x80, 0xFF);
    OutlineTileBufferColor(x, y);
}

uint32_t *Uint8PixelsToUint32Pixels(const uint8_t *pointer, size_t width, size_t height)
{
    uint32_t *array = calloc(width * height, sizeof(uint32_t));
    if (array)
        for (size_t i = 0; i < width * height; i++)
        {
            uint32_t current = pointer[i];
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            array[i] = (current << 24) + (current << 16) + (current << 8) + 0xFF;
#else // Little endian
            array[i] = (0xFF << 24) + (current << 16) + (current << 8) + current;
#endif // SDL_BYTEORDER == SDL_BIG_ENDIAN
        }
    return array;
}
