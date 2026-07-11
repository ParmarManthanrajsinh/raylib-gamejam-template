#include "gates.h"
#include "hex_grid.h"
#include "text_util.h"
#include "assets.h"
#include <cmath> // IWYU pragma: keep
#include <cstring>
#include <raylib.h>

namespace GateColors
{
    Color body_on = {58, 16, 32, 255};
    Color body_off = {26, 26, 53, 255};
    Color border_on = {255, 0, 64, 255};
    Color border_off = {68, 85, 119, 255};
    Color label = {255, 255, 255, 255};
}

int GetGateInputCount(GateType type)
{
    return type == GateType::NOT ? 1 : 2;
}

float GetInputNodeY(int index)
{
    float spacing = GRID_H / 5;
    return GRID_Y + spacing + index * spacing;
}

Vector2 GetInputNodeOutputPin(int index)
{
    return {INPUT_PIN_X, GetInputNodeY(index)};
}

Vector2 GetOutputNodeInputPin(int bit_index)
{
    float spacing = 22.0f;
    float base_y = OUTPUT_CENTER_Y - (1.5f * spacing);
    return {OUTPUT_PIN_X, base_y + bit_index * spacing};
}

Vector2 GetGateInputPinPos(GateType type, Vector2 center, int pin_index)
{
    float pin_x = center.x - HEX_SIZE * SQRT3 / 2;
    int input_count = GetGateInputCount(type);
    if (input_count == 1) return {pin_x + 4, center.y};

    float spacing = HEX_SIZE * SQRT3 * 0.55f;
    float start_y = center.y - spacing / 2;
    float frac = (pin_index + 0.5f) / input_count;
    return {pin_x + 4, start_y + frac * spacing};
}

Vector2 GetGateInputPinPos(const t_Gate& gate, int pin_index)
{
    Vector2 c = GetHexCenter(gate.row, gate.col);
    return GetGateInputPinPos(gate.type, c, pin_index);
}

Vector2 GetGateOutputPinPos(Vector2 center)
{
    return {center.x + HEX_SIZE * SQRT3 / 2 - 4, center.y};
}

Vector2 GetGateOutputPinPos(const t_Gate& gate)
{
    Vector2 c = GetHexCenter(gate.row, gate.col);
    return GetGateOutputPinPos(c);
}

void DrawGateShape
(
    const t_Gate& gate,
    float x,
    float y,
    float w,
    float h,
    int output_val,
    float alpha
)
{
    float pulse = 0.5f + 0.5f * sinf(GetTime() * 5.0f);
    bool active = output_val == 1;

    Color base = GateColors::body_off;
    Color edge = GateColors::border_off;
    Color glow = {0,0,0,0};

    if (active)
    {
        switch (gate.type)
        {
            case GateType::AND:  edge = {0, 255, 128, 255}; base = {0, 60, 30, 255}; break;
            case GateType::OR:   edge = {0, 200, 255, 255}; base = {0, 40, 60, 255}; break;
            case GateType::NOT:  edge = {255, 100, 100, 255}; base = {60, 20, 20, 255}; break;
            case GateType::XOR:  edge = {200, 100, 255, 255}; base = {50, 20, 60, 255}; break;
            case GateType::NAND: edge = {255, 255, 100, 255}; base = {60, 60, 20, 255}; break;
            case GateType::NOR:  edge = {255, 150, 50, 255};  base = {60, 40, 10, 255}; break;
            case GateType::XNOR: edge = {150, 255, 255, 255}; base = {30, 60, 60, 255}; break;
        }
        glow = edge;

        for (int r = 10; r > 0; r -= 2)
        {
            Color c = glow;
            c.a = static_cast<unsigned char>(float(c.a) * (r / 10.0f) * pulse * alpha);
            DrawCircleV({x + w / 2, y + h / 2}, w * 0.4f + r, c);
        }
    }

    Vector2 center = {x + w / 2, y + h / 2};
    float radius = w * 0.45f;
    Vector2 v[6];
    for (int i = 0; i < 6; i++)
    {
        float a = PI / 6 + (PI / 3) * i;
        v[i] = {center.x + radius * cosf(a), center.y + radius * sinf(a)};
    }
    
    Color chip_base = {20, 25, 35, static_cast<unsigned char>(255 * alpha)};
    DrawTriangle(v[0], v[1], v[2], chip_base);
    DrawTriangle(v[0], v[2], v[3], chip_base);
    DrawTriangle(v[0], v[3], v[4], chip_base);
    DrawTriangle(v[0], v[4], v[5], chip_base);

    Color inner_fill = active ? ColorAlpha(base, 0.4f * alpha) : ColorAlpha(base, 0.1f * alpha);
    DrawTriangle(v[0], v[1], v[2], inner_fill);
    DrawTriangle(v[0], v[2], v[3], inner_fill);
    DrawTriangle(v[0], v[3], v[4], inner_fill);
    DrawTriangle(v[0], v[4], v[5], inner_fill);

    Color etch = active ? ColorAlpha(glow, 0.8f * alpha) : ColorAlpha(edge, 0.4f * alpha);
    DrawLineEx({center.x - 10, center.y - 10}, {center.x, center.y - 15}, 2.0f, etch);
    DrawLineEx({center.x, center.y - 15}, {center.x + 10, center.y - 10}, 2.0f, etch);
    DrawLineEx({center.x - 10, center.y + 10}, {center.x, center.y + 15}, 2.0f, etch);
    DrawLineEx({center.x, center.y + 15}, {center.x + 10, center.y + 10}, 2.0f, etch);

    for (int i = 0; i < 6; i++)
    {
        Vector2 p1 = v[i];
        Vector2 p2 = v[(i + 1) % 6];
        DrawLineEx(p1, p2, 3.0f, ColorAlpha(edge, alpha));
        if (active) DrawLineEx(p1, p2, 1.5f, ColorAlpha(WHITE, alpha * 0.8f));
    }

    for (int i = 0; i < 6; i++)
    {
        DrawCircleV(v[i], 2.5f, ColorAlpha(edge, alpha));
        if (active) DrawCircleV(v[i], 1.5f, ColorAlpha(WHITE, alpha));
    }

    const char* label = GateTypeToString(gate.type);
    
    int label_len = static_cast<int>(strlen(label));
    int font_size = (label_len <= 3) ? 18 : (label_len <= 4) ? 15 : 13;
    DrawTextCentered
    (
        GetGameFont(), label, {x, y, w, h}, font_size,
        ColorAlpha(GateColors::label, alpha)
    );
}
