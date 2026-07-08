#include "ui.h"
#include "hex_grid.h"
#include "text_util.h"
#include <cmath>
#include <cstring> // IWYU pragma: keep
#include <raylib.h>

void DrawBackground()
{
    // Main gradient
    DrawRectangleGradientV(0, 0, 720, 720, {10, 10, 26, 255}, {21, 21, 42, 255});

    // Info bar background
    DrawRectangle(0, 0, 720, static_cast<int>(INFO_BAR_H), {18, 18, 42, 255});
    DrawLine(0, static_cast<int>(INFO_BAR_H), 720, static_cast<int>(INFO_BAR_H), {42, 42, 74, 255});

    // Grid area background with border
    Rectangle grid_rect = GetGridRect();
    Color grid_bg = {13, 13, 32, 255};
    DrawRectangleRec(grid_rect, grid_bg);
    DrawRectangleLinesEx(grid_rect, 2, {42, 58, 74, 255});

    // Lane guides from inputs to grid
    float grid_left_x = GRID_X - HEX_SIZE * SQRT3 / 2;
    for (int i = 0; i < 4; i++)
    {
        float y = GetInputNodeY(i);
        Color lane = ColorAlpha({0, 245, 212, 255}, 0.20f);
        DrawLineEx({INPUT_PIN_X + 4, y}, {grid_left_x, y}, 2, lane);
        DrawCircleV({grid_left_x - 2, y}, 2.5f, lane);
    }

    // Lane guides from grid to output
    float grid_right_x = GRID_X + (GRID_COLS - 1) * SPACING_X + SPACING_X / 2 + HEX_SIZE * SQRT3 / 2;
    for (int b = 0; b < 4; b++)
    {
        Vector2 pin = GetOutputNodeInputPin(b);
        Color lane = ColorAlpha({0, 245, 212, 255}, 0.20f);
        DrawLineEx({grid_right_x + 4, pin.y}, {pin.x - 6, pin.y}, 2, lane);
        DrawCircleV({grid_right_x + 4, pin.y}, 2.5f, lane);
    }

    // Separators above and below palette
    DrawLine
    (
        0, 
        static_cast<int>(PALETTE_Y - 2), 
        720,
        static_cast<int>(PALETTE_Y - 2), 
        {42, 42, 74, 255}
    );
    DrawLine
    (
        0, 
        static_cast<int>(PALETTE_Y + PALETTE_H + 2), 
        720,
        static_cast<int>(PALETTE_Y + PALETTE_H + 2), 
        {42, 42, 74, 255}
    );
}

void DrawInfoBar(int target_hex, int current_hex, bool solved, float anim_time)
{
    Font font = GetFontDefault();
    char target_text[32];
    snprintf(target_text, sizeof(target_text), "Target: 0x%X", target_hex);
    DrawTextShadowed(font, target_text, 20, 16, 10, {136, 153, 187, 255});

    char output_text[32];
    snprintf(output_text, sizeof(output_text), "Output: 0x%X", current_hex);
    Color output_color = solved ? Color{0, 255, 136, 255} : WHITE;
    DrawTextShadowed(font, output_text, 280, 16, 10, output_color);

    if (solved)
    {
        float pulse = 0.7f + 0.3f * sinf(anim_time * 4);
        Color solved_color = 
        {
            0, 255, 136,
            static_cast<unsigned char>(255 * pulse)
        };
        DrawTextShadowed(font, "SOLVED!", 600, 14, 11, solved_color);
    }
}

