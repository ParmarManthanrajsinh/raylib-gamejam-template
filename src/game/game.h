#pragma once

#include <raylib.h>
#include <vector>
#include <unordered_map>

#include "hex_grid.h"
#include "gates.h"
#include "circuit.h"
#include "wires.h"
// #include "ui.hpp"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

struct PinHit 
{
    int  source_type = -1;
    int  source_id   = 0;
    int  pin_index   = 0;
    bool is_input    = false;

    bool IsValid() const { return source_type >= 0; }
};

class Game 
{
public:
    Game();
    ~Game();

    void Update();
    void Draw();
    bool ShouldClose() const;

private:
    // Core state
    std::vector<Gate> gates;
    std::vector<Wire> wires;
    int input_bits[4];
    int output_bits[4];
    std::unordered_map<int, int> gate_outputs;
    int target_hex;
    int gate_id_counter;
    bool solved;
    float anim_time;
    float solved_pulse;

    // UI state
    int selected_gate_index;   // -1 = none
    WireDragState wire_drag_state;
    HexCell hovered_cell;
    PinHit hovered_pin;
    Vector2 mouse_pos;

    // Helpers
    void Reset();
    void Evaluate();
    Gate* FindGateAt(int row, int col);
    Gate* FindGateById(int id);
    PinHit FindPinAt(Vector2 pos);
    void RemoveWiresForGate(int gate_id);

    // Event handlers
    void HandleClick(Vector2 pos);
    void HandleRightClick(Vector2 pos);
};
