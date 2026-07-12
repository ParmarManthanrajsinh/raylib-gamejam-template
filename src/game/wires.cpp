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
        [[maybe_unused]] const int input_bits[4],
        int dragging_gate_id = -1,
        Vector2 drag_pos = {-1000, -1000}
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
                    if (g.id == dragging_gate_id)
                        return GetGateOutputPinPos(drag_pos);
                    
                    return GetGateOutputPinPos(g);
                }
            }
        }
        return {};
    }

    Vector2 GetWireTargetPos
    (
        const t_Wire& w, 
        const std::vector<t_Gate>& gates, 
        int dragging_gate_id = -1, 
        Vector2 drag_pos = {-1000, -1000}
    )
    {
        if (w.to_type == 0)
        {
            for (const auto& g : gates)
            {
                if (g.id == w.to_id)
                {
                    if (g.id == dragging_gate_id) 
                        return GetGateInputPinPos(g.type, drag_pos, w.to_pin);
                    
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

    int GetWireSignal
    (
        const t_Wire& w, 
        const int input_bits[4], 
        const std::unordered_map<int, int>& gate_outputs
    )
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
        float dist = fabsf(p2.x - p1.x) * 0.5f;
        float offset = fmaxf(dist, 40.0f);
        
        Vector2 cp1 = {p1.x + offset, p1.y};
        Vector2 cp2 = {p2.x - offset, p2.y};

        Color draw_color = color;
        draw_color.a = static_cast<unsigned char>(255 * alpha);
        Color glow_color = color;
        glow_color.a = static_cast<unsigned char>(100 * alpha);
        
        Color base_dark = {40, 50, 65, static_cast<unsigned char>(255 * alpha)};
        
        line_width = has_signal ? 3.5f : 3.0f;
        draw_color = has_signal ? 
            ColorAlpha({0, 255, 255, 255}, 0.9f * alpha) : 
            ColorAlpha({160, 175, 190, 255}, alpha);
        
        int segments = 24;
        Vector2 prev = p1; 
        for (int i = 1; i <= segments; i++)
        {
            float t = static_cast<float>(i) / static_cast<float>(segments);
            float u = 1.0f - t;
            float tt = t * t;
            float uu = u * u;
            float uuu = uu * u;
            float ttt = tt * t;

            Vector2 cur = 
            {
                uuu * p1.x + 3 * uu * t * cp1.x + 3 * u * tt * cp2.x + ttt * p2.x,
                uuu * p1.y + 3 * uu * t * cp1.y + 3 * u * tt * cp2.y + ttt * p2.y
            };
            
            if (glow) DrawLineEx(prev, cur, line_width + 8, glow_color);
            DrawLineEx(prev, cur, 6.0f, base_dark); // dark base
            DrawLineEx(prev, cur, line_width, draw_color);
            
            prev = cur;
        }

        if (has_signal)
        {
            float t_offset = fmodf(anim_time * 1.5f, 1.0f);
            for (int i = 0; i < 4; i++)
            {
                float t = fmodf(t_offset + static_cast<float>(i) * 0.25f, 1.0f);
                float u = 1.0f - t;
                float tt = t * t;
                float uu = u * u;
                float uuu = uu * u;
                float ttt = tt * t;

                Vector2 dot_pos = 
                {
                    uuu * p1.x + 3 * uu * t * cp1.x + 3 * u * tt * cp2.x + ttt * p2.x,
                    uuu * p1.y + 3 * uu * t * cp1.y + 3 * u * tt * cp2.y + ttt * p2.y
                };
                
                DrawCircleV(dot_pos, 7.0f, ColorAlpha({0, 255, 255, 255}, 0.5f * alpha));
                DrawCircleV(dot_pos, 3.5f, ColorAlpha(WHITE, alpha));
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
    float anim_time,
    int dragging_gate_id,
    Vector2 drag_pos
)
{
    for (const auto& w : wires)
    {
        Vector2 from = GetWireSourcePos(w, gates, input_bits, dragging_gate_id, drag_pos);
        Vector2 to = GetWireTargetPos(w, gates, dragging_gate_id, drag_pos);
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
    float pulse = 0.5f + 0.5f * sinf(GetTime() * 10.0f);
    Color ghost_color = ColorAlpha({0, 245, 212, 255}, 0.3f + 0.5f * pulse);
    DrawBezierWire
    (
        from, 
        mouse_pos, 
        ghost_color, 
        1.0f, 
        3.0f + pulse * 1.5f, 
        true, 
        GetTime() * 2.0f, 
        true
    );
}