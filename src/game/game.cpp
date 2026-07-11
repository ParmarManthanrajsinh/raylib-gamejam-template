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

namespace
{
    float Dist(Vector2 a, Vector2 b)
    {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        return sqrtf(dx * dx + dy * dy);
    }
}

Game::Game()
: game_state(GameState::TITLE_SCREEN),
  target_hex(0),
  gate_id_counter(1),
  solved(false),
  anim_time(0),
  solved_pulse(0),
  transition_time(0),
  selected_gate_index(-1),
  mouse_pos{-100, -100},
  ghost_pos{-100, -100},
  screen_shake_time(0),
  level_timer(0),
  robot_idle_timer(0),
  robot_delete_count(0),
  robot_last_action_time(0),
  robot_last_mouse_pos{-100, -100},
  robot_mouse_still_time(0),
  robot_first_gate_placed(false),
  robot_first_wire_connected(false),
  robot_obstacle_attempts(0),
  robot_matching_bits_prev(0)
{
    render_target = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    for (int i = 0; i < 4; i++) input_bits[i] = 0;
    for (int i = 0; i < 4; i++) output_bits[i] = 0;
    for (int i = 0; i < 7; i++) robot_gate_type_counts[i] = 0;
    Reset();
}
Game::~Game()
{
    UnloadRenderTexture(render_target);
}

void Game::Reset(bool is_clear)
{
    gates.clear();
    wires.clear();
    gate_outputs.clear();
    obstacles.clear();

    for (int i = 0; i < 4; i++) input_bits[i] = GetRandomValue(0, 1);
    for (int i = 0; i < 4; i++) output_bits[i] = 0;

    // Generate obstacles
    int num_obstacles = GetRandomValue(3, 6);
    for (int i = 0; i < num_obstacles; i++)
    {
        t_HexCell obs{};
        obs.row = GetRandomValue(2, 6);
        obs.col = GetRandomValue(2, 6);
        bool exists = false;
        for (const auto& o : obstacles) {
            if (o.row == obs.row && o.col == obs.col) { exists = true; break; }
        }
        if (!exists) obstacles.push_back(obs);
    }

    gate_id_counter = 1;
    selected_gate_index = -1;
    dragging_gate_id = -1;
    wire_drag_state = {};
    hovered_cell = {};
    hovered_pin = {};
    solved = false;
    solved_pulse = 0;
    transition_time = 0;
    anim_time = 0;
    level_timer = 0;
    level_complete_delay = 0;
    screen_shake_time = 0;
    
    // Ensure the target is actually reachable? A random 1-15 is fine.
    if (!is_clear) {
        target_hex = GetRandomValue(1, 15);
        robot.OnLevelStart(target_hex);
    }
    Evaluate();
}

void Game::Evaluate()
{
    int hex_val = EvaluateCircuit(gates, wires, input_bits, gate_outputs, output_bits);
    bool was_solved = solved;
    solved = (hex_val == target_hex);
    if (solved && !was_solved)
    {
        solved_pulse = 1.0f;
        screen_shake_time = 0.4f;
        SpawnParticles({OUTPUT_CENTER_X, OUTPUT_CENTER_Y}, {0, 255, 136, 255}, 50);
        PlaySfx(SfxType::SOLVED);
        
        robot.OnSolved(gates.size(), wires.size(), level_timer);
        
        level_complete_delay = 1.5f;
        last_stats.gates_used = static_cast<int>(gates.size());
        last_stats.wires_used = static_cast<int>(wires.size());
        last_stats.time_taken = level_timer;
        last_stats.efficiency_score = 10000 - static_cast<int>(level_timer * 10) - (last_stats.gates_used * 50) - (last_stats.wires_used * 25);
        if (last_stats.efficiency_score < 0) last_stats.efficiency_score = 0;
    }
}

void Game::SpawnParticles(Vector2 pos, Color color, int count)
{
    for (int i = 0; i < count; i++)
    {
        Particle p;
        p.pos = pos;
        float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f * PI;
        float speed = 20.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f;
        p.vel = {cosf(angle) * speed, sinf(angle) * speed};
        p.max_life = 0.3f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.4f;
        p.life = p.max_life;
        p.color = color;
        particles.push_back(p);
    }
}

t_Gate* Game::FindGateAt(int row, int col)
{
    for (auto& g : gates)
    {
        if (g.row == row && g.col == col) 
        {
            return &g;
        }
    }
    return nullptr;
}

t_Gate* Game::FindGateById(int id)
{
    for (auto& g : gates)
    {
        if (g.id == id) return &g;
    }
    return nullptr;
}

