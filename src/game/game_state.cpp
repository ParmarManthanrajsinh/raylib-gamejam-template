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
    if (game_state != GameState::PLAYING && game_state != GameState::TUTORIAL)
    {
        if (game_state == GameState::TITLE_SCREEN)
        {
            robot.SetScreen(RobotScreen::TITLE);
            GameState next = UpdateTitleScreen(anim_time);
            if (next == GameState::TITLE_TO_PLAY_TRANSITION)
            {
                robot.SetScreen(RobotScreen::PLAYING);
                if (tutorial_required)
                {
                    game_state = GameState::TITLE_TO_TUTORIAL_TRANSITION;
                }
                else
                {
                    game_state = GameState::TITLE_TO_PLAY_TRANSITION;
                }
                PlaySfx(SfxType::SOLVED); // Juice for starting
            }
            else if (next == GameState::TITLE_TO_HOW_TO_PLAY_TRANSITION)
            {
                robot.SetScreen(RobotScreen::HOW_TO_PLAY);
                game_state = GameState::TITLE_TO_HOW_TO_PLAY_TRANSITION;
                PlaySfx(SfxType::SOLVED);
            }
        }
        else if 
        (
            game_state == GameState::TITLE_TO_PLAY_TRANSITION ||
            game_state == GameState::HOW_TO_PLAY_TO_PLAY_TRANSITION
        )
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
        else if (game_state == GameState::TITLE_TO_TUTORIAL_TRANSITION)
        {
            float dt = GetFrameTime();
            transition_time += dt;
            if (transition_time >= 0.8f)
            {
                game_state = GameState::TUTORIAL;
                transition_time = 0;
                SetupTutorial();
                robot.SetScreen(RobotScreen::PLAYING);
                robot.OnTutorialStep(tutorial.current_step);
            }
        }
        else if (game_state == GameState::TUTORIAL_TO_PLAY_TRANSITION)
        {
            float dt = GetFrameTime();
            transition_time += dt;
            if (transition_time >= 0.8f)
            {
                game_state = GameState::PLAYING;
                transition_time = 0;
                Reset(); // Initialize fresh board for normal play
                
                save_data.tutorial_complete = true;
                tutorial_required = false;
                SaveSaveData(save_data);
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
            robot.SetScreen(RobotScreen::HOW_TO_PLAY);
            GameState next = UpdateHowToPlay(anim_time);
            if (next == GameState::HOW_TO_PLAY_TO_TITLE_TRANSITION)
            {
                robot.SetScreen(RobotScreen::TITLE);
                game_state = GameState::HOW_TO_PLAY_TO_TITLE_TRANSITION;
                PlaySfx(SfxType::SOLVED);
            }
            else if (next == GameState::HOW_TO_PLAY_TO_PLAY_TRANSITION)
            {
                robot.SetScreen(RobotScreen::PLAYING);
                game_state = GameState::HOW_TO_PLAY_TO_PLAY_TRANSITION;
                PlaySfx(SfxType::SOLVED); // Juice for starting
            }
        }

        else if (game_state == GameState::LEVEL_COMPLETE)
        {
            robot.SetScreen(RobotScreen::LEVEL_COMPLETE);
            GameState next = UpdateLevelComplete(anim_time, last_stats);
            if (next == GameState::LEVEL_COMPLETE_TO_PLAY_TRANSITION)
            {
                robot.SetScreen(RobotScreen::PLAYING);
                game_state = GameState::LEVEL_COMPLETE_TO_PLAY_TRANSITION;
                PlaySfx(SfxType::SOLVED);
            }
            else if (next == GameState::LEVEL_COMPLETE_TO_TITLE_TRANSITION)
            {
                robot.SetScreen(RobotScreen::TITLE);
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
        mouse_pos = GetMousePosition();
        cursor.Update(mouse_pos, false, {}, 0, false, false, false, false, false, false, dt);
        robot.Update(dt, mouse_pos);
        return;
    }

    if (game_state == GameState::TUTORIAL)
    {
        level_timer += GetFrameTime();
    }
    else if (!solved)
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
                robot.SetScreen(RobotScreen::LEVEL_COMPLETE);
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

    if (hovered_pin.IsValid()) 
    {
        robot.SetHoveredPin(&hovered_pin);
    }
    else
    {
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

    {
        bool wire_dragging = wire_drag_state.IsActive();
        Vector2 wire_start = {wire_drag_state.start_x, wire_drag_state.start_y};
        int wire_sig = 0;
        if (wire_dragging)
        {
            if (wire_drag_state.from_type == 0)
                wire_sig = input_bits[wire_drag_state.from_id];
            else if (wire_drag_state.from_type == 1)
            {
                auto it = gate_outputs.find(wire_drag_state.from_id);
                wire_sig = (it != gate_outputs.end()) ? it->second : 0;
            }
        }
        bool gate_dragging = (dragging_gate_id != -1);
        bool pin_is_hovered = hovered_pin.IsValid();
        bool gate_is_hovered =
            hovered_cell.IsValid() &&
            FindGateAt(hovered_cell.row, hovered_cell.col) != nullptr &&
            !pin_is_hovered;
        bool button_is_hovered =
            CheckMenuButtonClick(mouse_pos) ||
            CheckMusicButtonClick(mouse_pos) ||
            CheckCollisionPointRec(mouse_pos, GetClearButtonRect()) ||
            PickPaletteGate(mouse_pos) >= 0;
        bool is_forbidden = false;
        if (selected_gate_index >= 0 && hovered_cell.IsValid())
        {
            bool is_obstacle = false;
            for (const auto& o : obstacles)
            {
                if (o.row == hovered_cell.row && o.col == hovered_cell.col)
                {
                    is_obstacle = true;
                    break;
                }
            }
            is_forbidden = is_obstacle ||
                FindGateAt(hovered_cell.row, hovered_cell.col) != nullptr;
        }
        bool left_clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        cursor.Update
        (
            mouse_pos,
            wire_dragging, wire_start, wire_sig,
            gate_dragging,
            pin_is_hovered,
            gate_is_hovered,
            button_is_hovered,
            is_forbidden,
            left_clicked,
            GetFrameTime()
        );
    }

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
                    if (o.row == hovered_cell.row && o.col == hovered_cell.col)
                    {
                        is_obstacle = true;
                        break;
                    }
                }

                t_Gate* existing = FindGateAt(hovered_cell.row, hovered_cell.col);
                if (!is_obstacle && (!existing || existing->id == dragging_gate_id))
                {
                    t_Gate* g = FindGateById(dragging_gate_id);
                    if (g)
                    {
                        g->row = hovered_cell.row;
                        g->col = hovered_cell.col;
                        SpawnParticles
                        (
                            GetHexCenter(hovered_cell.row, hovered_cell.col),
                            {0, 255, 255, 255},
                            15
                        );
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
        if (GetRandomValue(0, 2) == 0) 
        {
            SpawnParticles(mouse_pos, {0, 245, 212, 200}, 1);
        }
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
        robot.SetScreen(RobotScreen::TITLE);
        game_state = GameState::PLAYING_TO_TITLE_TRANSITION;
        transition_time = 0;
        PlaySfx(SfxType::SOLVED);
    }
    if (IsKeyPressed(KEY_N))
    {
        SetMusicPlaying(!IsMusicPlaying());
        PlaySfx(SfxType::TOGGLE_INPUT);
    }

    if (wire_drag_state.IsActive() || dragging_gate_id != -1)
    {
        robot_last_action_time = anim_time;
    }

    if (game_state == GameState::TUTORIAL && tutorial.current_step >= 6)
    {
        float step_elapsed = anim_time - tutorial.step_enter_time;
        if (step_elapsed >= 2.0f)
        {
            game_state = GameState::TUTORIAL_TO_PLAY_TRANSITION;
            transition_time = 0;
            PlaySfx(SfxType::SOLVED);
            robot.OnSessionEnd(gates.size(), wires.size());
            return;
        }
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
        if (mood == RobotMood::HAPPY || mood == RobotMood::EXCITED) 
            SpawnParticles({p.x, p.y - 40}, {255, 220, 50, 255}, 1);
        else if (mood == RobotMood::ANGRY)
            SpawnParticles({p.x, p.y - 20}, {255, 50, 50, 255}, 1);
        else if (mood == RobotMood::SURPRISED)
            SpawnParticles({p.x, p.y - 50}, {100, 200, 255, 255}, 1);
    }
}
