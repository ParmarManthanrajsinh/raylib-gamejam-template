#include "menu.h"
#include "assets.h"
#include "hex_grid.h"
#include "text_util.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <raylib.h>

namespace
{
    const char* funny_taglines[] =
    {
        "Where logic gates come to party",
        "Now with 100% more AND-ing",
        "0x48 0x45 0x58 0x46 0x4F 0x52 0x47 0x45",
        "Beats touching grass",
        "Your CPU is jealous",
        "Not a to-do list app",
        "Finally, a use for boolean algebra",
        "Hex and the City",
        "Gate Expectations",
        "The Gatefather",
        "Circuit of Dreams",
        "Wired and Dangerous",
    };

    int current_tagline = -1;

    int GetRandomTagline()
    {
        static int last = -1;
        int idx = GetRandomValue(0, 11);
        while (idx == last) idx = GetRandomValue(0, 11);
        last = idx;
        return idx;
    }

    void DrawHexButton
    (
        Rectangle bounds,
        const char* text,
        bool hovered,
        float anim_time,
        Color base_color
    )
    {
        float pulse = hovered ? 0.8f + 0.2f * sinf(anim_time * 6.0f) : 1.0f;
        Color border = ColorAlpha(base_color, 0.8f * pulse);
        Color text_color = hovered ? WHITE : ColorAlpha(base_color, 0.9f);

        if (hovered)
        {
            DrawRectangleRounded
            (
                {bounds.x - 3, bounds.y - 3, bounds.width + 6, bounds.height + 6},
                0.3f, 8, ColorAlpha(base_color, 0.15f)
            );
        }

        DrawRectangleRounded(bounds, 0.3f, 8, ColorAlpha({5, 10, 25, 255}, 0.9f));
        DrawRectangleRoundedLines(bounds, 0.3f, 8, border);

        Font font = GetGameFont();
        Vector2 text_size = MeasureTextEx(font, text, 20.0f, 1.0f);
        float tx = bounds.x + (bounds.width - text_size.x) / 2.0f;
        float ty = bounds.y + (bounds.height - text_size.y) / 2.0f;
        DrawTextShadowed(font, text, static_cast<int>(tx), static_cast<int>(ty), 20, text_color);
    }

    void DrawMenuBackground(float anim_time)
    {
        DrawRectangleGradientV
        (
            0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
            {5, 8, 20, 255}, {12, 18, 38, 255}
        );

        float bg_offset = anim_time * 6.0f;
        for (int y = -20; y < 750; y += 40)
        {
            for (int x = -20; x < 750; x += 35)
            {
                float x_off = (y % 80 == 0) ? 0 : 17.5f;
                float wave = sinf(anim_time * 0.8f + x * 0.01f + y * 0.01f) * 0.3f + 0.7f;
                DrawHexOutline
                (
                    {x + x_off - bg_offset, static_cast<float>(y)},
                    16.0f, 1.0f,
                    {30, 45, 80, static_cast<unsigned char>(20 * wave)}
                );
            }
        }

        float time = GetTime();
        for (int i = 0; i < 50; i++)
        {
            float px = fmodf(static_cast<float>(i * 137) + time * 10.0f * (1.0f + (i % 3) * 0.5f), 720.0f);
            float py = fmodf(static_cast<float>(i * 93) - time * 5.0f * (1.0f + (i % 5) * 0.2f) + 720.0f, 720.0f);
            DrawCircleV
            (
                {px, py},
                (i % 2 == 0) ? 1.5f : 0.8f,
                {0, 200, 255, static_cast<unsigned char>(10 + (i % 20))}
            );
        }
    }

