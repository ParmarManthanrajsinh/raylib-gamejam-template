#include "menu.h"
#include "assets.h"
#include "hex_grid.h"
#include "text_util.h"
#include "gates.h"
#include <cmath>
#include <array>
#include <raylib.h>

namespace
{
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
    int y = 110;

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
    y += 26;

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

    float demo_center_y = y + 26;
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

    y = static_cast<int>(demo_center_y + gate_height / 2.0f + 10);

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
    y += 15;

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
    y += 28;

    const char* gates_header = "GATES";
    Vector2 gates_header_size = MeasureTextEx(font, gates_header, 22, 1);
    float gates_header_x = (SCREEN_WIDTH - gates_header_size.x) / 2.0f;
    DrawTextShadowed(font, gates_header, gates_header_x, y, 22, {255, 100, 200, 255});
    DrawPoly({gates_header_x - 15.0f, y + 12.0f}, 6, 8.0f, 90.0f, ColorAlpha({255, 100, 200, 255}, 0.8f));
    DrawPolyLinesEx({gates_header_x - 15.0f, y + 12.0f}, 6, 8.0f, 90.0f, 2.0f, {255, 100, 200, 255});
    y += 8;

    constexpr std::array gate_descriptions =
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

    y = static_cast<int>(gate_row_y + gate_shape_h + 12);

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

