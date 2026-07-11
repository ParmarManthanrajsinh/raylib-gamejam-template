#pragma once

#include <raylib.h>
#include <unordered_map>
#include <vector>

#include "circuit.h"
#include "gates.h"
#include "hex_grid.h"
#include "menu.h"
#include "wires.h"

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

struct Particle
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

        // Core state
        std::vector<t_Gate> gates;
        std::vector<t_Wire> wires;
        int input_bits[4];
        int output_bits[4];
        std::unordered_map<int, int> gate_outputs;
        int target_hex;
        int gate_id_counter;
        bool solved;
        float anim_time;
        float solved_pulse;
        float transition_time;

        // UI state
        int selected_gate_index; // -1 = none
        int dragging_gate_id;    // -1 = none
        t_WireDragState wire_drag_state;
        t_HexCell hovered_cell;
        t_PinHit hovered_pin;
        Vector2 mouse_pos;
        Vector2 ghost_pos;

        // Juice
        std::vector<Particle> particles;
        float screen_shake_time;
        RenderTexture2D render_target;

        // Helpers
        void Reset();
        void Evaluate();
        t_Gate* FindGateAt(int row, int col);
        t_Gate* FindGateById(int id);
        t_PinHit FindPinAt(Vector2 pos);
        void RemoveWiresForGate(int gate_id);
        void SpawnParticles(Vector2 pos, Color color, int count);

        // Event handlers
        void HandleClick(Vector2 pos);
        void HandleRightClick(Vector2 pos);
};