    void DrawTitleHexFrame(Vector2 center, float size, float anim_time)
    {
        float rotation = anim_time * 8.0f;
        float pulse = 0.6f + 0.4f * sinf(anim_time * 2.0f);

        for (int layer = 3; layer >= 0; layer--)
        {
            float r = size + layer * 12.0f;
            Color c = {0, 200, 255, static_cast<unsigned char>(15 * pulse * (1.0f - layer * 0.2f))};
            Vector2 v[6];
            for (int i = 0; i < 6; i++)
            {
                float a = rotation + (PI / 3.0f) * i;
                v[i] = {center.x + r * cosf(a), center.y + r * sinf(a)};
            }
            for (int i = 0; i < 6; i++)
            {
                int next = (i + 1) % 6;
                DrawLineEx(v[i], v[next], 2.0f, c);
            }
        }

        Vector2 v[6];
        for (int i = 0; i < 6; i++)
        {
            float a = rotation + (PI / 3.0f) * i;
            v[i] = {center.x + size * cosf(a), center.y + size * sinf(a)};
        }
        for (int i = 0; i < 6; i++)
        {
            int next = (i + 1) % 6;
            Color line_color = ColorAlpha({0, 255, 255, 255}, 0.6f * pulse);
            DrawLineEx(v[i], v[next], 2.5f, line_color);
            DrawCircleV(v[i], 3.0f, ColorAlpha({0, 255, 255, 255}, 0.8f * pulse));
        }

        for (int i = 0; i < 6; i++)
        {
            int next = (i + 1) % 6;
            Vector2 mid = {(v[i].x + v[next].x) / 2.0f, (v[i].y + v[next].y) / 2.0f};
            float trace_pulse = 0.3f + 0.3f * sinf(anim_time * 3.0f + i * 1.0f);
            Color trace_c = ColorAlpha({0, 180, 255, 255}, trace_pulse * 0.4f);

            Vector2 dir = {mid.x - center.x, mid.y - center.y};
            float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
            if (len > 0.1f)
            {
                dir.x /= len;
                dir.y /= len;
            }
            Vector2 end = {mid.x + dir.x * 80.0f, mid.y + dir.y * 80.0f};
            DrawLineEx(mid, end, 1.0f, trace_c);
            DrawCircleV(end, 2.0f, trace_c);
        }
    }

    void DrawAnimatedTitle(Font font, float anim_time, float center_x, float title_y)
    {
        const char* title = "HEXFORGE";
        int title_len = static_cast<int>(strlen(title));
        float char_spacing = 4.0f;

        Vector2 total_size = MeasureTextEx(font, title, 72.0f, char_spacing);
        float start_x = center_x - total_size.x / 2.0f;

        float cursor_x = start_x;
        for (int i = 0; i < title_len; i++)
        {
            char ch[2] = {title[i], '\0'};
            Vector2 char_size = MeasureTextEx(font, ch, 72.0f, char_spacing);

            float delay = i * 0.08f;
            float age = anim_time - delay;

            float scale = 1.0f;
            float alpha = 1.0f;
            if (age < 0.4f && age >= 0.0f)
            {
                float t = age / 0.4f;
                scale = 0.3f + 0.7f * t * t;
                alpha = t;
            }
            else if (age < 0.0f)
            {
                scale = 0.3f;
                alpha = 0.0f;
            }

            float wobble = sinf(anim_time * 2.5f + i * 0.8f) * 3.0f * (1.0f - fminf(fmaxf(age - 0.4f, 0.0f) / 0.3f, 1.0f));

            float draw_x = cursor_x;
            float draw_y = title_y + wobble;

            Color shadow_c = ColorAlpha(BLACK, 0.5f * alpha);
            DrawTextEx(font, ch, {draw_x + 2, draw_y + 2}, 72.0f * scale, char_spacing, shadow_c);

            Color char_color;
            if (i < 3)
            {
                char_color = ColorAlpha({0, 255, 255, 255}, 255 * alpha);
            }
            else
            {
                float glow_pulse = 0.7f + 0.3f * sinf(anim_time * 3.0f + i * 0.5f);
                char_color = ColorAlpha({255, 255, 255, 255}, 255 * alpha * glow_pulse);
            }
            DrawTextEx(font, ch, {draw_x, draw_y}, 72.0f * scale, char_spacing, char_color);

            if (i < 3 && alpha > 0.5f)
            {
                Color glow = ColorAlpha({0, 255, 255, 255}, 30.0f * alpha);
                for (int r = 20; r > 0; r -= 5)
                {
                    DrawCircleV
                    (
                        {draw_x + char_size.x / 2.0f, draw_y + 36.0f},
                        static_cast<float>(r),
                        ColorAlpha(glow, static_cast<float>(r) / 20.0f * 0.3f)
                    );
                }
            }

            cursor_x += char_size.x + char_spacing;
        }
    }
}

