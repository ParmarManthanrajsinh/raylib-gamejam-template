#pragma once
#include <raylib.h>

inline void DrawTextShadowed(Font font, const char* text, int x, int y,
                             int font_size, Color color)
{
    Vector2 shadow = {static_cast<float>(x) + 1, static_cast<float>(y) + 1};
    Vector2 pos = {static_cast<float>(x), static_cast<float>(y)};
    DrawTextEx(font, text, shadow, static_cast<float>(font_size), 1,
               ColorAlpha(BLACK, 0.4f));
    DrawTextEx(font, text, pos, static_cast<float>(font_size), 1, color);
}

inline void DrawTextShadowedV(Font font, const char* text, Vector2 pos,
                              int font_size, Color color)
{
    DrawTextEx(font, text, {pos.x + 1, pos.y + 1},
               static_cast<float>(font_size), 1, ColorAlpha(BLACK, 0.4f));
    DrawTextEx(font, text, pos, static_cast<float>(font_size), 1, color);
}

inline void DrawTextCentered(Font font, const char* text, Rectangle bounds,
                             int font_size, Color color)
{
    Vector2 size = MeasureTextEx(font, text, static_cast<float>(font_size), 1);
    int x = static_cast<int>(bounds.x + (bounds.width - size.x) / 2);
    int y = static_cast<int>(bounds.y + (bounds.height - size.y) / 2);
    DrawTextShadowed(font, text, x, y, font_size, color);
}
