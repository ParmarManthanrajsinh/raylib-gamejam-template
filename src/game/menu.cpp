#include "menu.h"
#include "assets.h"
#include "hex_grid.h"
#include "text_util.h"
#include "gates.h"
#include <cmath>
#include <array>
#include <cstring>
#include <raylib.h>

namespace
{
    const char* funny_taglines[11] =
    {
        "Where logic gates come to party",
        "Now with 100% more AND-ing",
        "0x48 0x45 0x58 0x46 0x4F 0x52 0x47 0x45",
        "Beats touching grass",
        "Your CPU is jealous",
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

            // Animated circuit border
            float border_anim = fmodf(anim_time * 200.0f, bounds.width + bounds.height);
            if (border_anim < bounds.width)
            {
                DrawLineEx
                (
                    {bounds.x + border_anim, bounds.y},
                    {bounds.x + border_anim + 20, bounds.y},
                    3.0f, base_color
                );
                DrawLineEx
                (
                    {bounds.x + bounds.width - border_anim, bounds.y + bounds.height},
                    {bounds.x + bounds.width - border_anim - 20, bounds.y + bounds.height},
                    3.0f, base_color
                );
            }
            else
            {
                float h_anim = border_anim - bounds.width;
                DrawLineEx
                (
                    {bounds.x + bounds.width, bounds.y + h_anim},
                    {bounds.x + bounds.width, bounds.y + h_anim + 20},
                    3.0f, base_color
                );
                DrawLineEx
                (
                    {bounds.x, bounds.y + bounds.height - h_anim},
                    {bounds.x, bounds.y + bounds.height - h_anim - 20},
                    3.0f, base_color
                );
            }
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

                // Data pulses on grid
                float pulse = 0.0f;
                if (fmodf(anim_time * 50.0f + x + y, 400.0f) < 10.0f) pulse = 1.0f;

                DrawHexOutline
                (
                    {x + x_off - bg_offset, static_cast<float>(y)},
                    16.0f, 1.0f,
                    {
                        30,
                        static_cast<unsigned char>(45 + pulse*150),
                        static_cast<unsigned char>(80 + pulse*175),
                        static_cast<unsigned char>(20 * wave + pulse*100)
                    }
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

        Vector2 mpos = GetMousePosition();
        for (int i = 0; i < 12; i++)
        {
            float px = fmodf
            (
                static_cast<float>(i * 317) + time * 15.0f * (1.0f + (i % 2) * 0.5f),
                900.0f
            ) - 100.0f;

            float py = fmodf
            (
                static_cast<float>(i * 253) - time * 10.0f * (1.0f + (i % 3) * 0.3f) + 900.0f,
                900.0f
            ) - 100.0f;

            // Mouse dodge logic
            float dx = px - mpos.x;
            float dy = py - mpos.y;
            float dist = sqrtf(dx*dx + dy*dy);
            if (dist < 150.0f && dist > 1.0f)
            {
                float force = (150.0f - dist) / 150.0f;
                px += (dx / dist) * force * 50.0f;
                py += (dy / dist) * force * 50.0f;
                // Draw connecting wire
                DrawLineEx({px, py}, mpos, 1.5f * force, ColorAlpha({0, 255, 255, 255}, force * 0.5f));
            }

            t_Gate dummy{};
            dummy.type = static_cast<GateType>(i % 7);
            DrawGateShape(dummy, px, py, 65.0f, 65.0f * 1.732f * 0.75f, 0, (dist < 150.0f) ? 0.6f : 0.15f);
        }
    }

    void DrawTitleHexFrame(Vector2 center, float size, float anim_time)
    {
        float rotation = anim_time * 2.0f;
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

            float wobble = sinf
            (
                anim_time * 2.5f + i * 0.8f
            ) * 3.0f * (1.0f - fminf(fmaxf(age - 0.4f, 0.0f) / 0.3f, 1.0f));

            float draw_x = cursor_x;
            float draw_y = title_y + wobble;

            Color shadow_c = ColorAlpha(BLACK, 0.5f * alpha);
            DrawTextEx(font, ch, {draw_x + 2, draw_y + 2}, 72.0f * scale, char_spacing, shadow_c);

            Color char_color;
            bool is_glitch = GetRandomValue(0, 100) > 97;

            if (i < 3)
            {
                char_color = ColorAlpha({0, 255, 255, 255}, 255 * alpha);
            }
            else
            {
                float glow_pulse = 0.7f + 0.3f * sinf(anim_time * 3.0f + i * 0.5f);
                char_color = ColorAlpha({255, 255, 255, 255}, 255 * alpha * glow_pulse);
            }

            if (is_glitch)
            {
                float gx = GetRandomValue(-4, 4);
                DrawTextEx(font, ch, {draw_x + gx, draw_y}, 72.0f * scale, char_spacing, RED);
                DrawTextEx(font, ch, {draw_x - gx, draw_y}, 72.0f * scale, char_spacing, BLUE);
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

    void DrawCRTAndMouseTrail()
    {
        // CRT Scanlines
        for (int y = 0; y < SCREEN_HEIGHT; y += 4)
        {
            DrawLine(0, y, SCREEN_WIDTH, y, ColorAlpha(BLACK, 0.3f));
        }

        // Edge Vignette
        for (int i = 0; i < 40; i++)
        {
            DrawRectangleLines(i, i, SCREEN_WIDTH - i*2, SCREEN_HEIGHT - i*2, ColorAlpha(BLACK, 0.05f));
        }

        // Mouse Trail
        static Vector2 trail[15];
        Vector2 mpos = GetMousePosition();

        // Shift trail
        for (int i = 14; i > 0; i--) trail[i] = trail[i-1];
        trail[0] = mpos;

        for (int i = 0; i < 14; i++)
        {
            if (trail[i].x != 0 && trail[i+1].x != 0)
            {
                float alpha = 1.0f - (static_cast<float>(i) / 14.0f);
                DrawLineEx(trail[i], trail[i+1], 2.0f * alpha, ColorAlpha({0, 255, 200, 255}, alpha));
                DrawCircleV(trail[i], 1.5f * alpha, ColorAlpha({0, 255, 255, 255}, alpha));
            }
        }
    }
}

GameState UpdateTitleScreen(float)
{
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    {
        return GameState::TITLE_TO_PLAY_TRANSITION;
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

        if (CheckCollisionPointRec(mpos, play_btn))
            return GameState::TITLE_TO_PLAY_TRANSITION;
        if (CheckCollisionPointRec(mpos, howto_btn))
            return GameState::TITLE_TO_HOW_TO_PLAY_TRANSITION;
    }

    return GameState::TITLE_SCREEN;
}

GameState UpdateHowToPlay(float)
{
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_H))
    {
        return GameState::HOW_TO_PLAY_TO_TITLE_TRANSITION;
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    {
        return GameState::HOW_TO_PLAY_TO_PLAY_TRANSITION;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mpos = GetMousePosition();
        float btn_w = 280.0f;
        float btn_h = 44.0f;
        float btn_x = (SCREEN_WIDTH - btn_w) / 2.0f;

        Rectangle back_btn = {btn_x, 660, btn_w, btn_h};

        if (CheckCollisionPointRec(mpos, back_btn))
            return GameState::HOW_TO_PLAY_TO_TITLE_TRANSITION;
    }

    return GameState::HOW_TO_PLAY;
}

void DrawTitleScreen(float anim_time, float transition_time)
{
    ClearBackground({13, 13, 26, 255});

    Vector2 mpos = GetMousePosition();
    float ox = (SCREEN_WIDTH / 2.0f - mpos.x) * 0.03f;
    float oy = (SCREEN_HEIGHT / 2.0f - mpos.y) * 0.03f;
    Camera2D cam = { {0,0}, {ox, oy}, 0.0f, 1.0f };

    BeginMode2D(cam);
    DrawMenuBackground(anim_time);

    Font font = GetGameFont();

    Vector2 title_center = {SCREEN_WIDTH / 2.0f, 160.0f};
    DrawTitleHexFrame(title_center, 140.0f, anim_time);

    DrawAnimatedTitle(font, anim_time, SCREEN_WIDTH / 2.0f, 126.0f);
    EndMode2D();

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
        210, 630, 12,
        ColorAlpha({100, 150, 200, 255}, 0.35f)
    );

    if (transition_time > 0.0f)
    {
        float t = transition_time / 0.8f;
        if (t > 1.0f) t = 1.0f;

        // Circular wipe effect
        float max_radius = 1200.0f;
        float radius = t * max_radius;

        // Draw expanding hole by drawing thick circles outside
        for (int i = 0; i < 40; i++)
        {
            DrawCircleLines(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, radius + i, {13, 13, 26, 255});
        }

        // Fade to game background
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ColorAlpha({13, 13, 26, 255}, t * t));
    }

