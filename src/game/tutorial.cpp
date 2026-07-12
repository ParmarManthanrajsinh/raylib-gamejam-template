#include "tutorial.h"
#include "hex_grid.h"
#include "gates.h"
#include "assets.h"
#include "text_util.h"
#include <cmath>
#include <cstdio> // IWYU pragma: keep

namespace
{
    Rectangle GetPaletteBtnRect(int index)
    {
        constexpr float btn_w = 68.0f;
        constexpr float spacing = 4.0f;
        float total_gate = GATE_COUNT * btn_w + (GATE_COUNT - 1) * spacing;
        float total = total_gate + spacing + 90.0f;
        float start_x = (SCREEN_WIDTH - total) / 2.0f;
        return {start_x + index * (btn_w + spacing), 650.0f + 12.0f, btn_w, 32.0f};
    }

    void DrawHighlightRing(Vector2 center, float radius, Color c, float t)
    {
        float pulse = (sinf(t * 5.0f) + 1.0f) * 0.5f;
        float r = radius + pulse * 5.0f;
        DrawCircleLinesV(center, r, Fade(c, 0.7f + pulse * 0.3f));
        DrawCircleLinesV(center, r + 2.0f, Fade(c, 0.3f + pulse * 0.2f));
        DrawCircleV(center, radius * 0.5f, Fade(c, 0.15f * pulse));
    }

    void DrawArrowBetween(Vector2 from, Vector2 to, Color c, float t)
    {
        float phase = fmodf(t * 3.0f, 1.0f);
        float mid = phase;
        Vector2 dir = {to.x - from.x, to.y - from.y};
        float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
        if (len < 1.0f) return;
        dir.x /= len; dir.y /= len;

        Vector2 mid_pt = {from.x + dir.x * len * mid, from.y + dir.y * len * mid};
        float arrow_alpha = 1.0f - fabsf(mid - 0.5f) * 2.0f;
        DrawCircleV(mid_pt, 4.0f * arrow_alpha, Fade(c, arrow_alpha));

        // Arrow head at end
        float head_len = 10.0f;
        float head_angle = PI / 6;
        Vector2 head_right =
        {
            to.x - dir.x * head_len * cosf(head_angle) + dir.y * head_len * sinf(head_angle),
            to.y - dir.y * head_len * cosf(head_angle) - dir.x * head_len * sinf(head_angle)
        };
        Vector2 head_left =
        {
            to.x - dir.x * head_len * cosf(head_angle) - dir.y * head_len * sinf(head_angle),
            to.y - dir.y * head_len * cosf(head_angle) + dir.x * head_len * sinf(head_angle)
        };
        DrawLineEx(to, head_right, 2.5f, c);
        DrawLineEx(to, head_left, 2.5f, c);
        DrawLineEx(from, to, 1.5f + 1.0f * sinf(t * 2.0f), Fade(c, 0.3f));
    }

    void DrawStepText(const char* line1, const char* line2, float t)
    {
        Font font = GetGameFont();
        float alpha = 0.8f + 0.2f * sinf(t * 3.0f);
        Color c = ColorAlpha({0, 255, 255, 255}, alpha);

        if (line1)
        {
            DrawTextShadowed(font, line1, 90, 80, 15, c);
        }
        if (line2)
        {
            DrawTextShadowed
            (
                font, 
                line2, 
                90, 
                98, 
                13, 
                ColorAlpha({200, 220, 255, 255}, alpha * 0.8f)
            );
        }
    }
}

