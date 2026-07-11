#include "menu.h"
#include "assets.h"
#include "hex_grid.h"
#include "text_util.h"
#include "gates.h"
#include <cmath>
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

            // Animated circuit border
            float border_anim = fmodf(anim_time * 200.0f, bounds.width + bounds.height);
            if (border_anim < bounds.width) {
                DrawLineEx({bounds.x + border_anim, bounds.y}, {bounds.x + border_anim + 20, bounds.y}, 3.0f, base_color);
                DrawLineEx({bounds.x + bounds.width - border_anim, bounds.y + bounds.height}, {bounds.x + bounds.width - border_anim - 20, bounds.y + bounds.height}, 3.0f, base_color);
            }
            else
            {
                float h_anim = border_anim - bounds.width;
                DrawLineEx({bounds.x + bounds.width, bounds.y + h_anim}, {bounds.x + bounds.width, bounds.y + h_anim + 20}, 3.0f, base_color);
                DrawLineEx({bounds.x, bounds.y + bounds.height - h_anim}, {bounds.x, bounds.y + bounds.height - h_anim - 20}, 3.0f, base_color);
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
                    {30, static_cast<unsigned char>(45 + pulse*150), static_cast<unsigned char>(80 + pulse*175), static_cast<unsigned char>(20 * wave + pulse*100)}
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
            float px = fmodf(static_cast<float>(i * 317) + time * 15.0f * (1.0f + (i % 2) * 0.5f), 900.0f) - 100.0f;
            float py = fmodf(static_cast<float>(i * 253) - time * 10.0f * (1.0f + (i % 3) * 0.3f) + 900.0f, 900.0f) - 100.0f;

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

            float wobble = sinf(anim_time * 2.5f + i * 0.8f) * 3.0f * (1.0f - fminf(fmaxf(age - 0.4f, 0.0f) / 0.3f, 1.0f));

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

        if (CheckCollisionPointRec(mpos, play_btn)) return GameState::TITLE_TO_PLAY_TRANSITION;
        if (CheckCollisionPointRec(mpos, howto_btn)) return GameState::TITLE_TO_HOW_TO_PLAY_TRANSITION;
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

        if (CheckCollisionPointRec(mpos, back_btn)) return GameState::HOW_TO_PLAY_TO_TITLE_TRANSITION;
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

    // Add very faint background so it's not totally empty but not distracting
    for (int i = 0; i < 40; i++)
    {
        DrawRectangleLines(i, i, SCREEN_WIDTH - i*2, SCREEN_HEIGHT - i*2, ColorAlpha(SKYBLUE, 0.02f));
    }

    // 5. Slow Drift BG: drifting faint gates
    for (int i = 0; i < 6; i++)
    {
        float px = fmodf(static_cast<float>(i * 412) + anim_time * 8.0f * (1.0f + (i % 2) * 0.5f), 900.0f) - 100.0f;
        float py = fmodf(static_cast<float>(i * 333) - anim_time * 5.0f * (1.0f + (i % 3) * 0.3f) + 900.0f, 900.0f) - 100.0f;
        t_Gate dummy; dummy.type = static_cast<GateType>(i % 7);
        DrawGateShape(dummy, px, py, 100.0f, 100.0f * 1.732f * 0.75f, 0, 0.03f); // Very faint
    }

    Font font = GetGameFont();

    float header_pulse = 0.7f + 0.3f * sinf(anim_time * 2.0f);
    const char* title_text = "HOW TO PLAY";
    Vector2 title_size = MeasureTextEx(font, title_text, 40, 1);
    DrawTextShadowed
    (
        font, 
        title_text, 
        (SCREEN_WIDTH - title_size.x) / 2.0f, 
        25, 
        40, 
        ColorAlpha({0, 255, 255, 255}, header_pulse)
    );

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

    const char* h1 = "THE GOAL";
    Vector2 h1_size = MeasureTextEx(font, h1, 22, 1);
    float h1_x = (SCREEN_WIDTH - h1_size.x) / 2.0f;
    DrawTextShadowed(font, h1, h1_x, y, 22, {0, 255, 255, 255});
    
    DrawPoly({h1_x - 15.0f, y + 12.0f}, 6, 8.0f, 90.0f, ColorAlpha({0, 255, 255, 255}, 0.8f));
    DrawPolyLinesEx({h1_x - 15.0f, y + 12.0f}, 6, 8.0f, 90.0f, 2.0f, {0, 255, 255, 255});
    y += 32;

    const char* goal_txt = "Transform INPUT bits into the TARGET hex value using Logic Gates.";
    Vector2 goal_size = MeasureTextEx(font, goal_txt, 15, 1);
    DrawTextShadowed(font, goal_txt, (SCREEN_WIDTH - goal_size.x) / 2.0f, y, 15, {220, 230, 245, 255});
    y += 40;

    float cx = SCREEN_WIDTH / 2.0f, cy = y + 30;

    static int demo_in1 = 1;
    static int demo_in2 = 1;
    Rectangle r1 = {cx - 80, cy - 25, 30, 20};
    Rectangle r2 = {cx - 80, cy + 5, 30, 20};

    Vector2 mp = GetMousePosition();
    bool hov1 = CheckCollisionPointRec(mp, r1);
    bool hov2 = CheckCollisionPointRec(mp, r2);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
    {
        if (hov1) { demo_in1 ^= 1; } // Optional: play sound if we included audio.h
        if (hov2) { demo_in2 ^= 1; }
    }

    int demo_out = demo_in1 & demo_in2; // AND gate

    // Input nodes
    DrawRectangleRounded(r1, 0.5f, 4, demo_in1 ? Color{0, 255, 255, 100} : Color{100, 100, 100, 100});
    if (hov1) DrawRectangleRoundedLines(r1, 0.5f, 4, WHITE);
    DrawTextShadowed(font, demo_in1 ? "1" : "0", cx - 70, cy - 23, 16, WHITE);

    DrawRectangleRounded(r2, 0.5f, 4, demo_in2 ? Color{0, 255, 255, 100} : Color{100, 100, 100, 100});
    if (hov2) DrawRectangleRoundedLines(r2, 0.5f, 4, WHITE);
    DrawTextShadowed(font, demo_in2 ? "1" : "0", cx - 70, cy + 7, 16, WHITE);

    float wire_pulse1 = demo_in1 ? (sinf(anim_time * 8.0f) + 1.0f) * 0.5f : 0.2f;
    float wire_pulse2 = demo_in2 ? (sinf(anim_time * 8.0f) + 1.0f) * 0.5f : 0.2f;
    float wire_pulse_out = demo_out ? (sinf(anim_time * 8.0f) + 1.0f) * 0.5f : 0.2f;

    DrawLineEx({cx - 50, cy - 15}, {cx - 20, cy - 15}, 3.0f, demo_in1 ? ColorAlpha(RED, wire_pulse1) : DARKGRAY);
    DrawLineEx({cx - 50, cy + 15}, {cx - 20, cy + 15}, 3.0f, demo_in2 ? ColorAlpha(RED, wire_pulse2) : DARKGRAY);
    DrawLineEx({cx + 20, cy}, {cx + 60, cy}, 3.0f, demo_out ? ColorAlpha(RED, wire_pulse_out) : DARKGRAY);

    float flow = fmodf(anim_time * 1.5f, 1.0f);
    if (demo_in1) DrawCircleV({cx - 50 + flow * 30.0f, cy - 15}, 2.0f, WHITE);
    if (demo_in2) DrawCircleV({cx - 50 + flow * 30.0f, cy + 15}, 2.0f, WHITE);
    if (demo_out) DrawCircleV({cx + 20 + flow * 40.0f, cy}, 2.0f, WHITE);

    // Gate
    t_Gate mock_gate; mock_gate.type = GateType::AND;
    DrawGateShape(mock_gate, cx - 20, cy - 20 * 1.732f * 0.75f, 40, 40 * 1.732f * 0.75f, demo_out, 1.0f);

    // Output
    DrawRectangleRounded({cx + 60, cy - 15, 30, 30}, 0.5f, 4, demo_out ? Color{0, 255, 100, 150} : Color{100, 100, 100, 150});
    DrawTextShadowed(font, demo_out ? "1" : "0", cx + 70, cy - 8, 20, WHITE);

    if (demo_out)
    {
        DrawTextShadowed(font, "TARGET MET!", cx + 100, cy - 5, 16, {0, 255, 100, 255});
    }
    else
    {
        DrawTextShadowed(font, "Click inputs to solve ->", cx - 240, cy - 5, 12, {150, 150, 150, 200});
    }

    y += 90;

    // --- CONTROLS SECTION ---
    const char* h2 = "CONTROLS";
    Vector2 h2_size = MeasureTextEx(font, h2, 22, 1);
    float h2_x = (SCREEN_WIDTH - h2_size.x) / 2.0f;
    DrawTextShadowed(font, h2, h2_x, y, 22, {0, 255, 150, 255});
    DrawPoly({h2_x - 15.0f, y + 12.0f}, 6, 8.0f, 90.0f, ColorAlpha({0, 255, 150, 255}, 0.8f));
    DrawPolyLinesEx({h2_x - 15.0f, y + 12.0f}, 6, 8.0f, 90.0f, 2.0f, {0, 255, 150, 255});
    y += 32;

    int col1 = cx - 180, col2 = cx + 50;

    // Left click
    DrawRectangleRounded({static_cast<float>(col1), static_cast<float>(y), 24, 30}, 0.5f, 4, {100, 150, 200, 50});
    DrawRectangleRounded({static_cast<float>(col1), static_cast<float>(y), 11, 14}, 0.5f, 4, {0, 255, 200, 200}); // L-click highlight
    DrawTextShadowed(font, "LEFT CLICK", col1 + 35, y + 8, 14, {0, 255, 200, 255});
    DrawTextShadowed(font, "Place / Wire / Toggle", col1 + 35, y + 22, 12, {200, 215, 235, 180});

    // Right click
    DrawRectangleRounded({static_cast<float>(col2), static_cast<float>(y), 24, 30}, 0.5f, 4, {100, 150, 200, 50});
    DrawRectangleRounded({static_cast<float>(col2 + 13), static_cast<float>(y), 11, 14}, 0.5f, 4, {255, 100, 100, 200}); // R-click highlight
    DrawTextShadowed(font, "RIGHT CLICK", col2 + 35, y + 8, 14, {255, 100, 100, 255});
    DrawTextShadowed(font, "Delete Gate", col2 + 35, y + 22, 12, {200, 215, 235, 180});

    y += 45;

    // Drag / Keys
    DrawRectangleRounded({static_cast<float>(col1), static_cast<float>(y), 24, 20}, 0.2f, 4, {200, 150, 50, 150});
    DrawTextShadowed(font, "DRAG", col1 + 35, y + 4, 14, {255, 200, 50, 255});
    DrawTextShadowed(font, "Move Gate", col1 + 35, y + 18, 12, {200, 215, 235, 180});

    DrawRectangleRounded({static_cast<float>(col2), static_cast<float>(y), 24, 20}, 0.2f, 4, {200, 150, 255, 150});
    DrawTextShadowed(font, "KEYS 1-4", col2 + 35, y + 4, 14, {200, 150, 255, 255});
    DrawTextShadowed(font, "Quick Toggle Input Bits", col2 + 35, y + 18, 12, {200, 215, 235, 180});

    y += 55;

    // --- GATE CHEAT SHEET ---
    const char* h3 = "GATES";
    Vector2 h3_size = MeasureTextEx(font, h3, 22, 1);
    float h3_x = (SCREEN_WIDTH - h3_size.x) / 2.0f;
    DrawTextShadowed(font, h3, h3_x, y, 22, {255, 100, 200, 255});
    DrawPoly({h3_x - 15.0f, y + 12.0f}, 6, 8.0f, 90.0f, ColorAlpha({255, 100, 200, 255}, 0.8f));
    DrawPolyLinesEx({h3_x - 15.0f, y + 12.0f}, 6, 8.0f, 90.0f, 2.0f, {255, 100, 200, 255});
    y += 32;

    const char* gate_desc[] =
    {
        "Both = 1", "Either = 1", "Flip bit", "Differ = 1",
        "NOT AND", "NOT OR", "Same = 1"
    };

    float spacing = 85;
    float start_x = cx - (3.0f * spacing);
    int hovered_gate = -1;

    for (int i = 0; i < 7; i++)
    {
        t_Gate dg; dg.type = static_cast<GateType>(i);
        float gx = start_x + i * spacing;
        Rectangle g_rect = {gx - 25, static_cast<float>(y), 50, 50 * 1.732f * 0.75f};
        bool is_hov = CheckCollisionPointRec(mp, g_rect);
        if (is_hov) hovered_gate = i;

        float scale = is_hov ? 1.2f : 1.0f;
        float gw = 50 * scale;
        float gh = 50 * 1.732f * 0.75f * scale;
        DrawGateShape(dg, gx - gw/2, y + (50 * 1.732f * 0.75f - gh)/2, gw, gh, is_hov ? 1 : 0, is_hov ? 1.0f : 0.8f);

        Vector2 text_size = MeasureTextEx(font, gate_desc[i], 11, 1);
        DrawTextShadowed(font, gate_desc[i], static_cast<int>(gx - text_size.x/2), y + 45, 11, {200, 215, 235, 180});
    }

    if (hovered_gate != -1)
    {
        float hx = start_x + hovered_gate * spacing;
        float hy = y + 80.0f;
        const char* tip = "LOGIC CORE";
        if (hovered_gate == 0) tip = "Out=1 if IN1=1 and IN2=1";
        if (hovered_gate == 1) tip = "Out=1 if IN1=1 or IN2=1";
        if (hovered_gate == 2) tip = "Out flips the IN bit";
        if (hovered_gate == 3) tip = "Out=1 if inputs differ";
        if (hovered_gate == 4) tip = "Out=0 if IN1=1 and IN2=1";
        if (hovered_gate == 5) tip = "Out=0 if IN1=1 or IN2=1";
        if (hovered_gate == 6) tip = "Out=1 if inputs match";

        Vector2 tip_s = MeasureTextEx(font, tip, 15, 1);
        DrawRectangleRounded({hx - tip_s.x/2 - 15.0f, hy - 18.0f, tip_s.x + 30.0f, 36.0f}, 0.5f, 4, ColorAlpha(BLACK, 0.9f));
        DrawRectangleRoundedLines({hx - tip_s.x/2 - 15.0f, hy - 18.0f, tip_s.x + 30.0f, 36.0f}, 0.5f, 4, {255, 100, 200, 255});
        DrawTextShadowed(font, tip, static_cast<int>(hx - tip_s.x/2), static_cast<int>(hy - 8), 15, WHITE);
    }

    y += 14;

    float btn_w = 280.0f;
    float btn_h = 44.0f;
    float btn_x = (SCREEN_WIDTH - btn_w) / 2.0f;
    Rectangle back_btn = {btn_x, 660, btn_w, btn_h};

    bool back_hovered = CheckCollisionPointRec(GetMousePosition(), back_btn);
    DrawHexButton(back_btn, "BACK TO TITLE  [ESC]", back_hovered, anim_time, {0, 150, 255, 255});

    float play_alpha = 0.3f + 0.2f * sinf(anim_time * 1.5f);
    const char* jump_txt = "ENTER or SPACE to jump straight into the game!";
    Vector2 jump_size = MeasureTextEx(font, jump_txt, 11, 1);
    DrawTextShadowed
    (
        font, jump_txt,
        (SCREEN_WIDTH - jump_size.x) / 2.0f, 650, 11,
        ColorAlpha({150, 180, 220, 255}, play_alpha)
    );

    if (transition_time > 0.0f)
    {
        float t = transition_time / 0.8f;
        if (t > 1.0f) t = 1.0f;

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
