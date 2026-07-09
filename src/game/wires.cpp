#include "wires.h"
#include "hex_grid.h" // IWYU pragma: keep
#include <algorithm>  // IWYU pragma: keep
#include <cmath>
#include <raylib.h>

namespace
{
    Vector2 GetWireSourcePos
    (
        const t_Wire& w,
        const std::vector<t_Gate>& gates,
        [[maybe_unused]] const int input_bits[4]
    )
    {
        if (w.from_type == 0)
        {
            return GetInputNodeOutputPin(w.from_id);
        }
        if (w.from_type == 1)
        {
            for (const auto& g : gates)
            {
                if (g.id == w.from_id)
                {
                    return GetGateOutputPinPos(g);
                }
            }
        }
        return {};
    }

    Vector2 GetWireTargetPos(const t_Wire& w, const std::vector<t_Gate>& gates)
    {
        if (w.to_type == 0)
        {
            for (const auto& g : gates)
            {
                if (g.id == w.to_id)
                {
                    return GetGateInputPinPos(g, w.to_pin);
                }
            }
        }
        if (w.to_type == 1)
        {
            return GetOutputNodeInputPin(w.to_pin);
        }
        return {};
    }

    int GetWireSignal(const t_Wire& w, const int input_bits[4], const std::unordered_map<int, int>& gate_outputs)
    {
        if (w.from_type == 0)
        {
            return input_bits[w.from_id];
        }
        if (w.from_type == 1)
        {
            auto it = gate_outputs.find(w.from_id);
            if (it != gate_outputs.end())
            {
                return it->second;
            }
        }
        return 0;
    }

    void DrawBezierWire
    (
        Vector2 p1,
        Vector2 p2,
        Color color,
        float alpha,
        float line_width,
        bool glow,
        float anim_time,
        bool has_signal
    )
    {
        float mid_x = p1.x + 20;
        float target_approach_x = p2.x - 16;
        float route_x = target_approach_x > mid_x + 10 ? mid_x : fminf(mid_x, p1.x + fabsf(p2.x - p1.x) * 0.4f);

        Vector2 p1_end = {route_x, p1.y};
        Vector2 p2_start = {route_x, p2.y};

        Color draw_color = color;
        draw_color.a = static_cast<unsigned char>(255 * alpha);
        Color glow_color = color;
        glow_color.a = static_cast<unsigned char>(100 * alpha);

        if (glow)
        {
            DrawLineEx(p1, p1_end, line_width + 6, glow_color);
            DrawLineBezier(p1_end, p2_start, line_width + 6, glow_color);
            DrawLineEx(p2_start, p2, line_width + 6, glow_color);
        }

        DrawLineEx(p1, p1_end, line_width, draw_color);
        DrawLineBezier(p1_end, p2_start, line_width, draw_color);
        DrawLineEx(p2_start, p2, line_width, draw_color);

        if (has_signal)
        {
            // Simplified dots along Bezier curve
            float t_offset = fmodf(anim_time * 1.5f, 1.0f);
            for (int i = 0; i < 4; i++)
            {
                float t = fmodf(t_offset + static_cast<float>(i) * 0.25f, 1.0f);
                Vector2 dot_pos;
                // Approximate position based on t
                if (t < 0.2f)
                    dot_pos = {p1.x + (p1_end.x - p1.x) * (t / 0.2f), p1.y};
                else if (t < 0.8f)
                {
                    float ct = (t - 0.2f) / 0.6f; // bezier t
                    float u = 1.0f - ct;
                    float tt = ct * ct;
                    float uu = u * u;
                    float uuu = uu * u;
                    float ttt = tt * ct;
                    
                    Vector2 cp1 = {p1_end.x, p1_end.y + (p2_start.y - p1_end.y) * 0.3f};
                    Vector2 cp2 = {p2_start.x, p2_start.y - (p2_start.y - p1_end.y) * 0.3f};
                    
                    dot_pos.x = uuu * p1_end.x + 3 * uu * ct * cp1.x + 3 * u * tt * cp2.x + ttt * p2_start.x;
                    dot_pos.y = uuu * p1_end.y + 3 * uu * ct * cp1.y + 3 * u * tt * cp2.y + ttt * p2_start.y;
                }
                else
                    dot_pos = {p2_start.x + (p2.x - p2_start.x) * ((t - 0.8f) / 0.2f), p2_start.y};
                
                DrawCircleV(dot_pos, 4, WHITE);
            }
        }
    }
}

void DrawWire(const t_Wire& w, int signal_val, float anim_time)
{
    (void)w;
    (void)signal_val;
    (void)anim_time;
    // This function is kept for the unified draw below
}

void DrawAllWires
(
    const std::vector<t_Wire>& wires,
    const std::vector<t_Gate>& gates,
    const int input_bits[4],
    const std::unordered_map<int, int>& gate_outputs,
    float anim_time
)
{
    for (const auto& w : wires)
    {
        Vector2 from = GetWireSourcePos(w, gates, input_bits);
        Vector2 to = GetWireTargetPos(w, gates);
        if (from.x == 0 && from.y == 0 && to.x == 0 && to.y == 0)
        {
            continue;
        }

        int signal = GetWireSignal(w, input_bits, gate_outputs);
        bool active = signal == 1;
        float pulse = active ? 0.6f + 0.4f * sinf(anim_time * 6 + w.from_id * 1.7f) : 0;
        float alpha = active ? 0.8f + pulse * 0.2f : 0.6f;
        Color color = active ? Color{255, 0, 64, 255} : Color{102, 119, 153, 255};
        float line_width = active ? 3 + pulse * 2 : 2;

        DrawBezierWire(from, to, color, alpha, line_width, active, anim_time, active);
    }
}

void DrawGhostWire(Vector2 from, Vector2 mouse_pos)
{
    Color ghost_color = {0, 245, 212, 200}; // rgba(0,245,212,0.8)
    DrawBezierWire(from, mouse_pos, ghost_color, 1.0f, 3.0f, true, 0.0f, false);
}