    DrawCRTAndMouseTrail();
}

void DrawHowToPlay(float anim_time, float transition_time)
{
    ClearBackground({13, 13, 26, 255});

    for (int i = 0; i < 40; i++)
    {
        DrawRectangleLines(i, i, SCREEN_WIDTH - i*2, SCREEN_HEIGHT - i*2, ColorAlpha(SKYBLUE, 0.02f));
    }

    for (int i = 0; i < 6; i++)
    {
        float bg_x = fmodf
        (
            static_cast<float>(i * 412) + anim_time * 8.0f * (1.0f + (i % 2) * 0.5f), 900.0f
        ) - 100.0f;

        float bg_y = fmodf
        (
            static_cast<float>(i * 333) - anim_time * 5.0f * (1.0f + (i % 3) * 0.3f) + 900.0f,
            900.0f
        ) - 100.0f;

        t_Gate dummy{};
        dummy.type = static_cast<GateType>(i % 7);
        DrawGateShape(dummy, bg_x, bg_y, 100.0f, 100.0f * 1.732f * 0.75f, 0, 0.03f);
    }

    Font font = GetGameFont();
    Vector2 mouse_pos = GetMousePosition();
    float center_x = SCREEN_WIDTH / 2.0f;

    float header_pulse = 0.7f + 0.3f * sinf(anim_time * 2.0f);
    const char* title_text = "HOW TO PLAY";
    Vector2 title_size = MeasureTextEx(font, title_text, 40, 1);
    DrawTextShadowed
    (
        font, title_text,
        (SCREEN_WIDTH - title_size.x) / 2.0f, 25, 40,
        ColorAlpha({0, 255, 255, 255}, header_pulse)
    );

    const char* subtitle = "Your guide to building circuits with logic gates";
    Vector2 sub_size = MeasureTextEx(font, subtitle, 15.0f, 1.0f);
    DrawTextShadowed
    (
        font, subtitle,
        (SCREEN_WIDTH - sub_size.x) / 2.0f, 78, 15,
        ColorAlpha({136, 153, 187, 255}, 0.7f)
    );

    DrawLineEx({60, 105}, {660, 105}, 1.5f, ColorAlpha({0, 200, 255, 255}, 0.35f));
    int y = 120;

    const char* goal_header = "THE GOAL";
    Vector2 goal_header_size = MeasureTextEx(font, goal_header, 22, 1);
    float goal_header_x = (SCREEN_WIDTH - goal_header_size.x) / 2.0f;
    DrawTextShadowed(font, goal_header, goal_header_x, y, 22, {0, 255, 255, 255});
    DrawPoly({goal_header_x - 15.0f, y + 12.0f}, 6, 8.0f, 90.0f, ColorAlpha({0, 255, 255, 255}, 0.8f));
    DrawPolyLinesEx({goal_header_x - 15.0f, y + 12.0f}, 6, 8.0f, 90.0f, 2.0f, {0, 255, 255, 255});
    y += 32;

    const char* goal_text = "Transform INPUT bits into the TARGET hex value using Logic Gates.";
    Vector2 goal_text_size = MeasureTextEx(font, goal_text, 15, 1);
    DrawTextShadowed(font, goal_text, (SCREEN_WIDTH - goal_text_size.x) / 2.0f, y, 15, {220, 230, 245, 255});
    y += 38;

    const char* try_header = "TRY A GATE";
    Vector2 try_header_size = MeasureTextEx(font, try_header, 22, 1);
    float try_header_x = (SCREEN_WIDTH - try_header_size.x) / 2.0f;
    DrawTextShadowed(font, try_header, try_header_x, y, 22, {0, 255, 255, 255});
    DrawPoly({try_header_x - 15.0f, y + 12.0f}, 6, 8.0f, 90.0f, ColorAlpha({0, 255, 255, 255}, 0.8f));
    DrawPolyLinesEx({try_header_x - 15.0f, y + 12.0f}, 6, 8.0f, 90.0f, 2.0f, {0, 255, 255, 255});
    y += 34;

    static int demo_in1 = 1;
    static int demo_in2 = 1;
    static int selected_gate = 0;

    float demo_center_y = y + 38;
    float input_x = center_x - 210;
    float gate_width = 65;
    float gate_height = 65 * 1.732f * 0.75f;
    float gate_draw_x = center_x - gate_width / 2.0f;
    float gate_draw_y = demo_center_y - gate_height / 2.0f;
    float output_x = center_x + gate_width / 2.0f + 50;

    GateType current_gate = static_cast<GateType>(selected_gate);
    bool single_input = GetGateInputCount(current_gate) == 1;

    Rectangle input_rect1 = {input_x, demo_center_y - 28, 44, 24};
    Rectangle input_rect2 = {input_x, demo_center_y + 12, 44, 24};
    bool hover_in1 = CheckCollisionPointRec(mouse_pos, input_rect1);
    bool hover_in2 = CheckCollisionPointRec(mouse_pos, input_rect2);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        if (hover_in1)
        {
            demo_in1 ^= 1;
        }
        if (hover_in2 && !single_input)
        {
            demo_in2 ^= 1;
        }
    }

    int demo_out = 0;
    switch (current_gate)
    {
        case GateType::AND:  demo_out = demo_in1 & demo_in2; break;
        case GateType::OR:   demo_out = demo_in1 | demo_in2; break;
        case GateType::NOT:  demo_out = !demo_in1; break;
        case GateType::XOR:  demo_out = demo_in1 ^ demo_in2; break;
        case GateType::NAND: demo_out = !(demo_in1 & demo_in2); break;
        case GateType::NOR:  demo_out = !(demo_in1 | demo_in2); break;
        case GateType::XNOR: demo_out = !(demo_in1 ^ demo_in2); break;
    }

    DrawTextShadowed(font, "IN1", input_x, demo_center_y - 46, 11, {180, 200, 220, 180});
    DrawRectangleRounded(input_rect1, 0.5f, 4, demo_in1 ? Color{0,255,255,120} : Color{100,100,100,120});
    DrawRectangleRoundedLines(input_rect1, 0.5f, 4, hover_in1 ? WHITE : ColorAlpha({0,255,255,255}, 0.4f));
    DrawTextShadowed(font, demo_in1 ? "1" : "0", input_x + 16, demo_center_y - 24, 18, WHITE);

    if (!single_input)
    {
        DrawTextShadowed(font, "IN2", input_x, demo_center_y + 42, 11, {180, 200, 220, 180});
        DrawRectangleRounded(input_rect2, 0.5f, 4, demo_in2 ? Color{0,255,255,120} : Color{100,100,100,120});
        DrawRectangleRoundedLines(input_rect2, 0.5f, 4, hover_in2 ? WHITE : ColorAlpha({0,255,255,255}, 0.4f));
        DrawTextShadowed(font, demo_in2 ? "1" : "0", input_x + 16, demo_center_y + 16, 18, WHITE);
    }

    t_Gate mock_gate;
    mock_gate.type = current_gate;
    DrawGateShape(mock_gate, gate_draw_x, gate_draw_y, gate_width, gate_height, demo_out, 1.0f);

    DrawRectangleRounded({output_x, demo_center_y - 14, 44, 28}, 0.5f, 4,
        demo_out ? Color{0,255,100,150} : Color{80,80,80,150});
    DrawRectangleRoundedLines({output_x, demo_center_y - 14, 44, 28}, 0.5f, 4,
        demo_out ? ColorAlpha({0,255,100,255}, 0.8f) : ColorAlpha({150,150,150,255}, 0.3f));
    DrawTextShadowed(font, demo_out ? "1" : "0", output_x + 16, demo_center_y - 10, 20, WHITE);
    DrawTextShadowed(font, "OUT", output_x + 50, demo_center_y - 5, 11, {180, 200, 220, 180});

    float in1_wire_y = demo_center_y - 16;
    DrawLineEx({input_x + 44, in1_wire_y}, {gate_draw_x, in1_wire_y}, 3.0f,
        demo_in1 ? ColorAlpha({0,255,200,255}, 0.8f) : ColorAlpha(DARKGRAY, 0.5f));

    if (!single_input)
    {
        float in2_wire_y = demo_center_y + 24;
        DrawLineEx({input_x + 44, in2_wire_y}, {gate_draw_x, in2_wire_y}, 3.0f,
            demo_in2 ? ColorAlpha({0,255,200,255}, 0.8f) : ColorAlpha(DARKGRAY, 0.5f));
    }

    DrawLineEx({gate_draw_x + gate_width, demo_center_y}, {output_x, demo_center_y}, 3.0f,
        demo_out ? ColorAlpha({0,255,200,255}, 0.8f) : ColorAlpha(DARKGRAY, 0.5f));

    float flow = fmodf(anim_time * 2.0f, 1.0f);
    if (demo_in1)
    {
        float particle_x = input_x + 44 + flow * (gate_draw_x - input_x - 44);
        DrawCircleV({particle_x, in1_wire_y}, 3.0f, ColorAlpha(WHITE, 0.9f));
        DrawCircleV({particle_x, in1_wire_y}, 5.0f, ColorAlpha({0,255,200,255}, 0.3f));
    }
    if (!single_input && demo_in2)
    {
        float particle_x = input_x + 44 + flow * (gate_draw_x - input_x - 44);
        DrawCircleV({particle_x, demo_center_y + 24}, 3.0f, ColorAlpha(WHITE, 0.9f));
        DrawCircleV({particle_x, demo_center_y + 24}, 5.0f, ColorAlpha({0,255,200,255}, 0.3f));
    }
    if (demo_out)
    {
        float particle_x = gate_draw_x + gate_width + flow * (output_x - gate_draw_x - gate_width);
        DrawCircleV({particle_x, demo_center_y}, 3.0f, ColorAlpha(WHITE, 0.9f));
        DrawCircleV({particle_x, demo_center_y}, 5.0f, ColorAlpha({0,255,200,255}, 0.3f));
    }

    y = static_cast<int>(demo_center_y + gate_height / 2.0f + 20);

    DrawTextShadowed(font, "TRUTH TABLE", center_x - 40, y, 13, {150, 170, 200, 200});
    y += 18;

    int input_count = GetGateInputCount(current_gate);
    int combination_count = 1 << input_count;
    int current_index = input_count == 1 ? demo_in1 : (demo_in1 * 2 + demo_in2);

    DrawTextShadowed(font, "IN", center_x - 90, y, 11, {120, 140, 170, 200});
    DrawTextShadowed(font, "OUT", center_x + 20, y, 11, {120, 140, 170, 200});
    DrawLineEx
    (
        {center_x - 90, static_cast<float>(y + 14)},
        {center_x + 55, static_cast<float>(y + 14)},
        1, ColorAlpha({100, 120, 160, 255}, 0.3f)
    );
    y += 18;

    for (int row = 0; row < combination_count; row++)
    {
        int in_a = (row >> 1) & 1;
        int in_b = row & 1;
        int out_val = 0;
        switch (current_gate)
        {
            case GateType::AND:  out_val = in_a & in_b; break;
            case GateType::OR:   out_val = in_a | in_b; break;
            case GateType::NOT:  out_val = !in_a; break;
            case GateType::XOR:  out_val = in_a ^ in_b; break;
            case GateType::NAND: out_val = !(in_a & in_b); break;
            case GateType::NOR:  out_val = !(in_a | in_b); break;
            case GateType::XNOR: out_val = !(in_a ^ in_b); break;
        }

        bool is_current = (row == current_index);
        Color row_color = is_current ? Color{0,255,255,255} : Color{170,185,210,180};
        char row_buf[32];
        if (input_count == 1)
        {
            snprintf(row_buf, sizeof(row_buf), "%d", in_a);
        }
        else
        {
            snprintf(row_buf, sizeof(row_buf), "%d  %d", in_a, in_b);
        }
        DrawTextShadowed(font, row_buf, center_x - 85, y, 12, row_color);

        char output_buf[8];
        snprintf(output_buf, sizeof(output_buf), "%d", out_val);
        Color output_color = is_current
            ? Color{0,255,255,255}
            : (out_val ? Color{0,255,100,180} : Color{170,185,210,120});
        DrawTextShadowed(font, output_buf, center_x + 30, y, 12, output_color);

        if (is_current)
        {
            DrawCircleV({center_x - 98, static_cast<float>(y + 6)}, 3.0f, {0, 255, 255, 255});
            DrawRectangleRounded
            (
                {center_x - 102, static_cast<float>(y - 1), 100, 14},
                0.5f, 2, ColorAlpha({0,255,255,255}, 0.05f)
            );
        }
        y += 16;
    }
    y += 14;

    const char* controls_header = "CONTROLS";
    Vector2 controls_header_size = MeasureTextEx(font, controls_header, 22, 1);
    float controls_header_x = (SCREEN_WIDTH - controls_header_size.x) / 2.0f;
    DrawTextShadowed(font, controls_header, controls_header_x, y, 22, {0, 255, 150, 255});
    DrawPoly({controls_header_x - 15.0f, y + 12.0f}, 6, 8.0f, 90.0f, ColorAlpha({0, 255, 150, 255}, 0.8f));
    DrawPolyLinesEx({controls_header_x - 15.0f, y + 12.0f}, 6, 8.0f, 90.0f, 2.0f, {0, 255, 150, 255});
    y += 32;

    int col_left = static_cast<int>(center_x) - 180;
    int col_right = static_cast<int>(center_x) + 50;

    DrawRectangleRounded
    (
        {static_cast<float>(col_left), static_cast<float>(y), 24, 26},
        0.5f, 4, {100, 150, 200, 50}
    );
    DrawRectangleRounded
    (
        {static_cast<float>(col_left), static_cast<float>(y), 11, 13},
        0.5f, 4, {0, 255, 200, 200}
    );
    DrawTextShadowed(font, "LEFT CLICK", col_left + 35, y + 6, 13, {0, 255, 200, 255});
    DrawTextShadowed(font, "Place / Wire / Toggle", col_left + 35, y + 18, 11, {200, 215, 235, 150});

    DrawRectangleRounded
    (
        {static_cast<float>(col_right), static_cast<float>(y), 24, 26},
        0.5f, 4, {100, 150, 200, 50}
    );
    DrawRectangleRounded
    (
        {static_cast<float>(col_right + 13), static_cast<float>(y), 11, 13},
        0.5f, 4, {255, 100, 100, 200}
    );
    DrawTextShadowed(font, "RIGHT CLICK", col_right + 35, y + 6, 13, {255, 100, 100, 255});
    DrawTextShadowed(font, "Delete Gate", col_right + 35, y + 18, 11, {200, 215, 235, 150});
    y += 38;

    DrawRectangleRounded
    (
        {static_cast<float>(col_left), static_cast<float>(y), 24, 20},
        0.2f, 4, {200, 150, 50, 150}
    );
    DrawTextShadowed(font, "DRAG", col_left + 35, y + 4, 13, {255, 200, 50, 255});
    DrawTextShadowed(font, "Move Gate", col_left + 35, y + 16, 11, {200, 215, 235, 150});

    DrawRectangleRounded
    (
        {static_cast<float>(col_right), static_cast<float>(y), 24, 20},
        0.2f, 4, {200, 150, 255, 150}
    );
    DrawTextShadowed(font, "KEYS 1-4", col_right + 35, y + 4, 13, {200, 150, 255, 255});
    DrawTextShadowed(font, "Quick Toggle Input Bits", col_right + 35, y + 16, 11, {200, 215, 235, 150});
    y += 42;

    const char* gates_header = "GATES";
    Vector2 gates_header_size = MeasureTextEx(font, gates_header, 22, 1);
    float gates_header_x = (SCREEN_WIDTH - gates_header_size.x) / 2.0f;
    DrawTextShadowed(font, gates_header, gates_header_x, y, 22, {255, 100, 200, 255});
    DrawPoly({gates_header_x - 15.0f, y + 12.0f}, 6, 8.0f, 90.0f, ColorAlpha({255, 100, 200, 255}, 0.8f));
    DrawPolyLinesEx({gates_header_x - 15.0f, y + 12.0f}, 6, 8.0f, 90.0f, 2.0f, {255, 100, 200, 255});
    y += 8;

    const char* gate_descriptions[] =
    {
        "Both inputs = 1", "Either input = 1", "Flips the input bit",
        "Inputs differ", "NOT both = 1", "Neither = 1", "Inputs match"
    };

    float gate_spacing = 85.0f;
    float gate_start_x = center_x - (3.0f * gate_spacing);
    int hovered_gate_index = -1;
    float gate_row_y = static_cast<float>(y) + 10.0f;
    float gate_shape_w = 50.0f;
    float gate_shape_h = 50.0f * 1.732f * 0.75f;

    for (int i = 0; i < 7; i++)
    {
        t_Gate draw_gate;
        draw_gate.type = static_cast<GateType>(i);
        float gate_x = gate_start_x + i * gate_spacing;
        Rectangle gate_rect = {gate_x - 28, gate_row_y - 2, 56, gate_shape_h + 4};
        bool is_hovered = CheckCollisionPointRec(mouse_pos, gate_rect);
        bool is_selected = (i == selected_gate);
        if (is_hovered)
        {
            hovered_gate_index = i;
        }

        if (is_hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            selected_gate = i;
        }

        float scale = is_selected ? 1.15f : (is_hovered ? 1.08f : 1.0f);
        float scaled_w = gate_shape_w * scale;
        float scaled_h = gate_shape_h * scale;
        float scaled_x = gate_x - scaled_w / 2.0f;
        float scaled_y = gate_row_y + (gate_shape_h - scaled_h) / 2.0f;

        if (is_selected)
        {
            DrawRectangleRounded({gate_x - 30, gate_row_y - 4, 60, gate_shape_h + 8}, 0.3f, 4,
                ColorAlpha({0, 255, 255, 255}, 0.08f));
            DrawRectangleRoundedLines({gate_x - 30, gate_row_y - 4, 60, gate_shape_h + 8}, 0.3f, 4,
                ColorAlpha({0, 255, 255, 255}, 0.5f));
        }

        DrawGateShape
        (
            draw_gate, scaled_x, scaled_y, scaled_w, scaled_h,
            is_selected ? 1 : 0,
            is_selected ? 1.0f : (is_hovered ? 0.9f : 0.7f)
        );

        const char* gate_name = GateTypeToString(draw_gate.type);
        Vector2 name_size = MeasureTextEx(font, gate_name, 11, 1);
        Color name_color = is_selected ? Color{0, 255, 255, 255} : Color{180, 200, 220, 180};
        DrawTextShadowed
        (
            font, gate_name,
            gate_x - name_size.x / 2.0f, gate_row_y + gate_shape_h + 4,
            11, name_color
        );
    }

    y = static_cast<int>(gate_row_y + gate_shape_h + 24);

    if (hovered_gate_index != -1)
    {
        float tip_x = gate_start_x + hovered_gate_index * gate_spacing;
        const char* tip_text = gate_descriptions[hovered_gate_index];
        Vector2 tip_size = MeasureTextEx(font, tip_text, 14, 1);
        float tip_y = static_cast<float>(y) + 2.0f;
        DrawRectangleRounded
        (
            {tip_x - tip_size.x / 2.0f - 14, tip_y - 2, tip_size.x + 28, 24},
            0.5f, 4, ColorAlpha(BLACK, 0.9f)
        );
        DrawRectangleRoundedLines
        (
            {tip_x - tip_size.x / 2.0f - 14, tip_y - 2, tip_size.x + 28, 24},
            0.5f, 4, {255, 100, 200, 255}
        );
        DrawTextShadowed(font, tip_text, tip_x - tip_size.x / 2.0f, tip_y + 2, 14, WHITE);
        y += 30;
    }

    float back_btn_w = 280.0f;
    float back_btn_h = 44.0f;
    float back_btn_x = (SCREEN_WIDTH - back_btn_w) / 2.0f;
    Rectangle back_btn = {back_btn_x, 660, back_btn_w, back_btn_h};
    bool back_hovered = CheckCollisionPointRec(GetMousePosition(), back_btn);
    DrawHexButton(back_btn, "BACK TO TITLE  [ESC]", back_hovered, anim_time, {0, 150, 255, 255});

    float play_alpha = 0.3f + 0.2f * sinf(anim_time * 1.5f);
    const char* jump_text = "ENTER or SPACE to jump straight into the game!";
    Vector2 jump_size = MeasureTextEx(font, jump_text, 11, 1);
    DrawTextShadowed
    (
        font, jump_text,
        (SCREEN_WIDTH - jump_size.x) / 2.0f, 652, 11,
        ColorAlpha({150, 180, 220, 255}, play_alpha)
    );

    if (transition_time > 0.0f)
    {
        float t = transition_time / 0.8f;
        if (t > 1.0f)
        {
            t = 1.0f;
        }
        float max_radius = 1200.0f;
        float radius = t * max_radius;
        for (int i = 0; i < 40; i++)
        {
            DrawCircleLines(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, radius + i, {13, 13, 26, 255});
        }
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ColorAlpha({13, 13, 26, 255}, t * t));
    }

    DrawCRTAndMouseTrail();
}

