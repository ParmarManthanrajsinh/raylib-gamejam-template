#pragma once
#include <raylib.h>

inline void DrawTextShadowed(Font font, const char* text, int x, int y, int fontSize, Color color) {
    Vector2 shadow = { (float)x + 1, (float)y + 1 };
    Vector2 pos = { (float)x, (float)y };
    DrawTextEx(font, text, shadow, (float)fontSize, 1, ColorAlpha(BLACK, 0.4f));
    DrawTextEx(font, text, pos, (float)fontSize, 1, color);
}

inline void DrawTextShadowedV(Font font, const char* text, Vector2 pos, int fontSize, Color color) {
    DrawTextEx(font, text, { pos.x + 1, pos.y + 1 }, (float)fontSize, 1, ColorAlpha(BLACK, 0.4f));
    DrawTextEx(font, text, pos, (float)fontSize, 1, color);
}

inline void DrawTextCentered(Font font, const char* text, Rectangle bounds, int fontSize, Color color) {
    Vector2 size = MeasureTextEx(font, text, (float)fontSize, 1);
    int x = (int)(bounds.x + (bounds.width - size.x) / 2);
    int y = (int)(bounds.y + (bounds.height - size.y) / 2);
    DrawTextShadowed(font, text, x, y, fontSize, color);
}
