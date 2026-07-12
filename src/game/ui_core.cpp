#include "ui.h"
#include "audio.h"
#include "assets.h"
#include "text_util.h"
#include "hex_grid.h" // IWYU pragma: keep
#include <cmath>
#include <cstdio> // IWYU pragma: keep
#include <raylib.h>


static Rectangle GetMenuButtonRectRaw()
{
    return { 496.0f, 8.0f, 130.0f, 24.0f };
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
        DrawRectangleRounded
        (
            { r.x - 2, r.y - 2, r.width + 4, r.height + 4 },
            0.3f,
            6,
            ColorAlpha(SKYBLUE, 0.15f * pulse)
        );
    }

    DrawRectangleRounded(r, 0.3f, 6, bg);
    DrawRectangleRoundedLines(r, 0.3f, 6, border);

    Vector2 text_size = MeasureTextEx(font, "MENU [M]", 14.0f, 1.0f);
    float tx = r.x + (r.width - text_size.x) / 2.0f;
    float ty = r.y + (r.height - text_size.y) / 2.0f;
    DrawTextShadowed
    (
        font, "MENU [M]", static_cast<int>(tx), static_cast<int>(ty), 14,
        hovered ? WHITE : Color{ 150, 180, 220, 255 }
    );
}


static Rectangle GetMusicButtonRectRaw()
{
    return { 630.0f, 8.0f, 82.0f, 24.0f };
}

Rectangle GetMusicButtonRect()
{
    return GetMusicButtonRectRaw();
}

bool CheckMusicButtonClick(Vector2 mouse_pos)
{
    return CheckCollisionPointRec(mouse_pos, GetMusicButtonRectRaw());
}

void DrawMusicButton(float anim_time)
{
    Font font = GetGameFont();
    Rectangle r = GetMusicButtonRectRaw();
    bool hovered = CheckCollisionPointRec(GetMousePosition(), r);
    bool playing = IsMusicPlaying();

    Color accent = playing ? Color{ 0, 200, 180, 255 } : Color{ 120, 80, 80, 255 };
    Color bg = hovered ? Color{ 30, 40, 80, 255 } : Color{ 15, 20, 40, 255 };
    Color border = hovered ? accent : Color{ 40, 80, 120, 255 };

    if (hovered)
    {
        float pulse = 0.5f + 0.5f * sinf(anim_time * 4.0f);
        DrawRectangleRounded
        (
            { r.x - 2, r.y - 2, r.width + 4, r.height + 4 },
            0.3f,
            6,
            ColorAlpha(accent, 0.15f * pulse)
        );
    }

    DrawRectangleRounded(r, 0.3f, 6, bg);
    DrawRectangleRoundedLines(r, 0.3f, 6, border);

    const char* label = playing ? "SOUND [N]" : "MUTED [N]";
    Vector2 text_size = MeasureTextEx(font, label, 14.0f, 1.0f);
    float tx = r.x + (r.width - text_size.x) / 2.0f;
    float ty = r.y + (r.height - text_size.y) / 2.0f;
    Color text_col = playing
        ? (hovered ? WHITE : Color{ 150, 200, 190, 255 })
        : (hovered ? WHITE : Color{ 150, 100, 100, 255 });
    DrawTextShadowed(font, label, static_cast<int>(tx), static_cast<int>(ty), 14, text_col);
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
