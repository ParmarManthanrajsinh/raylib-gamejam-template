#include "game.hpp"
#include "hex_grid.hpp"
#include "gates.hpp"
#include "circuit.hpp"
#include "wires.hpp"
#include "ui.hpp"
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <raylib.h>

static float Dist(Vector2 a, Vector2 b) {
    float dx = a.x - b.x, dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

// ─────────────────────────────────────────────
// Construction / Destruction
// ─────────────────────────────────────────────

Game::Game()
    : target_hex(0)
    , gate_id_counter(1)
    , solved(false)
    , anim_time(0)
    , solved_pulse(0)
    , selected_gate_index(-1)
    , mouse_pos{ -100, -100 }
{
    for (int i = 0; i < 4; i++) input_bits[i] = 0;
    for (int i = 0; i < 4; i++) output_bits[i] = 0;
    Reset();
}

Game::~Game() {
}

// ─────────────────────────────────────────────
// State management
// ─────────────────────────────────────────────

void Game::Reset() {
    gates.clear();
    wires.clear();
    gate_outputs.clear();
    for (int i = 0; i < 4; i++) input_bits[i] = 0;
    for (int i = 0; i < 4; i++) output_bits[i] = 0;
    gate_id_counter = 1;
    selected_gate_index = -1;
    wire_drag_state = {};
    hovered_cell = {};
    hovered_pin = {};
    solved = false;
    solved_pulse = 0;

    target_hex = GetRandomValue(1, 15);
    if (target_hex == 0) target_hex = 10;
    Evaluate();
}

void Game::Evaluate() {
    int hex_val = EvaluateCircuit(gates, wires, input_bits, gate_outputs, output_bits);
    bool was_solved = solved;
    solved = (hex_val == target_hex);
    if (solved && !was_solved) solved_pulse = 1.0f;
}

Gate* Game::FindGateAt(int row, int col) {
    for (auto& g : gates)
        if (g.row == row && g.col == col) return &g;
    return nullptr;
}

Gate* Game::FindGateById(int id) {
    for (auto& g : gates)
        if (g.id == id) return &g;
    return nullptr;
}

void Game::RemoveWiresForGate(int gate_id) {
    wires.erase(
        std::remove_if(wires.begin(), wires.end(),
            [gate_id](const Wire& w) {
                return (w.from_type == 1 && w.from_id == gate_id) ||
                       (w.to_type == 0 && w.to_id == gate_id);
            }),
        wires.end());
}

PinHit Game::FindPinAt(Vector2 pos) {
    // Input node output pins
    for (int i = 0; i < 4; i++) {
        Vector2 p = GetInputNodeOutputPin(i);
        if (Dist(pos, p) < K_PIN_HIT_RADIUS) return { 0, i, 0, false };
    }

    // Gate pins
    for (const auto& gate : gates) {
        for (int pi = 0; pi < GetGateInputCount(gate.type); pi++) {
            Vector2 p = GetGateInputPinPos(gate, pi);
            if (Dist(pos, p) < K_PIN_HIT_RADIUS) return { 1, gate.id, pi, true };
        }
        Vector2 p = GetGateOutputPinPos(gate);
        if (Dist(pos, p) < K_PIN_HIT_RADIUS) return { 1, gate.id, 0, false };
    }

    // Output node input pins
    for (int b = 0; b < 4; b++) {
        Vector2 p = GetOutputNodeInputPin(b);
        if (Dist(pos, p) < K_PIN_HIT_RADIUS) return { 2, 0, b, true };
    }

    return {};
}

// ─────────────────────────────────────────────
// Event handlers
// ─────────────────────────────────────────────

void Game::HandleClick(Vector2 pos) {
    // Input node toggles
    for (int i = 0; i < 4; i++) {
        if (Dist(pos, { K_INPUT_X, GetInputNodeY(i) }) <= 20) {
            input_bits[i] ^= 1;
            Evaluate();
            return;
        }
    }

    PinHit clicked_pin = FindPinAt(pos);

    // Wire drag in progress
    if (wire_drag_state.IsActive()) {
        if (clicked_pin.IsValid() && clicked_pin.is_input) {
            Wire w;
            w.from_type = wire_drag_state.from_type;
            w.from_id   = wire_drag_state.from_id;
            w.from_pin  = wire_drag_state.from_pin;
            w.to_type   = (clicked_pin.source_type == 1) ? 0 : 1;
            w.to_id     = (clicked_pin.source_type == 1) ? clicked_pin.source_id : 0;
            w.to_pin    = clicked_pin.pin_index;

            bool is_self = (w.from_type == 1 && w.to_type == 0 && w.from_id == w.to_id);
            if (!is_self) {
                wires.erase(
                    std::remove_if(wires.begin(), wires.end(),
                        [&](const Wire& ex) {
                            return ex.to_type == w.to_type && ex.to_id == w.to_id && ex.to_pin == w.to_pin;
                        }),
                    wires.end());
                wires.push_back(w);
            }
            wire_drag_state = {};
            Evaluate();
            return;
        }
        if (clicked_pin.IsValid() && !clicked_pin.is_input) {
            Vector2 pin_pos;
            if (clicked_pin.source_type == 0) pin_pos = GetInputNodeOutputPin(clicked_pin.source_id);
            else if (auto* g = FindGateById(clicked_pin.source_id)) pin_pos = GetGateOutputPinPos(*g);
            else return;
            wire_drag_state = { clicked_pin.source_type, clicked_pin.source_id, clicked_pin.pin_index, pin_pos.x, pin_pos.y, true };
            return;
        }
        wire_drag_state = {};
    }

    // Start wire from output pin
    if (clicked_pin.IsValid() && !clicked_pin.is_input) {
        Vector2 pin_pos;
        if (clicked_pin.source_type == 0) pin_pos = GetInputNodeOutputPin(clicked_pin.source_id);
        else if (auto* g = FindGateById(clicked_pin.source_id)) pin_pos = GetGateOutputPinPos(*g);
        else return;
        wire_drag_state = { clicked_pin.source_type, clicked_pin.source_id, clicked_pin.pin_index, pin_pos.x, pin_pos.y, true };
        selected_gate_index = -1;
        return;
    }

    // Click input pin to remove its wire
    if (clicked_pin.IsValid() && clicked_pin.is_input) {
        int target_type = (clicked_pin.source_type == 1) ? 0 : 1;
        wires.erase(
            std::remove_if(wires.begin(), wires.end(),
                [&](const Wire& ex) {
                    return ex.to_type == target_type && ex.to_id == clicked_pin.source_id && ex.to_pin == clicked_pin.pin_index;
                }),
            wires.end());
        Evaluate();
        return;
    }

    // Palette click
    int pal_idx = PickPaletteGate(pos);
    if (pal_idx >= 0) {
        selected_gate_index = (selected_gate_index == pal_idx) ? -1 : pal_idx;
        wire_drag_state = {};
        return;
    }

    // Clear button
    if (CheckCollisionPointRec(pos, GetClearButtonRect())) {
        Reset();
        return;
    }

    // Grid placement
    HexCell cell = GetGridCell(pos);
    if (cell.IsValid() && selected_gate_index >= 0 && !FindGateAt(cell.row, cell.col)) {
        Gate ng;
        ng.id   = gate_id_counter++;
        ng.type = (GateType)selected_gate_index;
        ng.row  = cell.row;
        ng.col  = cell.col;
        gates.push_back(ng);
        gate_outputs[ng.id] = 0;
        Evaluate();
        return;
    }

    if (!clicked_pin.IsValid() && !cell.IsValid()) {
        selected_gate_index = -1;
        wire_drag_state = {};
    }
}

void Game::HandleRightClick(Vector2 pos) {
    HexCell cell = GetGridCell(pos);
    if (cell.IsValid()) {
        Gate* gate = FindGateAt(cell.row, cell.col);
        if (gate) {
            RemoveWiresForGate(gate->id);
            gate_outputs.erase(gate->id);
            gates.erase(std::remove_if(gates.begin(), gates.end(),
                [&](const Gate& g) { return g.id == gate->id; }), gates.end());
            Evaluate();
            return;
        }
    }
    selected_gate_index = -1;
    wire_drag_state = {};
}

// ─────────────────────────────────────────────
// Main loop
// ─────────────────────────────────────────────

void Game::Update() {
    mouse_pos = GetMousePosition();
    hovered_pin = FindPinAt(mouse_pos);
    hovered_cell = GetGridCell(mouse_pos);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))  HandleClick(mouse_pos);
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) HandleRightClick(mouse_pos);

    float dt = GetFrameTime();
    anim_time += dt;
    if (solved && solved_pulse > 0) solved_pulse = fmaxf(0, solved_pulse - dt * 0.8f);

    if (IsKeyPressed(KEY_ONE))   { input_bits[0] ^= 1; Evaluate(); }
    if (IsKeyPressed(KEY_TWO))   { input_bits[1] ^= 1; Evaluate(); }
    if (IsKeyPressed(KEY_THREE)) { input_bits[2] ^= 1; Evaluate(); }
    if (IsKeyPressed(KEY_FOUR))  { input_bits[3] ^= 1; Evaluate(); }
    if (IsKeyPressed(KEY_R))     { target_hex = GetRandomValue(1, 15); Evaluate(); }
    if (IsKeyPressed(KEY_T))     { target_hex = (target_hex + 1) % 16; Evaluate(); }
    if (IsKeyPressed(KEY_ESCAPE)) { selected_gate_index = -1; wire_drag_state = {}; }
}