static void DrawNodeBody(float x, float y, float radius, bool active, bool is_hovered)
{
    Color glow_color = active ? Color{255, 0, 64, 60} : Color{0, 68, 170, 60};
    Color inner_color =
        active ? Color{255, 32, 64, 255} : Color{26, 58, 106, 255};
    Color border_color =
        active ? Color{255, 96, 128, 255} : Color{51, 102, 170, 255};

    // Glow (layered circles)
    for (int r = 30; r > 10; r -= 4)
    {
        Color c = glow_color;
        c.a = static_cast<unsigned char>(float(c.a) * (1.0f - static_cast<float>(r - 10) / 20.0f));
        DrawCircleV({x, y}, static_cast<float>(r), c);
    }

    // Body
    DrawCircleV({x, y}, static_cast<float>(radius), inner_color);
    DrawCircleV({x, y}, static_cast<float>(radius), border_color);

    if (is_hovered)
    {
        DrawCircleV({x, y}, static_cast<float>(radius + 3), ColorAlpha(SKYBLUE, 0.5f));
    }
}

static void DrawPin(Vector2 pos, bool active, bool is_hovered, bool is_connected, bool show_delete)
{
    Color fill{};
    if (is_hovered)
    {
        fill = SKYBLUE;
    }
    else if (is_connected)
    {
        fill = active ? Color{255, 96, 128, 255} : Color{255, 170, 0, 255};
    }
    else
    {
        fill = active ? Color{255, 96, 128, 255} : Color{85, 119, 187, 255};
    }

    DrawCircleV(pos, PIN_RADIUS, fill);
    DrawCircleV(pos, PIN_RADIUS, WHITE);

    if (is_hovered)
    {
        Color ring = show_delete ? Color{255, 0, 64, 255} : SKYBLUE;
        DrawCircleLines(static_cast<int>(pos.x), static_cast<int>(pos.y), PIN_RADIUS + 3, ring);
        if (show_delete)
        {
            DrawTextShadowed
            (
                GetFontDefault(), 
                "X", 
                static_cast<int>(pos.x) - 4,
                static_cast<int>(pos.y) - 5,
                10,
                RED
            );
        }
    }
}

void DrawInputNodes(int input_bits[4], const t_Pin* hovered_pin)
{
    for (int i = 0; i < 4; i++)
    {
        float y = GetInputNodeY(i);
        bool active = input_bits[i] == 1;
        bool pin_hovered = hovered_pin && hovered_pin->source_type == 0 &&
                           hovered_pin->source_id == i &&
                           !hovered_pin->is_input;

        // Node body
        DrawNodeBody(INPUT_X, y, 16, active, false);

        // Label
        char label[8];
        snprintf(label, sizeof(label), "IN%d", i);
        DrawTextShadowed
        (
            GetFontDefault(), 
            label,
            static_cast<int>(INPUT_X - 10),
            static_cast<int>(y - 5),
            9,
            WHITE
        );

        // Output pin
        Vector2 pin_pos = GetInputNodeOutputPin(i);
        DrawPin(pin_pos, active, pin_hovered, false, false);
    }
}

