#include "game.h"
#include "assets.h" // IWYU pragma: keep
#include "audio.h"
#include "circuit.h" // IWYU pragma: keep
#include "gates.h"
#include "hex_grid.h"
#include "menu.h"
#include "text_util.h" // IWYU pragma: keep
#include "ui.h"
#include "wires.h"
#include <algorithm>
#include <cmath>
#include <raylib.h>

void Game::Update()
{
    if (game_state != GameState::PLAYING)
    {
        if (game_state == GameState::TITLE_SCREEN)
        {
            GameState next = UpdateTitleScreen(anim_time);
            if (next == GameState::TITLE_TO_PLAY_TRANSITION)
            {
                game_state = GameState::TITLE_TO_PLAY_TRANSITION;
                PlaySfx(SfxType::SOLVED); // Juice for starting
            }
            else if (next == GameState::TITLE_TO_HOW_TO_PLAY_TRANSITION)
            {
                game_state = GameState::TITLE_TO_HOW_TO_PLAY_TRANSITION;
                PlaySfx(SfxType::SOLVED);
            }
        }
        else if (game_state == GameState::TITLE_TO_PLAY_TRANSITION || game_state == GameState::HOW_TO_PLAY_TO_PLAY_TRANSITION)
        {
            float dt = GetFrameTime();
            transition_time += dt;
            if (transition_time >= 0.8f)
            {
                game_state = GameState::PLAYING;
                transition_time = 0;
                Reset(); // Initialize fresh board
            }
        }
        else if (game_state == GameState::TITLE_TO_HOW_TO_PLAY_TRANSITION)
        {
            float dt = GetFrameTime();
            transition_time += dt;
            if (transition_time >= 0.8f)
            {
                game_state = GameState::HOW_TO_PLAY;
                transition_time = 0;
            }
        }
        else if (game_state == GameState::HOW_TO_PLAY_TO_TITLE_TRANSITION)
        {
            float dt = GetFrameTime();
            transition_time += dt;
            if (transition_time >= 0.8f)
            {
                game_state = GameState::TITLE_SCREEN;
                transition_time = 0;
            }
        }
        else if (game_state == GameState::PLAYING_TO_TITLE_TRANSITION)
        {
            float dt = GetFrameTime();
            transition_time += dt;
            if (transition_time >= 0.8f)
            {
                game_state = GameState::TITLE_SCREEN;
                transition_time = 0;
            }
        }
        else if (game_state == GameState::HOW_TO_PLAY)
        {
            GameState next = UpdateHowToPlay(anim_time);
            if (next == GameState::HOW_TO_PLAY_TO_TITLE_TRANSITION)
            {
                game_state = GameState::HOW_TO_PLAY_TO_TITLE_TRANSITION;
                PlaySfx(SfxType::SOLVED);
            }
            else if (next == GameState::HOW_TO_PLAY_TO_PLAY_TRANSITION)
            {
                game_state = GameState::HOW_TO_PLAY_TO_PLAY_TRANSITION;
                PlaySfx(SfxType::SOLVED); // Juice for starting
            }
        }

        else if (game_state == GameState::LEVEL_COMPLETE)
        {
            GameState next = UpdateLevelComplete(anim_time, last_stats);
            if (next == GameState::LEVEL_COMPLETE_TO_PLAY_TRANSITION)
            {
                game_state = GameState::LEVEL_COMPLETE_TO_PLAY_TRANSITION;
                PlaySfx(SfxType::SOLVED);
            }
            else if (next == GameState::LEVEL_COMPLETE_TO_TITLE_TRANSITION)
            {
                game_state = GameState::LEVEL_COMPLETE_TO_TITLE_TRANSITION;
                PlaySfx(SfxType::SOLVED);
            }
        }
        else if (game_state == GameState::PLAYING_TO_LEVEL_COMPLETE_TRANSITION)
        {
            float dt = GetFrameTime();
            transition_time += dt;
            if (transition_time >= 0.8f)
            {
                game_state = GameState::LEVEL_COMPLETE;
                transition_time = 0;
            }
        }
        else if (game_state == GameState::LEVEL_COMPLETE_TO_PLAY_TRANSITION)
        {
            float dt = GetFrameTime();
            transition_time += dt;
            if (transition_time >= 0.8f)
            {
                game_state = GameState::PLAYING;
                transition_time = 0;
                Reset(); // fresh board
            }
        }
        else if (game_state == GameState::LEVEL_COMPLETE_TO_TITLE_TRANSITION)
        {
            float dt = GetFrameTime();
            transition_time += dt;
            if (transition_time >= 0.8f)
            {
                game_state = GameState::TITLE_SCREEN;
                transition_time = 0;
            }
        }

        float dt = GetFrameTime();
        anim_time += dt;
        if (screen_shake_time > 0) screen_shake_time -= dt;
        return;
    }

    if (!solved)
    {
        level_timer += GetFrameTime();
    }
    else
    {
        if (level_complete_delay > 0)
        {
            level_complete_delay -= GetFrameTime();
            if (level_complete_delay <= 0)
            {
                game_state = GameState::PLAYING_TO_LEVEL_COMPLETE_TRANSITION;
                transition_time = 0;
                PlaySfx(SfxType::SOLVED); // swoosh sound for menu transition
                return;
            }
        }
    }

    mouse_pos = GetMousePosition();
    hovered_pin = FindPinAt(mouse_pos);
    hovered_cell = GetGridCell(mouse_pos);

    if (hovered_pin.IsValid()) {
        robot.SetHoveredPin(&hovered_pin);
    } else {
        robot.SetHoveredPin(nullptr);
    }

    int hovered_palette = PickPaletteGate(mouse_pos);
    if (hovered_palette >= 0) {
        robot.OnPaletteHover(static_cast<GateType>(hovered_palette));
    }

    float mouse_dx = mouse_pos.x - robot_last_mouse_pos.x;
    float mouse_dy = mouse_pos.y - robot_last_mouse_pos.y;
    float mouse_speed = sqrtf(mouse_dx*mouse_dx + mouse_dy*mouse_dy) / GetFrameTime();
    robot_last_mouse_pos = mouse_pos;
    if (mouse_speed < 5.0f) robot_mouse_still_time += GetFrameTime();
    else robot_mouse_still_time = 0;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        HandleClick(mouse_pos);
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        HandleRightClick(mouse_pos);

    if (dragging_gate_id != -1)
    {
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            if (hovered_cell.IsValid())
            {
                bool is_obstacle = false;
                for (const auto& o : obstacles)
                {
                    if (o.row == hovered_cell.row && o.col == hovered_cell.col) { is_obstacle = true; break; }
                }

                t_Gate* existing = FindGateAt(hovered_cell.row, hovered_cell.col);
                if (!is_obstacle && (!existing || existing->id == dragging_gate_id))
                {
                    t_Gate* g = FindGateById(dragging_gate_id);
                    if (g)
                    {
                        g->row = hovered_cell.row;
                        g->col = hovered_cell.col;
                        SpawnParticles(GetHexCenter(hovered_cell.row, hovered_cell.col), {0, 255, 255, 255}, 15);
                        PlaySfx(SfxType::PLACE_GATE);
                    }
                }
            }
            dragging_gate_id = -1;
            Evaluate();
        }
    }

    if (wire_drag_state.IsActive())
    {
        if (GetRandomValue(0, 2) == 0) SpawnParticles(mouse_pos, {0, 245, 212, 200}, 1);
    }

    float dt = GetFrameTime();
    anim_time += dt;
    robot_idle_timer += dt;
    if (solved && solved_pulse > 0)
        solved_pulse = fmaxf(0, solved_pulse - dt * 0.8f);

    if (screen_shake_time > 0) screen_shake_time -= dt;

    for (auto& p : particles)
    {
        p.pos.x += p.vel.x * dt;
        p.pos.y += p.vel.y * dt;
        p.life -= dt;
    }
    particles.erase
    (
        std::remove_if
        (
            particles.begin(), particles.end(),
            [](const t_Particle& p)
            {
                return p.life <= 0;
            }
        ),
        particles.end()
    );

    ghost_pos.x += (mouse_pos.x - ghost_pos.x) * dt * 15.0f;
    ghost_pos.y += (mouse_pos.y - ghost_pos.y) * dt * 15.0f;

    // Keyboard input toggles removed for fixed difficulty
    if (IsKeyPressed(KEY_R))
    {
        target_hex = GetRandomValue(1, 15);
        robot.OnCheatDetected("reroll");
        Evaluate();
    }
    if (IsKeyPressed(KEY_T))
    {
        target_hex = (target_hex + 1) % 16;
        robot.OnCheatDetected("increment");
        Evaluate();
    }
    if (IsKeyPressed(KEY_M))
    {
        selected_gate_index = -1;
        wire_drag_state = {};
        robot.OnSessionEnd(gates.size(), wires.size());
        game_state = GameState::PLAYING_TO_TITLE_TRANSITION;
        transition_time = 0;
        PlaySfx(SfxType::SOLVED);
    }

    if (wire_drag_state.IsActive() || dragging_gate_id != -1)
    {
        robot_last_action_time = anim_time;
    }

    robot.Update
    (
        anim_time, mouse_pos, level_timer,
        output_bits, target_hex, solved,
        robot_mouse_still_time, robot_last_action_time,
        robot_idle_timer, robot_gate_type_counts,
        robot_delete_count, wires.size(),
        robot_obstacle_attempts, robot_matching_bits_prev
    );

    // Phase 3.4 Mood Particles
    if (GetRandomValue(0, 100) < 5)
    {
        RobotMood mood = robot.GetMood();
        Vector2 p = robot.GetPos();
        if (mood == RobotMood::HAPPY || mood == RobotMood::EXCITED) SpawnParticles({p.x, p.y - 40}, {255, 220, 50, 255}, 1);
        else if (mood == RobotMood::ANGRY) SpawnParticles({p.x, p.y - 20}, {255, 50, 50, 255}, 1);
        else if (mood == RobotMood::SURPRISED) SpawnParticles({p.x, p.y - 50}, {100, 200, 255, 255}, 1);
    }
}