GameState UpdateTitleScreen(float anim_time)
{
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    {
        return GameState::PLAYING;
    }
    if (IsKeyPressed(KEY_H))
    {
        return GameState::HOW_TO_PLAY;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mpos = GetMousePosition();
        float btn_w = 280.0f;
        float btn_h = 48.0f;
        float btn_x = (SCREEN_WIDTH - btn_w) / 2.0f;

        Rectangle play_btn = {btn_x, 460, btn_w, btn_h};
        Rectangle howto_btn = {btn_x, 530, btn_w, btn_h};

        if (CheckCollisionPointRec(mpos, play_btn)) return GameState::PLAYING;
        if (CheckCollisionPointRec(mpos, howto_btn)) return GameState::HOW_TO_PLAY;
    }

    return GameState::TITLE_SCREEN;
}

GameState UpdateHowToPlay(float anim_time)
{
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_H))
    {
        return GameState::TITLE_SCREEN;
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    {
        return GameState::PLAYING;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mpos = GetMousePosition();
        float btn_w = 280.0f;
        float btn_h = 44.0f;
        float btn_x = (SCREEN_WIDTH - btn_w) / 2.0f;

        Rectangle back_btn = {btn_x, 660, btn_w, btn_h};

        if (CheckCollisionPointRec(mpos, back_btn)) return GameState::TITLE_SCREEN;
    }

    return GameState::HOW_TO_PLAY;
}

void DrawTitleScreen(float anim_time)
{
    ClearBackground({13, 13, 26, 255});

    DrawMenuBackground(anim_time);

    Font font = GetGameFont();

    Vector2 title_center = {SCREEN_WIDTH / 2.0f, 160.0f};
    DrawTitleHexFrame(title_center, 140.0f, anim_time);

    DrawAnimatedTitle(font, anim_time, SCREEN_WIDTH / 2.0f, 126.0f);

    float subtitle_y = 215.0f;
    const char* subtitle = "Build logic circuits on a hex grid";
    Vector2 sub_size = MeasureTextEx(font, subtitle, 16.0f, 1.0f);
    float sub_alpha = 0.5f + 0.3f * sinf(anim_time * 2.0f);
    DrawTextShadowed
    (
        font, subtitle,
        static_cast<int>((SCREEN_WIDTH - sub_size.x) / 2.0f),
        static_cast<int>(subtitle_y),
        16,
        ColorAlpha({0, 200, 255, 255}, sub_alpha)
    );

    float line_width = 440.0f;
    float line_x = (SCREEN_WIDTH - line_width) / 2.0f;
    float line_y = 255.0f;
    float line_pulse = 0.4f + 0.3f * sinf(anim_time * 1.5f);
    DrawLineEx
    (
        {line_x, line_y}, {line_x + line_width, line_y},
        1.5f, ColorAlpha({0, 200, 255, 255}, line_pulse)
    );
    DrawCircleV({line_x, line_y}, 2.5f, ColorAlpha({0, 255, 255, 255}, line_pulse * 1.5f));
    DrawCircleV({line_x + line_width, line_y}, 2.5f, ColorAlpha({0, 255, 255, 255}, line_pulse * 1.5f));

    if (current_tagline < 0) current_tagline = GetRandomTagline();
    const char* tagline = funny_taglines[current_tagline];
    Vector2 tag_size = MeasureTextEx(font, tagline, 17.0f, 1.0f);
    float tag_alpha = 0.4f + 0.3f * sinf(anim_time * 2.0f);
    DrawTextShadowed
    (
        font, tagline,
        static_cast<int>((SCREEN_WIDTH - tag_size.x) / 2.0f),
        275,
        17,
        ColorAlpha({0, 200, 255, 255}, tag_alpha)
    );

    float btn_w = 280.0f;
    float btn_h = 48.0f;
    float btn_x = (SCREEN_WIDTH - btn_w) / 2.0f;

    Rectangle play_btn = {btn_x, 460, btn_w, btn_h};
    Rectangle howto_btn = {btn_x, 530, btn_w, btn_h};

    bool play_hovered = CheckCollisionPointRec(GetMousePosition(), play_btn);
    bool howto_hovered = CheckCollisionPointRec(GetMousePosition(), howto_btn);

    // float btn_fade = fminf(anim_time / 1.0f, 1.0f);
    DrawHexButton
    (
        {play_btn.x, play_btn.y, play_btn.width, play_btn.height},
        "PRESS ENTER TO PLAY",
        play_hovered, anim_time, {0, 255, 200, 255}
    );
    DrawHexButton
    (
        {howto_btn.x, howto_btn.y, howto_btn.width, howto_btn.height},
        "HOW TO PLAY  [H]",
        howto_hovered, anim_time, {0, 150, 255, 255}
    );

    DrawTextShadowed
    (
        font, "A raylib Game Jam project  |  hex + merge themes",
        155, 630, 12,
        ColorAlpha({100, 150, 200, 255}, 0.35f)
    );

    float hint_alpha = 0.25f + 0.2f * sinf(anim_time * 1.5f);
    DrawTextShadowed
    (
        font, "ENTER/SPACE to play   |   H for help   |   ESC to quit",
        115, 690, 11,
        ColorAlpha({150, 180, 220, 255}, hint_alpha)
    );
}

