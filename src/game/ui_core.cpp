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
    return { 582.0f, 8.0f, 130.0f, 24.0f };
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
    return { 8.0f, 8.0f, 82.0f, 24.0f };
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

    float cx = r.x + 14.0f;
    float cy = r.y + r.height / 2.0f;

    Color icon_col = playing
        ? (hovered ? WHITE : Color{ 0, 220, 200, 255 })
        : (hovered ? WHITE : Color{ 180, 100, 100, 255 });

    DrawRectangle
    (
        static_cast<int>(cx - 1.0f),
        static_cast<int>(cy - 4.0f),
        5, 8, icon_col
    );

    Vector2 cone_p1 = {cx + 4.0f, cy - 6.0f};
    Vector2 cone_p2 = {cx + 4.0f, cy + 6.0f};
    Vector2 cone_p3 = {cx + 10.0f, cy + 9.0f};
    Vector2 cone_p4 = {cx + 10.0f, cy - 9.0f};
    DrawTriangle(cone_p1, cone_p3, cone_p2, icon_col);
    DrawTriangle(cone_p2, cone_p4, cone_p1, icon_col);

    if (playing)
    {
        float wave_pulse = 0.5f + 0.5f * sinf(anim_time * 6.0f);
        Color wave_col = icon_col;
        wave_col.a = static_cast<unsigned char>(180 * wave_pulse);

        for (int i = 1; i <= 2; i++)
        {
            float r_val = static_cast<float>(i) * 5.0f;
            DrawCircleSectorLines
            (
                {cx + 10.0f, cy},
                r_val,
                -35.0f, 35.0f,
                12, wave_col
            );
        }
    }
    else
    {
        float slash_len = 8.0f;
        DrawLineEx
        (
            {cx + 6.0f - slash_len, cy - slash_len},
            {cx + 6.0f + slash_len, cy + slash_len},
            2.5f, Color{ 200, 80, 80, 255 }
        );
    }

    const char* label = playing ? "[N]" : "[N]";
    Vector2 text_size = MeasureTextEx(font, label, 14.0f, 1.0f);
    float tx = r.x + r.width - text_size.x - 10.0f;
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
