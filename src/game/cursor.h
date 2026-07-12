#pragma once
#include <array>
#include <raylib.h>

enum class CursorState
{
    DEFAULT,
    HOVER_PIN,
    HOVER_GATE,
    HOVER_BUTTON,
    DRAGGING_GATE,
    DRAGGING_WIRE,
    FORBIDDEN,
};

struct t_CursorParticle
{
    Vector2 pos = {0.0f, 0.0f};
    Vector2 vel = {0.0f, 0.0f};
    float life = 0.0f;
    float max_life = 0.0f;
    Color color = WHITE;
    float size = 1.0f;
};

constexpr int CURSOR_TRAIL_LENGTH = 8;
constexpr int CURSOR_PARTICLE_MAX = 32;

struct t_CustomCursor
{
    CursorState current_state = CursorState::DEFAULT;
    CursorState prev_state = CursorState::DEFAULT;
    float state_time = 0.0f;
    float scale = 1.0f;
    Vector2 smooth_pos = {-100.0f, -100.0f};
    Vector2 raw_pos = {-100.0f, -100.0f};
    Vector2 prev_smooth = {-100.0f, -100.0f};
    Vector2 wire_source = {-100.0f, -100.0f};
    int wire_signal = 0;

    std::array<Vector2, CURSOR_TRAIL_LENGTH> trail = {};
    int trail_write = 0;
    float trail_timer = 0.0f;

    std::array<t_CursorParticle, CURSOR_PARTICLE_MAX> particles = {};

    float ripple_radius = 0.0f;
    float ripple_life = 0.0f;
    Color ripple_color = WHITE;

    float speed = 0.0f;

    void Update
    (
        Vector2 mouse_pos,
        bool wire_dragging,
        Vector2 wire_start,
        int wire_sig,
        bool gate_dragging,
        bool pin_hovered,
        bool gate_hovered,
        bool button_hovered,
        bool is_forbidden,
        bool left_clicked,
        float dt
    );

    void Draw(float anim_time) const;

private:
    void SpawnParticle
    (
        Vector2 pos,
        Vector2 vel,
        float life,
        Color color,
        float size
    );
    void SpawnBurst
    (
        Vector2 pos,
        int count,
        float speed,
        float life,
        Color color,
        float size
    );
    void UpdateParticles(float dt);
    void DrawParticles() const;
    void DrawTrail() const;
    void DrawRipple() const;
    void DrawHand(float anim_time) const;
    void DrawWireCarry(float anim_time) const;
    void DrawStateEffects(float anim_time) const;
};
