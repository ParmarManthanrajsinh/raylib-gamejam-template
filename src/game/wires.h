#pragma once
#include "circuit.h"
#include "gates.h"
#include <raylib.h>
#include <unordered_map>
#include <vector>

struct t_WireDragState
{
    int from_type = 0;
    int from_id = 0;
    int from_pin = 0;
    float start_x = 0;
    float start_y = 0;
    bool active = false;

    bool IsActive() const
    {
        return active;
    }
};

void DrawWire(const t_Wire& w, int signal_val, float anim_time);
void DrawAllWires
(
    const std::vector<t_Wire>& wires,
    const std::vector<t_Gate>& gates,
    const int input_bits[4],
    const std::unordered_map<int, int>& gate_outputs,
    float anim_time,
    int dragging_gate_id = -1,
    Vector2 drag_pos = {-1000, -1000}
);
void DrawGhostWire(Vector2 from, Vector2 mouse_pos);