void DrawTutorialOverlay
(
    const t_TutorialState& ts, 
    float anim_time, 
    const std::vector<t_Gate>& gates
)
{
    if (ts.current_step == 0) return;

    Font font = GetGameFont();
    float t = anim_time;

    if (ts.current_step == 1)
    {
        // Dim edges, highlight center area + info bar + in/out nodes
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.3f));

        // Highlight info bar
        Rectangle info_bar = {(SCREEN_WIDTH - 400.0f) / 2.0f, 12.0f, 400.0f, 44.0f};
        DrawRectangleRounded(info_bar, 0.5f, 4, ColorAlpha({0, 255, 255, 255}, 0.06f));
        DrawRectangleRoundedLines
        (
            info_bar, 
            0.5f, 
            4, 
            Fade({0, 255, 255, 255}, 0.3f + 0.2f * sinf(t * 3.0f))
        );

        // Highlight input nodes
        for (int i = 0; i < 4; i++)
        {
            Vector2 pin = GetInputNodeOutputPin(i);
            DrawHighlightRing(pin, 14.0f, {0, 200, 255, 255}, t);
        }

        // Highlight output node
        for (int b = 0; b < 4; b++)
        {
            Vector2 pin = GetOutputNodeInputPin(b);
            DrawHighlightRing(pin, 12.0f, {0, 255, 136, 255}, t + b * 0.5f);
        }

        DrawStepText
        (
            "YOUR GOAL: Match the TARGET hex value", 
            "Use logic gates to transform INPUT bits", 
            t
        );
    }
    else if (ts.current_step == 2)
    {
        // Dim everything except palette area
        DrawRectangle(0, 0, SCREEN_WIDTH, PALETTE_Y, Fade(BLACK, 0.4f));

        // Highlight OR gate in palette (index 1)
        Rectangle pal_btn = GetPaletteBtnRect(1);
        DrawRectangleRounded
        (
            {pal_btn.x - 4, pal_btn.y - 4, pal_btn.width + 8, pal_btn.height + 8},
            0.3f, 6, ColorAlpha({200, 100, 255, 255}, 0.12f + 0.08f * sinf(t * 4.0f))
        );
        DrawRectangleRoundedLines
        (
            {pal_btn.x - 4, pal_btn.y - 4, pal_btn.width + 8, pal_btn.height + 8},
            0.3f, 6, Fade({200, 100, 255, 255}, 0.5f + 0.3f * sinf(t * 4.0f))
        );

        // Arrow pointing down to NOT gate
        Vector2 arrow_start = {SCREEN_WIDTH / 2.0f, PALETTE_Y - 40.0f};
        Vector2 arrow_end = 
        {
            pal_btn.x + pal_btn.width / 2.0f, 
            pal_btn.y + pal_btn.height + 8.0f
        };
        DrawArrowBetween(arrow_start, arrow_end, {255, 150, 50, 255}, t);

        DrawStepText
        (
            "Pick the OR gate from the palette", 
            "Click the orange-highlighted button below", 
            t
        );
    }
    else if (ts.current_step == 3)
    {
        // Dim palette area
        DrawRectangle
        (
            0, 
            PALETTE_Y - 4, 
            SCREEN_WIDTH, 
            SCREEN_HEIGHT - PALETTE_Y + 4, 
            Fade(BLACK, 0.4f)
        );

        // Highlight grid area
        Rectangle grid_rect = GetGridRect();
        DrawRectangleRounded(grid_rect, 0.1f, 4, ColorAlpha({0, 255, 255, 255}, 0.04f));
        DrawRectangleRoundedLines
        (
            grid_rect, 
            0.1f, 
            4, 
            Fade({0, 255, 200, 255}, 0.3f + 0.2f * sinf(t * 3.0f))
        );

        // Arrow from palette to grid
        float arrow_y = PALETTE_Y - 4.0f;
        Vector2 arrow_start = {SCREEN_WIDTH / 2.0f, arrow_y};
        Vector2 arrow_end = {SCREEN_WIDTH / 2.0f, GRID_Y + GRID_H / 2.0f};
        DrawArrowBetween(arrow_start, arrow_end, {0, 255, 200, 255}, t);

        DrawStepText
        (
            "Place the OR gate on an empty hex cell", 
            "Click any highlighted cell on the grid", 
            t
        );
    }
    else if (ts.current_step == 4)
    {
        // Dim irrelevant areas
        DrawRectangle(0, 0, SCREEN_WIDTH, GRID_Y - 10, Fade(BLACK, 0.15f));
        DrawRectangle
        (
            0, 
            PALETTE_Y - 4, 
            SCREEN_WIDTH, 
            SCREEN_HEIGHT - PALETTE_Y + 4, 
            Fade(BLACK, 0.5f)
        );

        // Pulse input node output pins (left side)
        for (int i = 0; i < 4; i++)
        {
            Vector2 pin = GetInputNodeOutputPin(i);
            DrawHighlightRing(pin, 14.0f, {0, 255, 255, 255}, t + i * 0.3f);
        }

        // Pulse the placed gate's input pin
        for (const auto& gate : gates)
        {
            if (gate.row == ts.placed_gate_row && gate.col == ts.placed_gate_col)
            {
                for (int p = 0; p < GetGateInputCount(gate.type); p++)
                {
                    Vector2 gpin = GetGateInputPinPos(gate, p);
                    DrawHighlightRing(gpin, 12.0f, {255, 200, 50, 255}, t + p * 0.5f);
                }
                // Draw connecting line hint between input and gate
                Vector2 from = {70.0f, GetInputNodeY(1)};
                Vector2 to = GetGateInputPinPos(gate, 0);
                DrawLineEx
                (
                    from, 
                    to, 
                    1.0f, 
                    Fade({0, 255, 255, 255}, 0.15f + 0.1f * sinf(t * 2.0f))
                );
                break;
            }
        }

        DrawStepText
        (
            "STEP 4: Wire INPUT to GATE", 
            "Click a glowing INPUT pin, then the gate's input pin", 
            t
        );
    }
    else if (ts.current_step == 5)
    {
        // Dim irrelevant areas
        DrawRectangle(0, 0, SCREEN_WIDTH, GRID_Y - 10, Fade(BLACK, 0.15f));
        DrawRectangle
        (
            0, 
            PALETTE_Y - 4, 
            SCREEN_WIDTH, 
            SCREEN_HEIGHT - PALETTE_Y + 4, 
            Fade(BLACK, 0.5f)
        );

        // Pulse the placed gate's output pin
        for (const auto& gate : gates)
        {
            if (gate.row == ts.placed_gate_row && gate.col == ts.placed_gate_col)
            {
                Vector2 gpin = GetGateOutputPinPos(gate);
                DrawHighlightRing(gpin, 14.0f, {255, 200, 50, 255}, t);
                break;
            }
        }

        // Pulse output node input pins (right side)
        for (int b = 0; b < 4; b++)
        {
            Vector2 pin = GetOutputNodeInputPin(b);
            DrawHighlightRing(pin, 12.0f, {0, 255, 136, 255}, t + b * 0.3f);
        }

        DrawStepText
        (
            "STEP 5: Wire GATE to OUTPUT", 
            "Click the gate's OUTPUT pin, then an OUTPUT node pin", 
            t
        );
    }
    else if (ts.current_step == 6)
    {
        // Celebration overlay
        float pulse = 0.6f + 0.4f * sinf(t * 2.0f);
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.1f));

        // "READY!" text with glow
        const char* ready = "YOU'RE READY!";
        Vector2 size = MeasureTextEx(font, ready, 32.0f, 1.0f);
        float cx = (SCREEN_WIDTH - size.x) / 2.0f;
        float cy = SCREEN_HEIGHT / 2.0f - 20.0f;
        for (int r = 20; r > 0; r -= 4)
        {
            DrawCircleV
            (
                {cx + size.x / 2.0f, cy + 16.0f}, 
                static_cast<float>(r), 
                ColorAlpha({0, 255, 200, 255}, static_cast<float>(20 - r) / 100.0f * pulse)
            );
        }
        DrawTextShadowed
        (
            font, 
            ready, 
            static_cast<int>(cx), 
            static_cast<int>(cy), 
            32, 
            ColorAlpha({0, 255, 200, 255}, pulse)
        );
        DrawTextShadowed
        (
            font, 
            "Get ready for real puzzles!", 
            static_cast<int>(cx), 
            static_cast<int>(cy + 40), 
            16, 
            ColorAlpha({200, 220, 255, 255}, 0.7f * pulse)
        );
    }
}
