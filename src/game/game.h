#pragma once

#include <raylib.h>
#include <unordered_map>
#include <vector>

#include "circuit.h"
#include "cursor.h"
#include "gates.h"
#include "hex_grid.h"
#include "menu.h"
#include "wires.h"
#include "robot.h"
#include "tutorial.h"
#include "save_data.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

struct t_PinHit
{
    int source_type = -1;
    int source_id = 0;
    int pin_index = 0;
    bool is_input = false;

    bool IsValid() const
    {
        return source_type >= 0;
    }
};

struct t_Particle
{
    Vector2 pos;
    Vector2 vel;
    float life;
    float max_life;
    Color color;
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
        // Game state
        GameState game_state;
        t_SaveData save_data;
        t_TutorialState tutorial;
        bool tutorial_required;

        // Core state
        std::vector<t_Gate> gates;
        std::vector<t_Wire> wires;
        std::vector<t_HexCell> obstacles;
        int input_bits[4];
        int output_bits[4];
        std::unordered_map<int, int> gate_outputs;
        int target_hex;
        int gate_id_counter;
        bool solved;
        float anim_time;
        float solved_pulse;
        float transition_time;
        float level_complete_delay;
        float level_timer;
        t_GameStats last_stats;

        // UI state
        int selected_gate_index; // -1 = none
        int dragging_gate_id;    // -1 = none
        t_WireDragState wire_drag_state;
        t_HexCell hovered_cell;
        t_PinHit hovered_pin;
        Vector2 mouse_pos;
        Vector2 ghost_pos;
        t_CustomCursor cursor;

        // Juice
        std::vector<t_Particle> particles;
        float screen_shake_time;
        RenderTexture2D render_target;

        // Helpers
        void Reset(bool is_clear = false);
        void SetupTutorial();
        void Evaluate();
        t_Gate* FindGateAt(int row, int col);
        t_Gate* FindGateById(int id);
        t_PinHit FindPinAt(Vector2 pos);
        void RemoveWiresForGate(int gate_id);
        void SpawnParticles(Vector2 pos, Color color, int count);

        // Robot tracking state
        Robot robot;
        float robot_idle_timer = 0;
        int robot_gate_type_counts[7] = {};
        int robot_delete_count = 0;
        float robot_last_action_time = 0;
        Vector2 robot_last_mouse_pos = {-100, -100};
        float robot_mouse_still_time = 0;
        bool robot_first_gate_placed = false;
        bool robot_first_wire_connected = false;
        int robot_obstacle_attempts = 0;
        int robot_matching_bits_prev = 0;

        // Event handlers
        void HandleClick(Vector2 pos);
        void HandleRightClick(Vector2 pos);
};