void Game::RemoveWiresForGate(int gate_id)
{
    wires.erase
    (
        std::remove_if
        (
            wires.begin(), wires.end(),
            [gate_id](const t_Wire& w)
            {
                return (w.from_type == 1 && w.from_id == gate_id) ||
                       (w.to_type == 0 && w.to_id == gate_id);
            }
        ),
        wires.end()
    );
}

t_PinHit Game::FindPinAt(Vector2 pos)
{
    // Input node output pins
    for (int i = 0; i < 4; i++)
    {
        Vector2 p = GetInputNodeOutputPin(i);
        if (Dist(pos, p) < PIN_HIT_RADIUS) return {0, i, 0, false};
    }

    // Gate pins
    for (const auto& gate : gates)
    {
        for (int pi = 0; pi < GetGateInputCount(gate.type); pi++)
        {
            Vector2 p = GetGateInputPinPos(gate, pi);
            if (Dist(pos, p) < PIN_HIT_RADIUS)return {1, gate.id, pi, true};
        }
        Vector2 p = GetGateOutputPinPos(gate);
        if (Dist(pos, p) < PIN_HIT_RADIUS) return {1, gate.id, 0, false};
    }

    // Output node input pins
    for (int b = 0; b < 4; b++)
    {
        Vector2 p = GetOutputNodeInputPin(b);
        if (Dist(pos, p) < PIN_HIT_RADIUS) return {2, 0, b, true};
    }

    return {};
}

// Event handlers






