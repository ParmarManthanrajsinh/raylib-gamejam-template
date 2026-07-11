#include "game.h"
#include "assets.h"
#include "audio.h"
#include "circuit.h"
#include "gates.h"
#include "hex_grid.h"
#include "menu.h"
#include "text_util.h" // IWYU pragma: keep
#include "ui.h"
#include "wires.h"
#include <algorithm>
#include <cmath>
#include <raylib.h>

void Game::HandleClick(Vector2 pos)
{
    robot_last_action_time = anim_time;
    robot_idle_timer = 0;
    // Input node toggles removed - inputs are now locked for difficulty.

    t_PinHit clicked_pin = FindPinAt(pos);

    // Wire drag in progress
    if (wire_drag_state.IsActive())
    {
        if (clicked_pin.IsValid() && clicked_pin.is_input)
        {
            t_Wire w{};
            w.from_type = wire_drag_state.from_type;
            w.from_id = wire_drag_state.from_id;
            w.from_pin = wire_drag_state.from_pin;
            w.to_type = (clicked_pin.source_type == 1) ? 0 : 1;
            w.to_id = (clicked_pin.source_type == 1) ? clicked_pin.source_id : 0;
            w.to_pin = clicked_pin.pin_index;

            bool is_self = (w.from_type == 1 && w.to_type == 0 && w.from_id == w.to_id);
            if (!is_self)
            {
                wires.erase
                (
                    std::remove_if(wires.begin(), wires.end(),
                    [&](const t_Wire& ex)
                    {
                        return ex.to_type == w.to_type &&
                               ex.to_id == w.to_id &&
                               ex.to_pin == w.to_pin;
                    }),
                    wires.end()
                );
                
                int from_count = 0;
                for (const auto& ex : wires) {
                    if (ex.from_type == w.from_type && ex.from_id == w.from_id && ex.from_pin == w.from_pin) {
                        from_count++;
                    }
                }

                if (from_count < 2)
                {
                    wires.emplace_back(w);
                    robot.OnWireConnected(wires.size(), gates.size());
                    if (!robot_first_wire_connected) {
                        robot_first_wire_connected = true;
                        robot.OnFirstWireConnected();
                    }
                    SpawnParticles(pos, {255, 255, 0, 255}, 15);
                    PlaySfx(SfxType::CONNECT_WIRE);
                }
                else
                {
                    SpawnParticles(pos, {255, 100, 100, 255}, 15);
                    PlaySfx(SfxType::DISCONNECT_WIRE);
                }
            }
            wire_drag_state = {};
            Evaluate();
            return;
        }
        if (clicked_pin.IsValid() && !clicked_pin.is_input)
        {
            Vector2 pin_pos{};
            if (clicked_pin.source_type == 0)
            {
                pin_pos = GetInputNodeOutputPin(clicked_pin.source_id);
            }
            else if (auto* g = FindGateById(clicked_pin.source_id))
            {
                pin_pos = GetGateOutputPinPos(*g);
            }
            else
            {
                return;
            }

            wire_drag_state =
            {
                clicked_pin.source_type,
                clicked_pin.source_id,
                clicked_pin.pin_index,
                pin_pos.x,
                pin_pos.y,
                true
            };
            return;
        }
        robot.OnWireDragCancelled();
        wire_drag_state = {};
    }

    // Start wire from output pin
    if (clicked_pin.IsValid() && !clicked_pin.is_input)
    {
        Vector2 pin_pos{};
        if (clicked_pin.source_type == 0)
            pin_pos = GetInputNodeOutputPin(clicked_pin.source_id);
        else if (auto* g = FindGateById(clicked_pin.source_id))
            pin_pos = GetGateOutputPinPos(*g);
        else
            return;

        wire_drag_state =
        {
            clicked_pin.source_type,
            clicked_pin.source_id,
            clicked_pin.pin_index,
            pin_pos.x,
            pin_pos.y,
            true
        };
        selected_gate_index = -1;
        return;
    }

    // Click input pin to remove its wire
    if (clicked_pin.IsValid() && clicked_pin.is_input)
    {
        int target_type = (clicked_pin.source_type == 1) ? 0 : 1;
        int old_count = static_cast<int>(wires.size());
        wires.erase
        (
            std::remove_if
            (
                wires.begin(), wires.end(),
                [&](const t_Wire& ex)
                {
                    return ex.to_type == target_type &&
                           ex.to_id == clicked_pin.source_id &&
                           ex.to_pin == clicked_pin.pin_index;
                }
            ),
            wires.end()
        );
        if (static_cast<int>(wires.size()) < old_count)
        {
            robot.OnWireDeleted();
            SpawnParticles(pos, {255, 100, 100, 255}, 15);
            screen_shake_time = 0.15f;
            PlaySfx(SfxType::DISCONNECT_WIRE);
        }
        Evaluate();
        return;
    }

    // Palette click
    int pal_idx = PickPaletteGate(pos);
    if (pal_idx >= 0)
    {
        selected_gate_index = (selected_gate_index == pal_idx) ? -1 : pal_idx;
        if (wire_drag_state.IsActive()) robot.OnWireDragCancelled();
        wire_drag_state = {};
        return;
    }

    // Menu button
    if (CheckMenuButtonClick(pos))
    {
        game_state = GameState::PLAYING_TO_TITLE_TRANSITION;
        transition_time = 0;
        PlaySfx(SfxType::SOLVED);
        return;
    }

    // Clear button
    if (CheckCollisionPointRec(pos, GetClearButtonRect()))
    {
        for (const auto& g : gates)
        {
            Vector2 c = GetHexCenter(g.row, g.col);
            SpawnParticles(c, {255, 80, 80, 255}, 20);
        }
        robot.OnClearPressed();
        PlaySfx(SfxType::REMOVE_GATE);
        screen_shake_time = 0.3f;
        Reset(true);
        return;
    }

    // Grid placement
    t_HexCell cell = GetGridCell(pos);
    bool is_obstacle = false;
    for (const auto& o : obstacles) {
        if (o.row == cell.row && o.col == cell.col) { 
            is_obstacle = true; 
            if (selected_gate_index >= 0) {
                robot.OnObstacleAttempt(); 
                robot_obstacle_attempts++;
            }
            break; 
        }
    }

    if (cell.IsValid() && selected_gate_index >= 0 && !FindGateAt(cell.row, cell.col) && !is_obstacle)
    {
        t_Gate ng{};
        ng.id = gate_id_counter++;
        ng.type = static_cast<GateType>(selected_gate_index);
        ng.row = cell.row;
        ng.col = cell.col;
        ng.spawn_time = anim_time;
        gates.emplace_back(ng);
        
        robot.OnGatePlaced(ng.type, gates.size());
        robot_gate_type_counts[static_cast<int>(ng.type)]++;
        if (!robot_first_gate_placed) {
            robot_first_gate_placed = true;
            robot.OnFirstGatePlaced(ng.type);
        }
        
        gate_outputs[ng.id] = 0;
        SpawnParticles(GetHexCenter(cell.row, cell.col), {0, 255, 255, 255}, 20);
        screen_shake_time = 0.1f;
        PlaySfx(SfxType::PLACE_GATE);
        Evaluate();
        return;
    }

    if (!clicked_pin.IsValid() && cell.IsValid())
    {
        if (selected_gate_index == -1)
        {
            t_Gate* gate = FindGateAt(cell.row, cell.col);
            if (gate)
            {
                dragging_gate_id = gate->id;
                PlaySfx(SfxType::DISCONNECT_WIRE); // subtle click sound
            }
        }
        if (wire_drag_state.IsActive()) robot.OnWireDragCancelled();
        wire_drag_state = {};
    }
}

void Game::HandleRightClick(Vector2 pos)
{
    robot_last_action_time = anim_time;
    robot_idle_timer = 0;
    t_HexCell cell = GetGridCell(pos);
    if (cell.IsValid())
    {
        t_Gate* gate = FindGateAt(cell.row, cell.col);
        if (gate)
        {
            Vector2 c = GetHexCenter(gate->row, gate->col);
            RemoveWiresForGate(gate->id);
            gate_outputs.erase(gate->id);
            gates.erase
            (
                std::remove_if
                (
                    gates.begin(), gates.end(),
                    [&](const t_Gate& g)
                    {
                        return g.id == gate->id;
                    }
                ),
                gates.end()
            );
            robot.OnGateDeleted(gate->type, gates.size());
            robot_delete_count++;
            SpawnParticles(c, {255, 80, 80, 255}, 30);
            screen_shake_time = 0.2f;
            PlaySfx(SfxType::REMOVE_GATE);
            Evaluate();
            return;
        }
    }
    selected_gate_index = -1;
    if (wire_drag_state.IsActive()) robot.OnWireDragCancelled();
    wire_drag_state = {};
}

