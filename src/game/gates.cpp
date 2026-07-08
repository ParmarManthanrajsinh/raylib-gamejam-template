#include "gates.h"
#include "hex_grid.h"
#include "text_util.h"
#include <cmath> // IWYU pragma: keep
#include <raylib.h>
#include <span>

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

Vector2 GetGateInputPinPos(const t_Gate& gate, int pin_index)
{
    Vector2 c = GetHexCenter(gate.row, gate.col);
    float pin_x = c.x - HEX_SIZE * SQRT3 / 2;
    int input_count = GetGateInputCount(gate.type);
    if (input_count == 1) return {pin_x + 4, c.y};

    float spacing = HEX_SIZE * SQRT3 * 0.55f;
    float start_y = c.y - spacing / 2;
    float frac = (pin_index + 0.5f) / input_count;
    return {pin_x + 4, start_y + frac * spacing};
}

Vector2 GetGateOutputPinPos(const t_Gate& gate)
{
    Vector2 c = GetHexCenter(gate.row, gate.col);
    return {c.x + HEX_SIZE * SQRT3 / 2 - 4, c.y};
}

namespace
{
    void DrawGlow(Vector2 center, float radius, Color color)
    {
        // Simple glow as layered circles
        for (int i = 3; i >= 0; i--)
        {
            float r = radius + i * 4;
            Color c = color;
            c.a = 20;
            DrawCircleV(center, r, c);
        }
    }

    void DrawFilledPolygon(std::span<const Vector2> verts, Color fill)
    {
        if (verts.size() < 3) return;
        for (std::size_t i = 1; i < verts.size() - 1; i++)
        {
            DrawTriangle(verts[0], verts[i], verts[i + 1], fill);
        }
    }

    void DrawPolyOutline
    (
        std::span<const Vector2> verts,
        Color color,
        float thickness
    )
    {
        for (std::size_t i = 0; i < verts.size(); i++)
        {
            std::size_t next = (i + 1) % verts.size();
            DrawLineEx(verts[i], verts[next], thickness, color);
        }
    }
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
    float cx = x + w / 2;
    float cy = y + h / 2;
    float hw = w / 2;
    float hh = h / 2;

    Color body = output_val ? GateColors::body_on : GateColors::body_off;
    Color border = output_val ? GateColors::border_on : GateColors::border_off;
    if (alpha < 1.0f)
    {
        body.a = static_cast<unsigned char>(static_cast<float>(body.a) * alpha);
        border.a = static_cast<unsigned char>(static_cast<float>(border.a) * alpha);
    }

    // Glow for active gates
    if (output_val)
    {
        DrawGlow({cx, cy}, hw, GateColors::border_on);
    }

    switch (gate.type)
    {
        case GateType::AND:
        {
            // D-shape: flat left, rounded right
            DrawRectangleRounded({x, y, w, h}, 0.35f, 8, body);
            DrawRectangleRoundedLines({x, y, w, h}, 0.35f, 8, border);
            break;
        }

        case GateType::OR:
        {
            // Shield shape
            Vector2 v[5] =
            {
                {cx - hw * 0.2f, y},
                {cx + hw * 0.7f, y + hh * 0.2f},
                {cx + hw * 0.9f, cy},
                {cx + hw * 0.7f, y + h * 0.8f},
                {cx - hw * 0.2f, y + h}
            };
            DrawFilledPolygon(v, body);
            DrawPolyOutline(v, border, 2.5f);
            break;
        }

        case GateType::NOT:
        {
            // Right-pointing triangle
            Vector2 v[3] = {{x, y}, {x, y + h}, {x + w, cy}};
            DrawTriangle(v[0], v[1], v[2], body);
            DrawPolyOutline(v, border, 2.5f);
            break;
        }

        case GateType::XOR:
        {
            // OR shape with an extra curve on the left side
            Vector2 v[5] =
            {
                {cx - hw * 0.25f, y},
                {cx + hw * 0.7f, y + hh * 0.2f},
                {cx + hw * 0.9f, cy},
                {cx + hw * 0.7f, y + h * 0.8f},
                {cx - hw * 0.25f, y + h}
            };
            DrawFilledPolygon(v, body);
            DrawPolyOutline(v, border, 2.5f);

            // Extra left curve line
            DrawLineEx
            (
                {cx - hw * 0.55f, y + hh * 0.15f},
                {cx - hw * 0.55f, y + h * 0.85f}, 2.5f, border
            );
            break;
        }

        case GateType::NAND:
        {
            DrawRectangleRounded({x, y, w, h}, 0.35f, 8, body);
            DrawRectangleRoundedLines({x, y, w, h}, 0.35f, 8, border);
            float r = w * 0.1f;
            DrawCircleV({x + w + 2, cy}, r, body);
            DrawCircleV({x + w + 2, cy}, r, border);
            break;
        }

        case GateType::NOR:
        {
            Vector2 v[5] =
            {
                {cx - hw * 0.2f, y},
                {cx + hw * 0.65f, y + hh * 0.2f},
                {cx + hw * 0.85f, cy},
                {cx + hw * 0.65f, y + h * 0.8f},
                {cx - hw * 0.2f, y + h}
            };
            DrawFilledPolygon(v, body);
            DrawPolyOutline(v, border, 2.5f);
            float r = w * 0.1f;
            DrawCircleV({x + w + 2, cy}, r, body);
            DrawCircleV({x + w + 2, cy}, r, border);
            break;
        }

        case GateType::XNOR:
        {
            Vector2 v[5] =
            {
                {cx - hw * 0.25f, y},
                {cx + hw * 0.65f, y + hh * 0.2f},
                {cx + hw * 0.85f, cy},
                {cx + hw * 0.65f, y + h * 0.8f},
                {cx - hw * 0.25f, y + h}
            };
            DrawFilledPolygon(v, body);
            DrawPolyOutline(v, border, 2.5f);
            DrawLineEx
            (
                {cx - hw * 0.55f, y + hh * 0.15f},
                {cx - hw * 0.55f, y + h * 0.85f}, 2.5f, border
            );
            float r = w * 0.1f;
            DrawCircleV({x + w + 2, cy}, r, body);
            DrawCircleV({x + w + 2, cy}, r, border);
            break;
        }
    }

    const char* label = GateTypeToString(gate.type);
    int font_size = static_cast<int>(hh * 0.55f);
    if (font_size < 8) font_size = 8;
    DrawTextCentered
    (
        GetFontDefault(), label, {x, y, w, h}, font_size,
        GateColors::label
    );
}