void DrawHowToPlay(float anim_time)
{
    ClearBackground({13, 13, 26, 255});

    DrawMenuBackground(anim_time);

    Font font = GetGameFont();

    float header_pulse = 0.7f + 0.3f * sinf(anim_time * 2.0f);
    DrawTextShadowed(font, "HOW TO PLAY", 205, 25, 40, ColorAlpha({0, 255, 255, 255}, header_pulse));

    const char* subtitle = "Your guide to building circuits with logic gates";
    Vector2 sub_size = MeasureTextEx(font, subtitle, 15.0f, 1.0f);
    DrawTextShadowed
    (
        font, subtitle,
        static_cast<int>((SCREEN_WIDTH - sub_size.x) / 2.0f),
        78, 15,
        ColorAlpha({136, 153, 187, 255}, 0.7f)
    );

    float line_y = 105.0f;
    DrawLineEx
    (
        {60, line_y}, {660, line_y},
        1.5f, ColorAlpha({0, 200, 255, 255}, 0.35f)
    );

    int y = 120;
    int line_h = 26;

    DrawTextShadowed(font, "THE GAME", 50, y, 22, {0, 255, 255, 255});
    DrawRectangle(42, y + 2, 4, 20, {0, 255, 255, 180});
    y += 32;

    DrawTextShadowed
    (
        font, "Match the TARGET hex value shown on the top bar.",
        60, y, 15, {220, 230, 245, 255}
    );
    y += line_h;

    DrawTextShadowed
    (
        font, "Toggle the 4 INPUT bits on the left side.",
        60, y, 15, {220, 230, 245, 255}
    );
    y += line_h;

    DrawTextShadowed
    (
        font, "Build a circuit of logic gates to transform them.",
        60, y, 15, {220, 230, 245, 255}
    );
    y += line_h;

    DrawTextShadowed
    (
        font, "When OUTPUT equals TARGET, you win!",
        60, y, 15, {0, 255, 150, 255}
    );
    y += line_h + 14;

    DrawTextShadowed(font, "STEP BY STEP", 50, y, 22, {255, 200, 50, 255});
    DrawRectangle(42, y + 2, 4, 20, {255, 200, 50, 180});
    y += 32;

    struct StepEntry
    {
        const char* step;
        const char* detail;
        Color detail_color;
    };

    StepEntry steps[] =
    {
        {"Click INPUT nodes or press 1-4 to set bits", "These are your starting ingredients.", {100, 160, 200, 255}},
        {"Select a GATE from the palette below", "AND, OR, NOT, XOR... pick your weapon.", {100, 160, 200, 255}},
        {"Click a HEX CELL on the grid to place it", "The grid is your canvas. Gates are your paint.", {100, 160, 200, 255}},
        {"Wire pins: click output pin then input pin", "One click to start, one click to connect.", {100, 160, 200, 255}},
        {"Watch the output update live - keep going!", "Repeat until output matches target. Easy, right?", {100, 160, 200, 255}},
    };

    for (int i = 0; i < 5; i++)
    {
        char num_buf[4];
        snprintf(num_buf, sizeof(num_buf), "%d.", i + 1);
        DrawTextShadowed(font, num_buf, 60, y, 16, {255, 200, 50, 255});
        DrawTextShadowed(font, steps[i].step, 85, y, 15, {220, 230, 245, 255});
        y += line_h - 2;
        DrawTextShadowed(font, steps[i].detail, 85, y, 13, steps[i].detail_color);
        y += line_h + 2;
    }

    y += 6;

    DrawTextShadowed(font, "CONTROLS", 50, y, 22, {0, 255, 150, 255});
    DrawRectangle(42, y + 2, 4, 20, {0, 255, 150, 180});
    y += 32;

    struct ControlEntry
    {
        const char* key;
        const char* action;
    };

    ControlEntry controls[] =
    {
        {"Left Click", "Place gate / Connect wire / Toggle input"},
        {"Right Click", "Delete a gate (no judgment)"},
        {"Click Input Pin", "Remove its connected wire"},
        {"Drag Gate", "Move it to a new hex cell"},
        {"R / T", "Random target / Cycle target"},
    };

    for (int i = 0; i < 5; i++)
    {
        DrawTextShadowed(font, controls[i].key, 60, y, 14, {0, 255, 200, 255});
        DrawTextShadowed(font, "=", 195, y, 14, ColorAlpha({136, 153, 187, 255}, 0.6f));
        DrawTextShadowed(font, controls[i].action, 215, y, 14, {200, 215, 235, 255});
        y += line_h;
    }

    y += 10;

    DrawTextShadowed(font, "GATE CHEAT SHEET", 50, y, 22, {255, 100, 200, 255});
    DrawRectangle(42, y + 2, 4, 20, {255, 100, 200, 180});
    y += 32;

    struct GateEntry
    {
        const char* name;
        const char* desc;
        Color color;
    };

    GateEntry gates_list[] =
    {
        {"AND",  "Both inputs = 1  -->  output 1", {0, 255, 128, 255}},
        {"OR",   "Either input = 1  -->  output 1", {0, 200, 255, 255}},
        {"NOT",  "Flips the bit: 1 --> 0, 0 --> 1", {255, 100, 100, 255}},
        {"XOR",  "Inputs are different  -->  output 1", {200, 100, 255, 255}},
        {"NAND", "AND but inverted", {255, 255, 100, 255}},
        {"NOR",  "OR but inverted", {255, 150, 50, 255}},
        {"XNOR", "Inputs are the same  -->  output 1", {150, 255, 255, 255}},
    };

    for (int i = 0; i < 7; i++)
    {
        DrawTextShadowed(font, gates_list[i].name, 60, y, 15, gates_list[i].color);
        DrawTextShadowed(font, gates_list[i].desc, 140, y, 14, {200, 215, 235, 230});
        y += line_h - 2;
    }

    y += 14;

    float btn_w = 280.0f;
    float btn_h = 44.0f;
    float btn_x = (SCREEN_WIDTH - btn_w) / 2.0f;
    Rectangle back_btn = {btn_x, 660, btn_w, btn_h};

    bool back_hovered = CheckCollisionPointRec(GetMousePosition(), back_btn);
    DrawHexButton(back_btn, "BACK TO TITLE  [ESC]", back_hovered, anim_time, {0, 150, 255, 255});

    float play_alpha = 0.3f + 0.2f * sinf(anim_time * 1.5f);
    DrawTextShadowed
    (
        font, "ENTER or SPACE to jump straight into the game!",
        155, 650, 11,
        ColorAlpha({150, 180, 220, 255}, play_alpha)
    );
}