void Game::Draw()
{
    bool is_level_complete_mode = (game_state == GameState::LEVEL_COMPLETE || 
                                   game_state == GameState::PLAYING_TO_LEVEL_COMPLETE_TRANSITION || 
                                   game_state == GameState::LEVEL_COMPLETE_TO_PLAY_TRANSITION ||
                                   game_state == GameState::LEVEL_COMPLETE_TO_TITLE_TRANSITION);

    if (game_state != GameState::PLAYING && !is_level_complete_mode)
    {
        BeginDrawing();
        if (game_state == GameState::TITLE_SCREEN || game_state == GameState::TITLE_TO_PLAY_TRANSITION || game_state == GameState::TITLE_TO_HOW_TO_PLAY_TRANSITION)
        {
            DrawTitleScreen(anim_time, transition_time);
        }
        else if (game_state == GameState::PLAYING_TO_TITLE_TRANSITION)
        {
            DrawTitleScreen(anim_time, transition_time);
        }
        else if (game_state == GameState::HOW_TO_PLAY || game_state == GameState::HOW_TO_PLAY_TO_PLAY_TRANSITION || game_state == GameState::HOW_TO_PLAY_TO_TITLE_TRANSITION)
        {
            DrawHowToPlay(anim_time, transition_time);
        }
        EndDrawing();
        return;
    }

    BeginTextureMode(render_target);
    ClearBackground({13, 13, 26, 255});

    Camera2D camera = {};
    camera.zoom = 1.0f;
    if (screen_shake_time > 0)
    {
        float shake_mag = screen_shake_time * 15.0f;
        camera.offset.x = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 - 1) * shake_mag;
        camera.offset.y = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 - 1) * shake_mag;
    }
    BeginMode2D(camera);

    DrawBackground();

    bool has_selection = selected_gate_index >= 0;
    bool cell_occupied =
        hovered_cell.IsValid() &&
        FindGateAt(hovered_cell.row, hovered_cell.col) != nullptr;
    DrawGrid(hovered_cell, has_selection, cell_occupied, anim_time);

    // Draw obstacles
    for (const auto& obs : obstacles)
    {
        Vector2 c = GetHexCenter(obs.row, obs.col);
        DrawPoly(c, 6, HEX_SIZE * 0.95f, 90.0f, ColorAlpha({20, 0, 0, 255}, 0.8f));
        DrawPolyLinesEx(c, 6, HEX_SIZE * 0.95f, 90.0f, 2.0f, ColorAlpha(RED, 0.5f));
        // Draw cross/stripes to make it look like an obstacle
        DrawLineEx({c.x - HEX_SIZE*0.5f, c.y - HEX_SIZE*0.5f}, {c.x + HEX_SIZE*0.5f, c.y + HEX_SIZE*0.5f}, 2.0f, ColorAlpha(RED, 0.3f));
        DrawLineEx({c.x - HEX_SIZE*0.5f, c.y + HEX_SIZE*0.5f}, {c.x + HEX_SIZE*0.5f, c.y - HEX_SIZE*0.5f}, 2.0f, ColorAlpha(RED, 0.3f));
    }

    Vector2 drag_pos = {-1000, -1000};
    if (dragging_gate_id != -1)
    {
        drag_pos = mouse_pos;
        if (hovered_cell.IsValid() && (!FindGateAt(hovered_cell.row, hovered_cell.col) || FindGateAt(hovered_cell.row, hovered_cell.col)->id == dragging_gate_id))
        {
            Vector2 c = GetHexCenter(hovered_cell.row, hovered_cell.col);
            drag_pos.x = c.x * 0.5f + mouse_pos.x * 0.5f;
            drag_pos.y = c.y * 0.5f + mouse_pos.y * 0.5f;
        }
    }

    DrawAllWires(wires, gates, input_bits, gate_outputs, anim_time, dragging_gate_id, drag_pos);

    if (wire_drag_state.IsActive())
    {
        DrawGhostWire
        (
            {wire_drag_state.start_x, wire_drag_state.start_y},
            mouse_pos
        );
    }

    // Draw gates + pins
    for (const auto& gate : gates)
    {
        if (gate.id == dragging_gate_id) continue; // Draw dragged gate later

        Vector2 c = GetHexCenter(gate.row, gate.col);
        float scale = 1.0f;
        float age = anim_time - gate.spawn_time;
        if (age < 0.2f)
        {
            scale = 1.0f + 0.5f * (1.0f - age / 0.2f);
        }
        float gw = HEX_SIZE * 1.3f * scale;
        float gh = HEX_SIZE * SQRT3 * 0.75f * scale;
        int out_val = gate_outputs.count(gate.id) ? gate_outputs.at(gate.id) : 0;

        // Occupied cell base highlight
        DrawFilledHexagon(c, HEX_SIZE - 1, ColorAlpha({0, 200, 255, 255}, 0.05f));
        DrawHexOutline(c, HEX_SIZE - 1, 1.5f, ColorAlpha({0, 200, 255, 255}, 0.3f));

        if (hovered_cell.row == gate.row && hovered_cell.col == gate.col)
        {
            DrawFilledHexagon(c, HEX_SIZE - 2, ColorAlpha({0, 255, 255, 255}, 0.15f));
            DrawHexOutline(c, HEX_SIZE - 2, 2.5f, {0, 255, 255, 255});
        }

        DrawGateShape(gate, c.x - gw / 2, c.y - gh / 2, gw, gh, out_val);

        for (int p = 0; p < GetGateInputCount(gate.type); p++)
        {
            Vector2 pp = GetGateInputPinPos(gate, p);
            bool hov = hovered_pin.source_type == 1 &&
                       hovered_pin.source_id == gate.id &&
                       hovered_pin.pin_index == p && hovered_pin.is_input;

            Color pc = hov ? SKYBLUE : Color{85, 119, 187, 255};
            DrawCircleV(pp, PIN_RADIUS, pc);
            DrawCircleLines(static_cast<int>(pp.x), static_cast<int>(pp.y), PIN_RADIUS, WHITE);
        }

        Vector2 op = GetGateOutputPinPos(gate);
        bool hov_out = hovered_pin.source_type == 1 &&
                       hovered_pin.source_id == gate.id &&
                       !hovered_pin.is_input;

        Color oc = hov_out ? SKYBLUE
                           : (out_val ? Color{255, 96, 128, 255}
                                      : Color{85, 119, 187, 255});

        DrawCircleV(op, PIN_RADIUS, oc);
        DrawCircleLines(static_cast<int>(op.x), static_cast<int>(op.y), PIN_RADIUS, WHITE);
    }

    // Ghost gate attached to cursor
    if (selected_gate_index >= 0)
    {
        GateType ft = static_cast<GateType>(selected_gate_index);
        t_Gate fg = {0, ft, 0, 0};
        float gw = HEX_SIZE * 1.3f;
        float gh = HEX_SIZE * SQRT3 * 0.75f;
        
        Vector2 gp = mouse_pos;
        if (hovered_cell.IsValid() && !cell_occupied)
        {
            Vector2 c = GetHexCenter(hovered_cell.row, hovered_cell.col);
            
            // Draw highlight under cell
            float pulse = 0.6f + 0.4f * sinf(anim_time * 4);
            DrawFilledHexagon(c, HEX_SIZE + 4, ColorAlpha(SKYBLUE, 0.06f * pulse));
            DrawFilledHexagon(c, HEX_SIZE - 2, ColorAlpha(SKYBLUE, 0.2f * pulse));
            DrawHexOutline(c, HEX_SIZE - 1, 3.0f, ColorAlpha(SKYBLUE, 0.8f * pulse));

            // Magnetic snap
            gp.x = c.x * 0.5f + mouse_pos.x * 0.5f;
            gp.y = c.y * 0.5f + mouse_pos.y * 0.5f;
        }

        DrawGateShape(fg, gp.x - gw / 2, gp.y - gh / 2, gw, gh, 0, 0.8f);
    }

    // Draw dragged gate at mouse
    if (dragging_gate_id != -1)
    {
        t_Gate* g = FindGateById(dragging_gate_id);
        if (g)
        {
            float gw = HEX_SIZE * 1.3f;
            float gh = HEX_SIZE * SQRT3 * 0.75f;
            
            if (hovered_cell.IsValid() && (!FindGateAt(hovered_cell.row, hovered_cell.col) || FindGateAt(hovered_cell.row, hovered_cell.col)->id == dragging_gate_id))
            {
                Vector2 c = GetHexCenter(hovered_cell.row, hovered_cell.col);
                float pulse = 0.6f + 0.4f * sinf(anim_time * 4);
                DrawFilledHexagon(c, HEX_SIZE + 4, ColorAlpha(SKYBLUE, 0.06f * pulse));
                DrawFilledHexagon(c, HEX_SIZE - 2, ColorAlpha(SKYBLUE, 0.2f * pulse));
                DrawHexOutline(c, HEX_SIZE - 1, 3.0f, ColorAlpha(SKYBLUE, 0.8f * pulse));
            }

            int out_val = gate_outputs.count(g->id) ? gate_outputs.at(g->id) : 0;
            DrawGateShape(*g, drag_pos.x - gw / 2, drag_pos.y - gh / 2, gw, gh, out_val);
        }
    }

    t_Pin stub_pin;
    t_Pin* hover_pin_ptr = nullptr;
    if (hovered_pin.IsValid())
    {
        stub_pin.source_type = hovered_pin.source_type;
        stub_pin.source_id = hovered_pin.source_id;
        stub_pin.pin_index = hovered_pin.pin_index;
        stub_pin.is_input = hovered_pin.is_input;
        hover_pin_ptr = &stub_pin;
    }

    DrawInputNodes(input_bits, hover_pin_ptr, anim_time);

    bool has_out_wire =
        std::any_of
        (
            wires.begin(), wires.end(),
            [](const t_Wire& w)
            {
                return w.to_type == 1;
            }
        );
    DrawOutputNode(output_bits, target_hex, hover_pin_ptr, has_out_wire, anim_time);
    DrawPalette(selected_gate_index);
    DrawMenuButton(anim_time);

    int hex_val = output_bits[0] + output_bits[1] * 2 + output_bits[2] * 4 +
        output_bits[3] * 8;
    DrawInfoBar(target_hex, hex_val, solved, anim_time);

    if (wire_drag_state.IsActive())
    {
        DrawCircleV(mouse_pos, 5, ColorAlpha(SKYBLUE, 0.7f));
    }

    for (const auto& p : particles)
    {
        Color c = p.color;
        c.a = static_cast<unsigned char>(255 * (p.life / p.max_life));
        DrawCircleV(p.pos, 3.0f * (p.life / p.max_life), c);
    }

    if (is_level_complete_mode)
    {
        float bg_alpha = 0.85f;
        if (game_state == GameState::PLAYING_TO_LEVEL_COMPLETE_TRANSITION)
        {
            bg_alpha = 0.85f * (transition_time / 0.8f);
        }
        else if (game_state == GameState::LEVEL_COMPLETE_TO_PLAY_TRANSITION || game_state == GameState::LEVEL_COMPLETE_TO_TITLE_TRANSITION)
        {
            // Keep bg_alpha at 0.85f while panel shrinks, then we will use a black circle wipe in DrawLevelComplete
            bg_alpha = 0.85f;
        }

        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ColorAlpha(BLACK, bg_alpha));
        DrawLevelComplete(anim_time, transition_time, last_stats, game_state);
    }

    EndMode2D();

    if (game_state == GameState::PLAYING || is_level_complete_mode) {
        robot.Draw(anim_time, mouse_pos);
    }
    EndTextureMode();

    BeginDrawing();
    ClearBackground(BLACK);
    BeginShaderMode(GetBloomShader());
    DrawTextureRec
    (
        render_target.texture, 
        { 0, 0, static_cast<float>(render_target.texture.width), static_cast<float>(-render_target.texture.height) }, 
        { 0, 0 }, 
        WHITE
    );
    EndShaderMode();
    EndDrawing();
}

bool Game::ShouldClose() const
{
    return WindowShouldClose();
}
