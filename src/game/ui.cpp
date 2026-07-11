#include "ui.h"
#include "hex_grid.h"
#include "text_util.h"
#include <cmath>
#include <cstring> // IWYU pragma: keep
#include <raylib.h>
#include "assets.h"

void DrawBackground()
{
    // Main gradient: dark navy
    DrawRectangleGradientV(0, 0, 720, 720, {5, 8, 20, 255}, {12, 18, 38, 255});

    // Faint hex pattern in background
    float bg_offset = GetTime() * 2.0f;
    for (int y = -20; y < 750; y += 40)
    {
        for (int x = -20; x < 750; x += 35)
        {
            float x_offset = (y % 80 == 0) ? 0 : 17.5f;
            DrawHexOutline({x + x_offset - bg_offset, static_cast<float>(y)}, 16.0f, 1.0f, {30, 45, 80, 30});
        }
    }

    // Drifting dust particles
    float time = GetTime();
    for(int i = 0; i < 50; i++) 
    {
        float px = fmodf(static_cast<float>(i * 137) + time * 10.0f * (1.0f + (i%3)*0.5f), 720.0f);
        float py = fmodf(static_cast<float>(i * 93) - time * 5.0f * (1.0f + (i%5)*0.2f) + 720.0f, 720.0f);
        DrawCircleV({px, py}, (i%2==0) ? 1.5f : 0.8f, {0, 200, 255, static_cast<unsigned char>(10 + (i%20))});
    }

    // Info bar background
    DrawRectangle(0, 0, 720, static_cast<int>(INFO_BAR_H), {8, 12, 28, 220});
    DrawLine(0, static_cast<int>(INFO_BAR_H), 720, static_cast<int>(INFO_BAR_H), {0, 200, 255, 150});

    // Grid area background with modern border
    Rectangle grid_rect = GetGridRect();
    Color grid_bg = {6, 10, 22, 200};
    DrawRectangleRec(grid_rect, grid_bg);
    DrawRectangleLinesEx(grid_rect, 2, {20, 150, 255, 100});
    DrawRectangleLinesEx({grid_rect.x-2, grid_rect.y-2, grid_rect.width+4, grid_rect.height+4}, 1, {0, 100, 200, 50});

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
        {0, 200, 255, 150}
    );
    DrawRectangle(0, static_cast<int>(PALETTE_Y - 1), 720, static_cast<int>(PALETTE_H + 4), {8, 12, 28, 240});
    DrawLine
    (
        0, 
        static_cast<int>(PALETTE_Y + PALETTE_H + 2), 
        720,
        static_cast<int>(PALETTE_Y + PALETTE_H + 2), 
        {0, 200, 255, 150}
    );
}

void DrawInfoBar(int target_hex, int current_hex, bool solved, float anim_time)
{
    Font font = GetGameFont();
    char target_text[32];
    snprintf(target_text, sizeof(target_text), "Target: 0x%X", target_hex);
    DrawTextShadowed(font, target_text, 20, 10, 18, {136, 153, 187, 255});

    char output_text[32];
    snprintf(output_text, sizeof(output_text), "Output: 0x%X", current_hex);
    Color output_color = solved ? Color{0, 255, 136, 255} : WHITE;
    DrawTextShadowed(font, output_text, 280, 10, 18, output_color);

    if (solved)
    {
        float pulse = 0.7f + 0.3f * sinf(anim_time * 4);
        Color solved_color = 
        {
            0, 255, 136,
            static_cast<unsigned char>(255 * pulse)
        };
        DrawTextShadowed(font, "SOLVED!", 600, 10, 18, solved_color);
    }
}

static void DrawNodeBody(float x, float y, float radius, bool active, bool is_hovered, float anim_time)
{
    float pulse = 0.5f + 0.5f * sinf(anim_time * 5.0f);
    Color glow_color = active ? Color{0, 255, 255, 60} : Color{0, 100, 255, 30};
    Color inner_color = active ? Color{0, 220, 255, 255} : Color{10, 30, 80, 255};
    Color border_color = active ? Color{255, 255, 255, 255} : Color{20, 100, 200, 255};

    if (active)
    {
        glow_color.a = static_cast<unsigned char>(glow_color.a * (1.0f + pulse * 0.5f));
    }

    // Glow (layered circles)
    for (int r = 35; r > 10; r -= 5)
    {
        Color c = glow_color;
        c.a = static_cast<unsigned char>(float(c.a) * (1.0f - static_cast<float>(r - 10) / 25.0f));
        DrawCircleV({x, y}, static_cast<float>(r), c);
    }

    // Body
    DrawCircleV({x, y}, static_cast<float>(radius), inner_color);
    DrawCircleV({x, y}, static_cast<float>(radius * 0.7f), border_color);

    if (is_hovered)
    {
        DrawCircleV({x, y}, static_cast<float>(radius + 4), ColorAlpha(SKYBLUE, 0.5f));
    }
}