void Game::Draw() {
    BeginDrawing();
    ClearBackground({ 13, 13, 26, 255 });

    DrawBackground();

    bool has_selection = selected_gate_index >= 0;
    bool cell_occupied = hovered_cell.IsValid() && FindGateAt(hovered_cell.row, hovered_cell.col) != nullptr;
    DrawGrid(hovered_cell, has_selection, cell_occupied);

    DrawAllWires(wires, gates, input_bits, gate_outputs, anim_time);

    if (wire_drag_state.IsActive()) {
        DrawGhostWire({ wire_drag_state.start_x, wire_drag_state.start_y }, mouse_pos);
    }

    // Draw gates + pins
    for (const auto& gate : gates) {
        Vector2 c = GetHexCenter(gate.row, gate.col);
        float gw = K_HEX_SIZE * 1.3f;
        float gh = K_HEX_SIZE * K_SQRT3 * 0.75f;
        int out_val = gate_outputs.count(gate.id) ? gate_outputs.at(gate.id) : 0;

        if (hovered_cell.row == gate.row && hovered_cell.col == gate.col) {
            DrawFilledHexagon(c, K_HEX_SIZE - 2, ColorAlpha(SKYBLUE, 0.1f));
            DrawHexOutline(c, K_HEX_SIZE - 2, 2, SKYBLUE);
        }

        DrawGateShape(gate, c.x - gw / 2, c.y - gh / 2, gw, gh, out_val);

        for (int p = 0; p < GetGateInputCount(gate.type); p++) {
            Vector2 pp = GetGateInputPinPos(gate, p);
            bool hov = hovered_pin.source_type == 1 && hovered_pin.source_id == gate.id
                    && hovered_pin.pin_index == p && hovered_pin.is_input;
            Color pc = hov ? SKYBLUE : Color{ 85, 119, 187, 255 };
            DrawCircleV(pp, K_PIN_RADIUS, pc);
            DrawCircleLines((int)pp.x, (int)pp.y, K_PIN_RADIUS, WHITE);
        }

        Vector2 op = GetGateOutputPinPos(gate);
        bool hov_out = hovered_pin.source_type == 1 && hovered_pin.source_id == gate.id && !hovered_pin.is_input;
        Color oc = hov_out ? SKYBLUE : (out_val ? Color{ 255, 96, 128, 255 } : Color{ 85, 119, 187, 255 });
        DrawCircleV(op, K_PIN_RADIUS, oc);
        DrawCircleLines((int)op.x, (int)op.y, K_PIN_RADIUS, WHITE);
    }

    // Cast PinHit to the Pin* expected by UI (same layout, just different struct)
    Pin stub_pin;
    Pin* hover_pin_ptr = nullptr;
    if (hovered_pin.IsValid()) {
        stub_pin.source_type = hovered_pin.source_type;
        stub_pin.source_id   = hovered_pin.source_id;
        stub_pin.pin_index   = hovered_pin.pin_index;
        stub_pin.is_input    = hovered_pin.is_input;
        hover_pin_ptr = &stub_pin;
    }

    DrawInputNodes(input_bits, hover_pin_ptr);

    bool has_out_wire = std::any_of(wires.begin(), wires.end(),
        [](const Wire& w) { return w.to_type == 1; });
    DrawOutputNode(output_bits, target_hex, hover_pin_ptr, has_out_wire);

    DrawPalette(selected_gate_index);

    int hex_val = output_bits[0] + output_bits[1] * 2 + output_bits[2] * 4 + output_bits[3] * 8;
    DrawInfoBar(target_hex, hex_val, solved, anim_time);

    if (wire_drag_state.IsActive()) {
        DrawCircleV(mouse_pos, 5, ColorAlpha(SKYBLUE, 0.7f));
    }

    EndDrawing();
}

bool Game::ShouldClose() const {
    return WindowShouldClose();
}
