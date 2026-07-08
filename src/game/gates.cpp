#include "gates.h"
#include "hex_grid.h"
#include "text_util.h"
#include <cmath>
#include <raylib.h>

static Color k_body_on  = { 58, 16, 32, 255 };
static Color k_body_off = { 26, 26, 53, 255 };
static Color k_border_on  = { 255, 0, 64, 255 };
static Color k_border_off = { 68, 85, 119, 255 };
static Color k_label      = { 255, 255, 255, 255 };

int GetGateInputCount(GateType type) {
    return type == GateType::NOT ? 1 : 2;
}

float GetInputNodeY(int index) {
    float spacing = K_GRID_H / 5;
    return K_GRID_Y + spacing + index * spacing;
}

Vector2 GetInputNodeOutputPin(int index) {
    return { K_INPUT_PIN_X, GetInputNodeY(index) };
}

Vector2 GetOutputNodeInputPin(int bit_index) {
    float spacing = 22.0f;
    float base_y = K_OUTPUT_CENTER_Y - (1.5f * spacing);
    return { K_OUTPUT_PIN_X, base_y + bit_index * spacing };
}

Vector2 GetGateInputPinPos(const Gate& gate, int pin_index) {
    Vector2 c = GetHexCenter(gate.row, gate.col);
    float pin_x = c.x - K_HEX_SIZE * K_SQRT3 / 2;
    int input_count = GetGateInputCount(gate.type);

    if (input_count == 1) {
        return { pin_x + 4, c.y };
    }

    float spacing = K_HEX_SIZE * K_SQRT3 * 0.55f;
    float start_y = c.y - spacing / 2;
    float frac = (pin_index + 0.5f) / input_count;
    return { pin_x + 4, start_y + frac * spacing };
}

Vector2 GetGateOutputPinPos(const Gate& gate) {
    Vector2 c = GetHexCenter(gate.row, gate.col);
    return { c.x + K_HEX_SIZE * K_SQRT3 / 2 - 4, c.y };
}

static void DrawGlow(Vector2 center, float radius, Color color) {
    // Simple glow as layered circles
    for (int i = 3; i >= 0; i--) {
        float r = radius + i * 4;
        Color c = color;
        c.a = 20;
        DrawCircleV(center, r, c);
    }
}

static void DrawFilledPolygon(const Vector2* verts, int count, Color fill) {
    if (count < 3) return;
    for (int i = 1; i < count - 1; i++) {
        DrawTriangle(verts[0], verts[i], verts[i + 1], fill);
    }
}

static void DrawPolyOutline(const Vector2* verts, int count, Color color, float thickness) {
    for (int i = 0; i < count; i++) {
        int next = (i + 1) % count;
        DrawLineEx(verts[i], verts[next], thickness, color);
    }
}

void DrawGateShape(const Gate& gate, float x, float y, float w, float h, int output_val, float alpha) {
    float cx = x + w / 2;
    float cy = y + h / 2;
    float hw = w / 2;
    float hh = h / 2;

    Color body   = output_val ? k_body_on  : k_body_off;
    Color border = output_val ? k_border_on : k_border_off;
    if (alpha < 1.0f) {
        body.a   = (unsigned char)((float)body.a   * alpha);
        border.a = (unsigned char)((float)border.a * alpha);
    }

    // Glow for active gates
    if (output_val) {
        DrawGlow({ cx, cy }, hw, k_border_on);
    }

    switch (gate.type) {
        case GateType::AND: {
            // D-shape: flat left, rounded right
            DrawRectangleRounded({ x, y, w, h }, 0.35f, 8, body);
            DrawRectangleRoundedLines({ x, y, w, h }, 0.35f, 8, border);
            break;
        }

        case GateType::OR: {
            // Shield shape
            Vector2 v[5] = {
                { cx - hw * 0.2f, y },
                { cx + hw * 0.7f, y + hh * 0.2f },
                { cx + hw * 0.9f, cy },
                { cx + hw * 0.7f, y + h * 0.8f },
                { cx - hw * 0.2f, y + h }
            };
            DrawFilledPolygon(v, 5, body);
            DrawPolyOutline(v, 5, border, 2.5f);
            break;
        }

        case GateType::NOT: {
            // Right-pointing triangle
            Vector2 v[3] = {
                { x, y },
                { x, y + h },
                { x + w, cy }
            };
            DrawTriangle(v[0], v[1], v[2], body);
            DrawPolyOutline(v, 3, border, 2.5f);
            break;
        }

        case GateType::XOR: {
            // OR shape with an extra curve on the left side
            Vector2 v[5] = {
                { cx - hw * 0.25f, y },
                { cx + hw * 0.7f, y + hh * 0.2f },
                { cx + hw * 0.9f, cy },
                { cx + hw * 0.7f, y + h * 0.8f },
                { cx - hw * 0.25f, y + h }
            };
            DrawFilledPolygon(v, 5, body);
            DrawPolyOutline(v, 5, border, 2.5f);
            // Extra left curve line
            DrawLineEx(
                { cx - hw * 0.55f, y + hh * 0.15f },
                { cx - hw * 0.55f, y + h * 0.85f },
                2.5f, border
            );
            break;
        }

        case GateType::NAND: {
            DrawRectangleRounded({ x, y, w, h }, 0.35f, 8, body);
            DrawRectangleRoundedLines({ x, y, w, h }, 0.35f, 8, border);
            float r = w * 0.1f;
            DrawCircleV({ x + w + 2, cy }, r, body);
            DrawCircleV({ x + w + 2, cy }, r, border);
            break;
        }

        case GateType::NOR: {
            Vector2 v[5] = {
                { cx - hw * 0.2f, y },
                { cx + hw * 0.65f, y + hh * 0.2f },
                { cx + hw * 0.85f, cy },
                { cx + hw * 0.65f, y + h * 0.8f },
                { cx - hw * 0.2f, y + h }
            };
            DrawFilledPolygon(v, 5, body);
            DrawPolyOutline(v, 5, border, 2.5f);
            float r = w * 0.1f;
            DrawCircleV({ x + w + 2, cy }, r, body);
            DrawCircleV({ x + w + 2, cy }, r, border);
            break;
        }

        case GateType::XNOR: {
            Vector2 v[5] = {
                { cx - hw * 0.25f, y },
                { cx + hw * 0.65f, y + hh * 0.2f },
                { cx + hw * 0.85f, cy },
                { cx + hw * 0.65f, y + h * 0.8f },
                { cx - hw * 0.25f, y + h }
            };
            DrawFilledPolygon(v, 5, body);
            DrawPolyOutline(v, 5, border, 2.5f);
            DrawLineEx(
                { cx - hw * 0.55f, y + hh * 0.15f },
                { cx - hw * 0.55f, y + h * 0.85f },
                2.5f, border
            );
            float r = w * 0.1f;
            DrawCircleV({ x + w + 2, cy }, r, body);
            DrawCircleV({ x + w + 2, cy }, r, border);
            break;
        }
    }

    const char* label = GateTypeToString(gate.type);
    int font_size = (int)(hh * 0.55f);
    if (font_size < 8) font_size = 8;
    DrawTextCentered(GetFontDefault(), label, { x, y, w, h }, font_size, k_label);
}