static void DrawPin(Vector2 pos, bool active, bool is_hovered, bool is_connected, bool show_delete)
{
    Color fill{};
    if (is_hovered)
    {
        fill = WHITE;
    }
    else if (is_connected)
    {
        fill = active ? Color{0, 255, 255, 255} : Color{50, 150, 255, 255};
    }
    else
    {
        fill = active ? Color{0, 255, 255, 255} : Color{30, 80, 140, 255};
    }

    DrawCircleV(pos, PIN_RADIUS, fill);
    DrawCircleV(pos, PIN_RADIUS * 0.5f, WHITE);

    if (is_hovered)
    {
        Color ring = show_delete ? Color{255, 50, 50, 255} : Color{0, 255, 255, 255};
        DrawCircleLines(static_cast<int>(pos.x), static_cast<int>(pos.y), PIN_RADIUS + 4, ring);
        if (show_delete)
        {
            DrawTextShadowed
            (
                GetGameFont(), 
                "X", 
                static_cast<int>(pos.x) - 4,
                static_cast<int>(pos.y) - 5,
                10,
                RED
            );
        }
    }
}

void DrawInputNodes(int input_bits[4], const t_Pin* hovered_pin, float anim_time)
{
    for (int i = 0; i < 4; i++)
    {
        float y = GetInputNodeY(i);
        bool active = input_bits[i] == 1;
        bool pin_hovered = hovered_pin && hovered_pin->source_type == 0 &&
                           hovered_pin->source_id == i &&
                           !hovered_pin->is_input;

        // Node body
        DrawNodeBody(INPUT_X, y, 16, active, false, anim_time);

        // Label
        char label[8];
        snprintf(label, sizeof(label), "IN%d", i);
        DrawTextShadowed
        (
            GetGameFont(), 
            label,
            static_cast<int>(INPUT_X - 12),
            static_cast<int>(y - 7),
            12,
            WHITE
        );

        // Output pin
        Vector2 pin_pos = GetInputNodeOutputPin(i);
        DrawPin(pin_pos, active, pin_hovered, false, false);
    }
}

