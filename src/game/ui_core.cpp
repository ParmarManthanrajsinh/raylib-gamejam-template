#include "ui.h"
#include "assets.h"
#include "text_util.h"
#include "hex_grid.h"
#include <cmath>
#include <cstdio>
#include <raylib.h>


static Rectangle GetMenuButtonRectRaw()
{
    return { 576.0f, 8.0f, 130.0f, 24.0f };
}

Rectangle GetMenuButtonRect()
{
    return GetMenuButtonRectRaw();
}


bool CheckMenuButtonClick(Vector2 mouse_pos)
{
    return CheckCollisionPointRec(mouse_pos, GetMenuButtonRectRaw());
}

void DrawMenuButton(float anim_time)
{
    Font font = GetGameFont();
    Rectangle r = GetMenuButtonRectRaw();
    bool hovered = CheckCollisionPointRec(GetMousePosition(), r);

    Color bg = hovered ? Color{ 30, 40, 80, 255 } : Color{ 15, 20, 40, 255 };
    Color border = hovered ? Color{ 0, 200, 255, 255 } : Color{ 40, 80, 120, 255 };

    if (hovered)
    {
        float pulse = 0.5f + 0.5f * sinf(anim_time * 4.0f);
        DrawRectangleRounded({ r.x - 2, r.y - 2, r.width + 4, r.height + 4 }, 0.3f, 6, ColorAlpha(SKYBLUE, 0.15f * pulse));
    }

    DrawRectangleRounded(r, 0.3f, 6, bg);
    DrawRectangleRoundedLines(r, 0.3f, 6, border);

    Vector2 text_size = MeasureTextEx(font, "MENU", 14.0f, 1.0f);
    float tx = r.x + (r.width - text_size.x) / 2.0f;
    float ty = r.y + (r.height - text_size.y) / 2.0f;
    DrawTextShadowed(font, "MENU", static_cast<int>(tx), static_cast<int>(ty), 14, hovered ? WHITE : Color{ 150, 180, 220, 255 });
}



Rectangle GetClearButtonRect()
{
    float btn_w = 68;
    float spacing = 4;
    float total_gate = 7 * btn_w + (7 - 1) * spacing; // GATE_COUNT is 7
    float total = total_gate + spacing + 90;
    float start_x = (720 - total) / 2;
    return {start_x + total_gate + spacing, 650.0f + 12.0f, 90, 32};
}

