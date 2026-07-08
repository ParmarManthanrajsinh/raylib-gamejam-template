#include "wires.h"
#include "hex_grid.h" // IWYU pragma: keep
#include <cmath>
#include <algorithm> // IWYU pragma: keep
#include <raylib.h>

static Vector2 GetWireSourcePos
(
    const t_Wire& w,
    const std::vector<t_Gate>& gates,
    const int input_bits[4]
) {
    if (w.from_type == 0) 
    { 
        return GetInputNodeOutputPin(w.from_id);
    }
    if (w.from_type == 1) 
    { 
        for (const auto& g : gates) 
        {
            if (g.id == w.from_id) return GetGateOutputPinPos(g);
        }
    }
    return {};
}

static Vector2 GetWireTargetPos(const t_Wire& w,
                                 const std::vector<t_Gate>& gates) {
    if (w.to_type == 0) { // Gate input
        for (const auto& g : gates) {
            if (g.id == w.to_id) return GetGateInputPinPos(g, w.to_pin);
        }
    }
    if (w.to_type == 1) { // Output node
        return GetOutputNodeInputPin(w.to_pin);
    }
    return {};
}

static int GetWireSignal(const t_Wire& w,
                          const int input_bits[4],
                          const std::unordered_map<int, int>& gate_outputs) {
    if (w.from_type == 0) return input_bits[w.from_id];
    if (w.from_type == 1) {
        auto it = gate_outputs.find(w.from_id);
        if (it != gate_outputs.end()) return it->second;
    }
    return 0;
}

static void DrawOrthogonalWire(Vector2 p1, Vector2 p2, Color color, float alpha,
                                float line_width, bool glow, float anim_time, bool has_signal) {
    float mid_x = p1.x + 20;
    float target_approach_x = p2.x - 16;

    float route_x;
    if (target_approach_x > mid_x + 10) {
        route_x = mid_x;
    } else {
        route_x = fminf(mid_x, p1.x + fabsf(p2.x - p1.x) * 0.4f);
    }

    // Glow layer for active signals
    if (glow) {
        Color glow_color = color;
        glow_color.a = (unsigned char)(100 * alpha);
        DrawLineEx({ p1.x, p1.y }, { route_x, p1.y }, line_width + 4, glow_color);
        DrawLineEx({ route_x, p1.y }, { route_x, p2.y }, line_width + 4, glow_color);
        DrawLineEx({ route_x, p2.y }, { p2.x, p2.y }, line_width + 4, glow_color);
    }

    Color draw_color = color;
    draw_color.a = (unsigned char)(255 * alpha);

    DrawLineEx({ p1.x, p1.y }, { route_x, p1.y }, line_width, draw_color);
    DrawLineEx({ route_x, p1.y }, { route_x, p2.y }, line_width, draw_color);
    DrawLineEx({ route_x, p2.y }, { p2.x, p2.y }, line_width, draw_color);

    // Signal dots
    if (has_signal) {
        float seg1_len = fabsf(route_x - p1.x);
        float seg2_len = fabsf(p2.y - p1.y);
        float seg3_len = fabsf(p2.x - route_x);
        float total_len = seg1_len + seg2_len + seg3_len;
        int num_dots = (int)(total_len / 25);
        if (num_dots < 1) num_dots = 1;

        for (int i = 0; i < num_dots; i++) {
            float t = fmodf((float)i / num_dots + anim_time * 0.8f, 1.0f);
            float dist = t * total_len;
            Vector2 dot_pos;

            if (dist < seg1_len) {
                float frac = dist / fmaxf(seg1_len, 0.001f);
                dot_pos = { p1.x + (route_x - p1.x) * frac, p1.y };
            } else if (dist < seg1_len + seg2_len) {
                float frac = (dist - seg1_len) / fmaxf(seg2_len, 0.001f);
                dot_pos = { route_x, p1.y + (p2.y - p1.y) * frac };
            } else {
                float frac = (dist - seg1_len - seg2_len) / fmaxf(seg3_len, 0.001f);
                dot_pos = { route_x + (p2.x - route_x) * frac, p2.y };
            }

            DrawCircleV(dot_pos, 3, WHITE);
        }
    }
}

void DrawWire(const t_Wire& w, int signal_val, float anim_time) {
    (void)w;
    (void)signal_val;
    (void)anim_time;
    // This function is kept for the unified draw below
}

void DrawAllWires(const std::vector<t_Wire>& wires,
                  const std::vector<t_Gate>& gates,
                  const int input_bits[4],
                  const std::unordered_map<int, int>& gate_outputs,
                  float anim_time) {
    for (const auto& w : wires) {
        Vector2 from = GetWireSourcePos(w, gates, input_bits);
        Vector2 to = GetWireTargetPos(w, gates);
        if (from.x == 0 && from.y == 0 && to.x == 0 && to.y == 0) continue;

        int signal = GetWireSignal(w, input_bits, gate_outputs);
        bool active = signal == 1;
        float pulse = active ? 0.6f + 0.4f * sinf(anim_time * 6 + w.from_id * 1.7f) : 0;
        float alpha = active ? 0.8f + pulse * 0.2f : 0.6f;
        Color color = active ? Color{ 255, 0, 64, 255 } : Color{ 102, 119, 153, 255 };
        float line_width = active ? 3 + pulse * 2 : 2;

        DrawOrthogonalWire(from, to, color, alpha, line_width, active, anim_time, active);
    }
}

void DrawGhostWire(Vector2 from, Vector2 mouse_pos) {
    Color ghost_color = { 0, 245, 212, 153 }; // rgba(0,245,212,0.6)
    float mid_x = from.x + 20;

    DrawLineEx({ from.x, from.y }, { mid_x, from.y }, 2, ghost_color);
    DrawLineEx({ mid_x, from.y }, { mid_x, mouse_pos.y }, 2, ghost_color);
    DrawLineEx({ mid_x, mouse_pos.y }, { mouse_pos.x, mouse_pos.y }, 2, ghost_color);
}