void DrawOutputNode(int output_bits[4], int target_hex, const t_Pin* hovered_pin, bool has_wire, float anim_time)
{
    float cx = OUTPUT_CENTER_X;
    float cy = OUTPUT_CENTER_Y;
    int val = output_bits[0] + output_bits[1] * 2 + output_bits[2] * 4 + output_bits[3] * 8;
    bool match = (val == target_hex);
    float pulse = match ? 0.7f + 0.3f * sinf(anim_time * 5.0f) : 1.0f;

    // Body glow
    Color glow_col = match ? ColorAlpha({0, 255, 136, 255}, 0.5f * pulse)
                           : ColorAlpha({0, 150, 255, 255}, 0.2f);
    for (int r = 70; r > 20; r -= 10)
    {
        Color c = glow_col;
        c.a = static_cast<unsigned char>(float(c.a) * (1.0f - static_cast<float>(r - 20) / 50.0f));
        DrawCircleV({cx, cy}, static_cast<float>(r), c);
    }

    // Body
    DrawRectangleRounded({cx - 45, cy - 65, 90, 130}, 0.2f, 10, {10, 14, 28, 255});
    Color border = match ? Color{0, 255, 136, 255} : Color{0, 150, 255, 255};
    DrawRectangleRoundedLines({cx - 45, cy - 65, 90, 130}, 0.2f, 10, border);
    DrawRectangleRoundedLines({cx - 43, cy - 63, 86, 126}, 0.2f, 10, ColorAlpha(border, 0.3f));

    Font font = GetGameFont();
    DrawTextShadowed
    (
        font, 
        "OUT", 
        static_cast<int>(cx - 16), 
        static_cast<int>(cy - 55), 
        16, 
        {100, 200, 255, 255}
    );

    char hex_text[16];
    snprintf(hex_text, sizeof(hex_text), "0x%X", val);
    Color hex_color = match ? Color{0, 255, 136, 255} : WHITE;
    DrawTextShadowed
    (
        font, 
        hex_text, 
        static_cast<int>(cx - 36), 
        static_cast<int>(cy - 20), 
        30, 
        hex_color
    );

    // Bit indicators
    for (int b = 0; b < 4; b++)
    {
        float bx = cx - 24 + b * 16;
        float by = cy + 40;
        Color bit_color =
            output_bits[b] ? Color{0, 255, 255, 255} : Color{20, 60, 100, 255};
        Color bit_border =
            output_bits[b] ? Color{255, 255, 255, 255} : Color{40, 100, 160, 255};
        DrawCircleV({bx, by}, 5, bit_color);
        DrawCircleV({bx, by}, 5, bit_border);

        char bit_label[4];
        snprintf(bit_label, sizeof(bit_label), "b%d", b);
        DrawTextShadowed
        (
            GetGameFont(), 
            bit_label, 
            static_cast<int>(bx - 6), 
            static_cast<int>(by + 10),
            10, 
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

    Vector2 text_size = MeasureTextEx(font, "MENU [ESC]", 14.0f, 1.0f);
    float tx = r.x + (r.width - text_size.x) / 2.0f;
    float ty = r.y + (r.height - text_size.y) / 2.0f;
    DrawTextShadowed(font, "MENU [ESC]", static_cast<int>(tx), static_cast<int>(ty), 14, hovered ? WHITE : Color{ 150, 180, 220, 255 });
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
    Vector2 mouse_pos = GetMousePosition();

    for (int i = 0; i < GATE_COUNT; i++)
    {
        GateType type = static_cast<GateType>(i);
        Rectangle r = GetPaletteButtonRect(i);
        bool is_selected = (i == selected_index);
        bool is_hovered = CheckCollisionPointRec(mouse_pos, r);

        // Hover animation
        if (is_hovered)
        {
            r.x -= 2; r.y -= 2;
            r.width += 4; r.height += 4;
        }

        Color bg = is_selected ? Color{0, 100, 150, 255} : (is_hovered ? Color{20, 40, 70, 255} : Color{15, 25, 45, 255});
        Color border = is_selected ? Color{0, 255, 255, 255} : (is_hovered ? Color{0, 200, 255, 255} : Color{40, 80, 120, 255});

        if (is_selected || is_hovered)
        {
            DrawRectangleRounded({r.x-2, r.y-2, r.width+4, r.height+4}, 0.2f, 6, ColorAlpha(border, 0.4f));
        }

        DrawRectangleRounded(r, 0.2f, 6, bg);
        
        // 3D lip
        Rectangle bottom_lip = {r.x, r.y + r.height - 6, r.width, 6};
        DrawRectangleRounded(bottom_lip, 0.2f, 6, ColorAlpha(BLACK, 0.4f));
        
        DrawRectangleRoundedLines(r, 0.2f, 6, border);

        t_Gate temp_gate;
        temp_gate.type = type;
        temp_gate.row = 0; temp_gate.col = 0;
        
        float gw = r.height * 1.2f;
        float gh = r.height * 1.2f;
        float gx = r.x + (r.width - gw) / 2.0f;
        float gy = r.y + (r.height - gh) / 2.0f - 2.0f; // Shift up slightly for 3D lip
        
        DrawGateShape(temp_gate, gx, gy, gw, gh, is_selected ? 1 : 0, 1.0f);
    }

    Rectangle clear_r = GetClearButtonRect();
    bool clear_hovered = CheckCollisionPointRec(mouse_pos, clear_r);
    if (clear_hovered)
    {
        clear_r.x -= 2; clear_r.y -= 2;
        clear_r.width += 4; clear_r.height += 4;
    }
    
    DrawRectangleRounded({clear_r.x-2, clear_r.y-2, clear_r.width+4, clear_r.height+4}, 0.2f, 6, ColorAlpha(RED, clear_hovered ? 0.4f : 0.2f));
    DrawRectangleRounded(clear_r, 0.2f, 6, clear_hovered ? Color{60, 15, 20, 255} : Color{40, 10, 15, 255});
    DrawRectangleRoundedLines(clear_r, 0.2f, 6, clear_hovered ? Color{255, 100, 100, 255} : Color{255, 50, 80, 255});
    DrawTextCentered(GetGameFont(), "CLEAR", clear_r, 14, clear_hovered ? WHITE : Color{255, 100, 120, 255});
}