void DrawOutputNode(int output_bits[4], int target_hex, const t_Pin* hovered_pin, bool has_wire)
{
    float cx = OUTPUT_CENTER_X;
    float cy = OUTPUT_CENTER_Y;
    int val = output_bits[0] + output_bits[1] * 2 + output_bits[2] * 4 + output_bits[3] * 8;
    bool match = (val == target_hex);

    // Body glow
    Color glow_col = match ? ColorAlpha({0, 255, 136, 255}, 0.4f)
                           : ColorAlpha({50, 70, 100, 255}, 0.3f);
    for (int r = 50; r > 15; r -= 5)
    {
        Color c = glow_col;
        c.a = static_cast<unsigned char>(float(c.a) * (1.0f - static_cast<float>(r - 15) / 35.0f));
        DrawCircleV({cx, cy}, static_cast<float>(r), c);
    }

    // Body
    DrawRectangleRounded({cx - 35, cy - 50, 70, 100}, 0.15f, 8, {26, 26, 58, 255});
    Color border = match ? Color{0, 255, 136, 255} : Color{58, 74, 106, 255};
    DrawRectangleRoundedLines({cx - 35, cy - 50, 70, 100}, 0.15f, 8, border);

    Font font = GetFontDefault();
    DrawTextShadowed
    (
        font, 
        "OUTPUT", 
        static_cast<int>(cx - 32), 
        static_cast<int>(cy - 28), 
        8, 
        {136, 153, 187, 255}
    );

    char hex_text[16];
    snprintf(hex_text, sizeof(hex_text), "0x%X", val);
    Color hex_color = match ? Color{0, 255, 136, 255} : WHITE;
    DrawTextShadowed
    (
        font, 
        hex_text, 
        static_cast<int>(cx - 24), 
        static_cast<int>(cy - 6), 
        16, 
        hex_color
    );

    // Bit indicators
    for (int b = 0; b < 4; b++)
    {
        float bx = cx - 18 + b * 12;
        float by = cy + 22;
        Color bit_color =
            output_bits[b] ? Color{255, 0, 64, 255} : Color{51, 68, 102, 255};
        Color bit_border =
            output_bits[b] ? Color{255, 96, 128, 255} : Color{68, 85, 119, 255};
        DrawCircleV({bx, by}, 4, bit_color);
        DrawCircleV({bx, by}, 4, bit_border);

        char bit_label[4];
        snprintf(bit_label, sizeof(bit_label), "b%d", b);
        DrawTextShadowed
        (
            GetFontDefault(), 
            bit_label, 
            static_cast<int>(bx - 4), 
            static_cast<int>(by + 8),
            6, 
            {102, 119, 153, 255}
        );
    }

    // Input pins
    for (int b = 0; b < 4; b++)
    {
        Vector2 pin_pos = GetOutputNodeInputPin(b);
        bool pin_hovered = hovered_pin && hovered_pin->source_type == 2 &&
                           hovered_pin->pin_index == b && hovered_pin->is_input;
        DrawPin(pin_pos, output_bits[b] == 1, pin_hovered, has_wire, has_wire);
    }
}

static Rectangle GetPaletteButtonRect(int index)
{
    float btn_w = 68;
    float spacing = 4;
    float total_gate = GATE_COUNT * btn_w + (GATE_COUNT - 1) * spacing;
    float total = total_gate + spacing + 90;
    float start_x = (720 - total) / 2;
    return {start_x + index * (btn_w + spacing), PALETTE_Y + 12, btn_w, 32};
}

int PickPaletteGate(Vector2 mouse_pos)
{
    for (int i = 0; i < GATE_COUNT; i++)
    {
        Rectangle r = GetPaletteButtonRect(i);
        if (CheckCollisionPointRec(mouse_pos, r)) return i;
    }
    return -1;
}

Rectangle GetClearButtonRect()
{
    float btn_w = 68;
    float spacing = 4;
    float total_gate = GATE_COUNT * btn_w + (GATE_COUNT - 1) * spacing;
    float total = total_gate + spacing + 90;
    float start_x = (720 - total) / 2;
    return {start_x + total_gate + spacing, PALETTE_Y + 12, 90, 32};
}

void DrawPalette(int selected_index)
{
    for (int i = 0; i < GATE_COUNT; i++)
    {
        GateType type = static_cast<GateType>(i);
        Rectangle r = GetPaletteButtonRect(i);
        bool is_selected = (i == selected_index);

        Color bg = is_selected ? Color{26, 58, 42, 255} : Color{26, 26, 53, 255};
        Color border = is_selected ? SKYBLUE : Color{58, 58, 85, 255};

        DrawRectangleRounded(r, 0.15f, 6, bg);
        DrawRectangleRoundedLines(r, 0.15f, 6, border);

        const char* label = GateTypeToString(type);
        DrawTextCentered(GetFontDefault(), label, r, 9, WHITE);
    }

    Rectangle clear_r = GetClearButtonRect();
    DrawRectangleRounded(clear_r, 0.15f, 6, Color{42, 26, 26, 255});
    DrawRectangleRoundedLines(clear_r, 0.15f, 6, Color{255, 68, 68, 255});
    DrawTextCentered(GetFontDefault(), "CLEAR", clear_r, 9, Color{255, 102, 102, 255});
}
