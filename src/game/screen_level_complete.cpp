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
    constexpr std::array funny_taglines =
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
        const char* title = "HEX GATE";
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
    auto title = titles[title_idx];

    // Funny Ranks
    std::string_view rank = "Silicon Wizard";
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
        std::snprintf(buf, sizeof(buf), "Rank: %s", rank.data());
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

