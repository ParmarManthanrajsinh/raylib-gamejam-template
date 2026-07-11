#pragma once
#include <raylib.h>
#include <string>
#include "gates.h" // For GateType
#include "hex_grid.h" // IWYU pragma: keep

enum class RobotMood { IDLE, HAPPY, ANGRY, SURPRISED, SAD, EXCITED, SASSY };
enum class RobotScreen { TITLE, HOW_TO_PLAY, PLAYING, LEVEL_COMPLETE };

struct t_HexBot
{
    Vector2 base_pos = {660, 530};
    Vector2 current_pos = {660, 530};
    Vector2 target_pos = {660, 530};
    RobotScreen current_screen = RobotScreen::PLAYING;
    RobotMood current_mood = RobotMood::IDLE;

    // Animation state
    float anim_time = 0;
    Vector2 eye_offset = {0, 0};
    float blink_timer = 0;
    bool is_blinking = false;

    // Dialog state
    std::string current_dialog = "";
    float dialog_timer = 0;
    int dialog_priority = 0; // 0=idle, 1=event, 2=interact, 3=boop, 4=solve, 5=cheat

    // Typewriter effect
    int type_cursor = 0;
    float type_timer = 0;

    // Interaction
    bool is_hovering_pin = false;
    float pin_hover_timer = 0.0f;
    float proximity_cooldown = 0.0f;
    int hovered_pin_type = -1;

    // Deletion spree tracking
    int recent_deletes = 0;
    float last_delete_time = -100.0f;

    // Phase 1 & 2 tracking
    int wire_cancel_count = 0;
    float wire_cancel_timer = 0.0f;
    float last_hint_time = 0.0f;
    int trust = 50;
    float patience = 60.0f;

    // Phase 3 tracking
    float dialog_duration = 0.0f;
    float dialog_elapsed = 0.0f;
    float boop_timer = 0.0f;

    // Phase 4 tracking
    float last_palette_hover_time = -100.0f;
    int last_palette_hover_type = -1;
    
    // Screen dialog timer
    float screen_dialog_timer = 0.0f;
    
    // Trail for flying
    std::vector<Vector2> trail;
};

class Robot
{
public:
    Robot();

    void Update
    (
        float game_anim_time, Vector2 mouse_pos, float level_timer,
        int output_bits[4], int target_hex, bool solved,
        float mouse_still_time, float last_action_time,
        float& idle_timer, int gate_type_counts[7],
        int delete_count, int wire_count,
        int obstacle_attempts, int& matching_bits_prev
    );

    void Draw(float game_anim_time, Vector2 mouse_pos);
    void Update(float dt, Vector2 mouse_pos);
    void SetScreen(RobotScreen screen);

    // Event Hooks
    void OnGatePlaced(GateType type, int total_gates);
    void OnFirstGatePlaced(GateType type);
    void OnWireConnected(int total_wires, int total_gates);
    void OnFirstWireConnected();
    void OnWireDeleted();
    void OnCheat();
    void OnGateDeleted(GateType type, int total_gates);
    void OnClearPressed();
    void OnSolved(int total_gates, int total_wires, float level_timer);
    void OnLevelStart(int target_hex);
    void OnPaletteHover(GateType type);
    void OnObstacleAttempt();
    void OnWireDragCancelled();
    void SetHoveredPin(const void* pin);
    void OnCheatDetected(const std::string& cheat_type);
    void OnSessionEnd(int total_gates, int total_wires);

    RobotMood GetMood() const { return bot.current_mood; }
    Vector2 GetPos() const { return bot.current_pos; }

private:
    t_HexBot bot;

    void Speak
    (
        const std::string& text,
        int priority = 1,
        RobotMood mood = RobotMood::IDLE,
        float duration = 0.0f
    );
    void UpdateAnimation(float dt, Vector2 mouse_pos);
};