GameState UpdateLevelComplete(float, const GameStats&)
{
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    {
        return GameState::LEVEL_COMPLETE_TO_PLAY_TRANSITION;
    }
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE))
    {
        return GameState::LEVEL_COMPLETE_TO_TITLE_TRANSITION;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mpos = GetMousePosition();
        float btn_w = 280.0f;
        float btn_h = 48.0f;
        float btn_x = (SCREEN_WIDTH - btn_w) / 2.0f;
        float panel_h = 560.0f;
        float panel_y = (SCREEN_HEIGHT - panel_h) / 2.0f;

        Rectangle next_btn = {btn_x, panel_y + 380.0f, btn_w, btn_h};
        Rectangle menu_btn = {btn_x, panel_y + 450.0f, btn_w, btn_h};

        if (CheckCollisionPointRec(mpos, next_btn)) return GameState::LEVEL_COMPLETE_TO_PLAY_TRANSITION;
        if (CheckCollisionPointRec(mpos, menu_btn)) return GameState::LEVEL_COMPLETE_TO_TITLE_TRANSITION;
    }

    return GameState::LEVEL_COMPLETE;
}

void DrawLevelComplete(float anim_time, float transition_time, const GameStats& stats, GameState game_state)
{
    float panel_w = 460.0f;
    float panel_h = 600.0f;

    // Scale animation (bloop)
    float scale = 1.0f;
    if (game_state == GameState::PLAYING_TO_LEVEL_COMPLETE_TRANSITION)
    {
        float t = transition_time / 0.8f;
        // Elastic out easing
        if (t == 0) scale = 0;
        else if (t == 1) scale = 1;
        else scale = pow(2, -10 * t) * sin((t * 10 - 0.75) * ((2 * PI) / 3)) + 1;
    }
    else if
    (
        game_state == GameState::LEVEL_COMPLETE_TO_PLAY_TRANSITION ||
        game_state == GameState::LEVEL_COMPLETE_TO_TITLE_TRANSITION
    )
    {
        float t = transition_time / 0.8f;
        // Back in easing
        float s = 1.70158f;
        scale = 1.0f - (t * t * ((s + 1) * t - s));
        if (scale < 0) scale = 0;
    }

    float current_w = panel_w * scale;
    float current_h = panel_h * scale;
    float panel_x = (SCREEN_WIDTH - current_w) / 2.0f;
    float panel_y = (SCREEN_HEIGHT - current_h) / 2.0f;

    // Confetti!
    static Vector2 confetti[100];
    static Color confetti_colors[100];
    static Vector2 confetti_vel[100];
    static bool confetti_init = false;

    if (game_state == GameState::PLAYING_TO_LEVEL_COMPLETE_TRANSITION && transition_time < 0.1f)
    {
        confetti_init = false;
    }

    if (!confetti_init && scale > 0.5f && game_state == GameState::PLAYING_TO_LEVEL_COMPLETE_TRANSITION)
    {
        confetti_init = true;
        for (int i = 0; i < 100; i++)
        {
            confetti[i] = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
            float angle = GetRandomValue(0, 360) * DEG2RAD;
            float speed = GetRandomValue(300, 1000);
            confetti_vel[i] = {cosf(angle) * speed, sinf(angle) * speed - 500.0f};
            int c = GetRandomValue(0, 4);
            if (c == 0) confetti_colors[i] = {0, 255, 255, 255}; // Cyan
            else if (c == 1) confetti_colors[i] = {255, 0, 255, 255}; // Magenta
            else if (c == 2) confetti_colors[i] = {255, 255, 0, 255}; // Yellow
            else if (c == 3) confetti_colors[i] = {0, 255, 0, 255}; // Green
            else confetti_colors[i] = {255, 50, 50, 255}; // Red
        }
    }

    if (confetti_init)
    {
        float dt = GetFrameTime();
        for (int i = 0; i < 100; i++)
        {
            confetti[i].x += confetti_vel[i].x * dt;
            confetti[i].y += confetti_vel[i].y * dt;
            confetti_vel[i].y += 1500.0f * dt; // gravity
            confetti_vel[i].x *= 0.93f; // drag
            if (confetti[i].y < SCREEN_HEIGHT + 20)
            {
                float size = 8.0f * scale;
                DrawRectangleV(confetti[i], {size, size}, confetti_colors[i]);
                // Confetti glow
                DrawRectangleV
                (
                    {confetti[i].x - size/2, confetti[i].y - size/2},
                    {size*2, size*2},
                    ColorAlpha(confetti_colors[i], 0.3f)
                );
            }
        }
    }

    if (scale < 0.1f) return;

    // Neon Glow / Bloom behind panel
    for (int i = 0; i < 5; i++)
    {
        float gw = current_w + i * 10 * scale;
        float gh = current_h + i * 10 * scale;
        float gx = (SCREEN_WIDTH - gw) / 2.0f;
        float gy = (SCREEN_HEIGHT - gh) / 2.0f;
        DrawRectangleRounded({gx, gy, gw, gh}, 0.1f, 8, ColorAlpha({0, 200, 255, 255}, 0.05f * scale));
    }

    // Draw neon panel base
    DrawRectangleRounded({panel_x, panel_y, current_w, current_h}, 0.1f, 8, ColorAlpha({10, 15, 25, 255}, 0.98f));

    // Panel Background Detail: Scrolling circuit grid
    BeginScissorMode((int)panel_x, (int)panel_y, (int)current_w, (int)current_h);
    float grid_y = fmodf(anim_time * 30.0f, 40.0f);
    for (int i = -1; i < 20; i++)
    {
        float gy = panel_y + i * 40 * scale + grid_y * scale;
        DrawLineEx({panel_x, gy}, {panel_x + current_w, gy}, 1.0f, ColorAlpha({0, 255, 255, 255}, 0.1f));
    }
    float grid_x = fmodf(anim_time * 20.0f, 40.0f);
    for (int i = -1; i < 15; i++)
    {
        float gx = panel_x + i * 40 * scale + grid_x * scale;
        DrawLineEx({gx, panel_y}, {gx, panel_y + current_h}, 1.0f, ColorAlpha({0, 255, 255, 255}, 0.1f));
    }
    EndScissorMode();

    DrawRectangleRoundedLines
    (
        {panel_x, panel_y, current_w, current_h},
        0.1f, 8, ColorAlpha({0, 255, 255, 255}, 0.8f * scale)
    );
    DrawRectangleRoundedLines
    (
        {panel_x-2, panel_y-2, current_w+4, current_h+4},
        0.1f, 8, ColorAlpha({255, 0, 255, 255}, 0.4f * scale)
    ); // double border

    // Dynamic titles
    static constexpr std::array titles =
    {
        "TARGET REACHED!",
        "IT'S ALIVE!",
        "BOOLEAN BEAUTY!",
        "MAGIC SMOKE CONTAINED!",
        "SPAGHETTI DETANGLED!",
        "CPU IS PLEASED."
    };
    static int title_idx = 0;
    if (game_state == GameState::PLAYING_TO_LEVEL_COMPLETE_TRANSITION && transition_time < 0.1f)
    {
        title_idx = GetRandomValue(0, 5);
    }
    const char* title = titles[title_idx];

    // Funny Ranks
    const char* rank = "Silicon Wizard";
    Color rank_col = {0, 255, 255, 255};
    if (stats.efficiency_score < 5000) { rank = "Magic Smoke Releaser"; rank_col = RED; }
    else if (stats.efficiency_score < 7000) { rank = "Spaghetti Chef"; rank_col = ORANGE; }
    else if (stats.efficiency_score < 8500) { rank = "Needs More Coffee"; rank_col = YELLOW; }

    // Lazy Developer rule!
    if (stats.gates_used == 0)
    {
        title = "PATH OF LEAST RESISTANCE";
        rank = "Lazy Developer";
        rank_col = MAGENTA;
    }
    else if (stats.efficiency_score < 9500) { rank = "Solid Developer"; rank_col = GREEN; }

    static float state_time = 0;
    if (game_state == GameState::LEVEL_COMPLETE) state_time += GetFrameTime();
    else state_time = 0;

    bool is_outbound =
    (
        game_state == GameState::LEVEL_COMPLETE_TO_PLAY_TRANSITION ||
        game_state == GameState::LEVEL_COMPLETE_TO_TITLE_TRANSITION
    );

    Font font = GetGameFont();

    // Header - Chromatic Split & Heavy Shadow
    float tw = MeasureTextEx(font, title, 32 * scale, 2).x;
    float tx = (SCREEN_WIDTH - tw) / 2.0f;
    float ty = panel_y + 40 * scale;
    float pulse = 0.5f + 0.5f * sinf(anim_time * 10.0f);

    // Shadow
    DrawTextShadowed
    (
        font, title, static_cast<int>(tx),
        static_cast<int>(ty + 4), 32 * scale, ColorAlpha(BLACK, 0.8f)
    );
    // Red split
    DrawTextEx(font, title, {tx - 2 * scale, ty}, 32 * scale, 2, ColorAlpha(RED, 0.8f));
    // Blue split
    DrawTextEx(font, title, {tx + 2 * scale, ty}, 32 * scale, 2, ColorAlpha(BLUE, 0.8f));
    // Main White/Cyan
    DrawTextEx(font, title, {tx, ty}, 32 * scale, 2, ColorAlpha({150, 255, 255, 255}, 0.9f + 0.1f * pulse));

    // Divider Line 1
    float div1_y = panel_y + 90 * scale;
    DrawLineEx
    (
        {panel_x + 40*scale, div1_y},
        {panel_x + panel_w*scale - 40*scale, div1_y},
        2.0f, ColorAlpha({255, 0, 255, 255}, 0.5f)
    );
    float dot1_x = panel_x + 40*scale + fmodf(anim_time * 200.0f, panel_w*scale - 80*scale);
    DrawCircleV({dot1_x, div1_y}, 4.0f*scale, {255, 255, 255, 255});

    char buf[128];
    float y_pos = panel_y + 120 * scale;

    float left_col_x = panel_x + 70 * scale;
    float right_col_x = panel_x + panel_w * scale - 70 * scale;

    if (is_outbound || (game_state == GameState::LEVEL_COMPLETE && state_time > 0.2f))
    {
        DrawTextShadowed
        (
            font,
            "Time:",
            static_cast<int>(left_col_x),
            static_cast<int>(y_pos),
            22 * scale,
            WHITE
        );
        std::snprintf(buf, sizeof(buf), "%.1fs", stats.time_taken);
        float sw = MeasureTextEx(font, buf, 22 * scale, 1).x;
        DrawTextShadowed
        (
            font,
            buf,
            static_cast<int>(right_col_x - sw),
            static_cast<int>(y_pos),
            22 * scale,
            {0, 255, 255, 255}
        ); // Cyan
    }
    y_pos += 40 * scale;

    if (is_outbound || (game_state == GameState::LEVEL_COMPLETE && state_time > 0.4f))
    {
        DrawTextShadowed
        (
            font,
            "Gates Used:",
            static_cast<int>(left_col_x),
            static_cast<int>(y_pos),
            22 * scale,
            WHITE
        );
        std::snprintf(buf, sizeof(buf), "%d", stats.gates_used);
        float sw = MeasureTextEx(font, buf, 22 * scale, 1).x;
        DrawTextShadowed
        (
            font,
            buf,
            static_cast<int>(right_col_x - sw),
            static_cast<int>(y_pos),
            22 * scale,
            {255, 0, 255, 255}
        ); // Magenta
    }
    y_pos += 40 * scale;

    if (is_outbound || (game_state == GameState::LEVEL_COMPLETE && state_time > 0.6f))
    {
        DrawTextShadowed
        (
            font,
            "Wires Used:",
            static_cast<int>(left_col_x),
            static_cast<int>(y_pos),
            22 * scale,
            WHITE
        );
        std::snprintf(buf, sizeof(buf), "%d", stats.wires_used);
        float sw = MeasureTextEx(font, buf, 22 * scale, 1).x;
        DrawTextShadowed
        (
            font,
            buf,
            static_cast<int>(right_col_x - sw),
            static_cast<int>(y_pos),
            22 * scale,
            {255, 255, 0, 255}
        ); // Yellow
    }
    y_pos += 50 * scale;

    // Divider Line 2
    float div2_y = panel_y + 265 * scale;
    DrawLineEx
    (
        {panel_x + 40*scale, div2_y},
        {panel_x + panel_w*scale - 40*scale, div2_y},
        2.0f,
        ColorAlpha({0, 255, 255, 255}, 0.5f)
    );
    float dot2_x = panel_x + panel_w*scale - 40*scale - fmodf(anim_time * 200.0f, panel_w*scale - 80*scale);
    DrawCircleV({dot2_x, div2_y}, 4.0f*scale, {255, 255, 255, 255});

    if (is_outbound || (game_state == GameState::LEVEL_COMPLETE && state_time > 1.0f))
    {
        std::snprintf(buf, sizeof(buf), "Efficiency: %d", stats.efficiency_score);
        float sw = MeasureTextEx(font, buf, 26 * scale, 1).x;
        // Pulse text
        float r_pulse = 0.5f + 0.5f * sinf(anim_time * 15.0f);
        DrawTextShadowed
        (
            font,
            buf,
            static_cast<int>((SCREEN_WIDTH - sw)/2),
            static_cast<int>(y_pos),
            26 * scale,
            ColorAlpha(WHITE, 0.8f + 0.2f * r_pulse)
        );

        y_pos += 55 * scale; // Pushed down further

        // Glowing box for Rank
        std::snprintf(buf, sizeof(buf), "Rank: %s", rank);
        float rw = MeasureTextEx(font, buf, 20 * scale, 1).x;
        Rectangle rank_rect = {(SCREEN_WIDTH - rw)/2 - 20*scale, y_pos - 8*scale, rw + 40*scale, 36*scale};
        DrawRectangleRounded(rank_rect, 0.3f, 8, ColorAlpha(rank_col, 0.2f + 0.1f * r_pulse));
        DrawRectangleRoundedLines(rank_rect, 0.3f, 8, ColorAlpha(rank_col, 0.6f + 0.4f * r_pulse));

        DrawTextShadowed
        (
            font,
            buf,
            static_cast<int>((SCREEN_WIDTH - rw)/2),
            static_cast<int>(y_pos),
            20 * scale,
            rank_col
        );
    }

    if (is_outbound || (game_state == GameState::LEVEL_COMPLETE && state_time > 1.5f))
    {
        // Buttons
        Vector2 mpos = GetMousePosition();
        float btn_w = 300.0f * scale;
        float btn_h = 50.0f * scale;
        float btn_x = (SCREEN_WIDTH - btn_w) / 2.0f;

        Rectangle next_btn = {btn_x, panel_y + 400 * scale, btn_w, btn_h};
        Rectangle menu_btn = {btn_x, panel_y + 470 * scale, btn_w, btn_h};

        bool next_hov = CheckCollisionPointRec(mpos, next_btn);
        bool menu_hov = CheckCollisionPointRec(mpos, menu_btn);

        // Cyberpunk cut-corner button draw function inline
        auto draw_cyber_btn = [&](Rectangle r, bool hov, Color base_col, const char* txt)
        {
            float c_cut = 15.0f * scale;
            Vector2 pts[6] =
            {
                {r.x + c_cut, r.y},
                {r.x + r.width, r.y},
                {r.x + r.width, r.y + r.height - c_cut},
                {r.x + r.width - c_cut, r.y + r.height},
                {r.x, r.y + r.height},
                {r.x, r.y + c_cut}
            };
            Color fill = hov ? ColorAlpha(base_col, 0.6f) : ColorAlpha(base_col, 0.2f);
            Color outline = hov ? WHITE : base_col;

            // Fill
            DrawTriangle(pts[0], pts[5], pts[1], fill);
            DrawTriangle(pts[1], pts[5], pts[4], fill);
            DrawTriangle(pts[1], pts[4], pts[2], fill);
            DrawTriangle(pts[2], pts[4], pts[3], fill);

            // Outline
            for (int i = 0; i < 6; i++)
            {
                DrawLineEx(pts[i], pts[(i+1)%6], hov ? 3.0f*scale : 2.0f*scale, outline);
            }

            // Tech detail lines
            if (hov)
            {
                DrawLineEx
                (
                    {r.x + 5*scale, r.y + r.height - 5*scale},
                    {r.x + 20*scale, r.y + r.height - 5*scale},
                    2.0f*scale,
                    WHITE
                );
            }

            float tw = MeasureTextEx(font, txt, 22 * scale, 2).x;
            DrawTextShadowed
            (
                font,
                txt,
                static_cast<int>(r.x + (r.width - tw)/2),
                static_cast<int>(r.y + 14 * scale),
                22 * scale,
                hov ? WHITE : ColorAlpha(WHITE, 0.8f)
            );
        };

        draw_cyber_btn(next_btn, next_hov, {0, 255, 255, 255}, "NEXT TARGET");
        draw_cyber_btn(menu_btn, menu_hov, {255, 0, 150, 255}, "MAIN MENU");
    }

    if (is_outbound)
    {
        float t = transition_time / 0.8f;
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ColorAlpha(BLACK, t));
    }
}
