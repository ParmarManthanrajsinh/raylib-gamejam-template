#pragma once
#include <raylib.h>
#include <vector>
#include <unordered_map>
#include "gates.h"
#include "circuit.h"

struct WireDragState {
    int   from_type = 0;
    int   from_id   = 0;
    int   from_pin  = 0;
    float start_x   = 0;
    float start_y   = 0;
    bool  active    = false;

    bool IsActive() const { return active; }
};

void DrawWire(const Wire& w, int signal_val, float anim_time);
void DrawAllWires(const std::vector<Wire>& wires,
                  const std::vector<Gate>& gates,
                  const int input_bits[4],
                  const std::unordered_map<int, int>& gate_outputs,
                  float anim_time);
void DrawGhostWire(Vector2 from, Vector2 mouse_pos);
