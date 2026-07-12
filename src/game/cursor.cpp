#include "cursor.h"
#include "hex_grid.h"
#include <algorithm>
#include <cmath>

namespace
{
    constexpr int TRAIL_SKIP_FRAMES = 2;
    constexpr float RIPPLE_MAX_RADIUS = 40.0f;
    constexpr float RIPPLE_DURATION = 0.35f;
    constexpr float WIRE_SWAY_AMOUNT = 6.0f;

    void DrawArrowCursor
    (
        Vector2 pos,
        float s,
        Color state_color,
        float glow_alpha,
        float anim_time,
        float speed,
        Vector2 move_dir
    )
    {
        float bob = sinf(anim_time * 2.5f) * 0.8f * s;
        Vector2 tip = {pos.x, pos.y + bob};

        float speed_norm = fminf(speed / 800.0f, 1.0f);
        float stretch = 1.0f + speed_norm * 0.25f;
        float tilt = speed_norm * 0.12f * move_dir.x;

        float w = 10.0f * s;
        float h = 20.0f * s * stretch;

        Vector2 p1 = tip;
        Vector2 p2 = {tip.x - w, tip.y + h};
        Vector2 p3 = {tip.x - w * 0.25f, tip.y + h * 0.85f};
        Vector2 p4 = {tip.x + w * 0.65f, tip.y + h * 0.65f};

        if (tilt != 0.0f)
        {
            float ct = cosf(tilt);
            float st = sinf(tilt);
            auto rotate = [&](Vector2 p) -> Vector2
            {
                float rx = (p.x - tip.x) * ct - (p.y - tip.y) * st + tip.x;
                float ry = (p.x - tip.x) * st + (p.y - tip.y) * ct + tip.y;
                return {rx, ry};
            };
            p1 = rotate(p1);
            p2 = rotate(p2);
            p3 = rotate(p3);
            p4 = rotate(p4);
        }

        Color glow = state_color;
        glow.a = static_cast<unsigned char>(50 * glow_alpha);
        Vector2 glow_p1 = {tip.x, tip.y - 1.0f * s};
        Vector2 glow_p2 = {tip.x - w - 1.5f, tip.y + h + 1.5f};
        Vector2 glow_p3 = {tip.x - w * 0.25f - 1.0f, tip.y + h * 0.85f + 0.5f};
        Vector2 glow_p4 = {tip.x + w * 0.65f + 1.0f, tip.y + h * 0.65f + 0.5f};
        DrawTriangle(glow_p1, glow_p4, glow_p2, glow);
        DrawTriangle(glow_p2, glow_p3, glow_p1, glow);

        Color white_outline = WHITE;
        white_outline.a = static_cast<unsigned char>(255 * glow_alpha);
        float outline_w = 3.0f * s;
        Vector2 wo_p1 = tip;
        Vector2 wo_p2 = {tip.x - w - outline_w, tip.y + h + outline_w};
        Vector2 wo_p3 = {tip.x - w * 0.25f - outline_w, tip.y + h * 0.85f + outline_w * 0.5f};
        Vector2 wo_p4 = {tip.x + w * 0.65f + outline_w, tip.y + h * 0.65f + outline_w * 0.5f};
        DrawTriangle(wo_p1, wo_p4, wo_p2, white_outline);
        DrawTriangle(wo_p2, wo_p3, wo_p1, white_outline);

        DrawTriangle(p1, p4, p2, {25, 35, 55, 240});
        DrawTriangle(p2, p3, p1, {25, 35, 55, 240});

        Color outline_col = state_color;
        outline_col.a = static_cast<unsigned char>(220 * glow_alpha);
        DrawLineEx(p1, p2, 2.0f, outline_col);
        DrawLineEx(p2, p3, 1.5f, outline_col);
        DrawLineEx(p3, p4, 1.5f, outline_col);
        DrawLineEx(p4, p1, 2.0f, outline_col);

        Color highlight_col = state_color;
        highlight_col.a = static_cast<unsigned char>(100 * glow_alpha);
        Vector2 hl_p1 = {tip.x, tip.y + 2.0f * s};
        Vector2 hl_p2 = {tip.x - w * 0.5f, tip.y + h * 0.9f};
        Vector2 hl_p3 = {tip.x - w * 0.15f, tip.y + h * 0.7f};
        DrawTriangle(hl_p1, hl_p3, hl_p2, highlight_col);
    }

    Color GetStateColor(CursorState state)
    {
        switch (state)
        {
        case CursorState::DEFAULT:        return {200, 220, 255, 255};
        case CursorState::HOVER_PIN:      return {0, 255, 255, 255};
        case CursorState::HOVER_GATE:     return {0, 200, 255, 255};
        case CursorState::HOVER_BUTTON:   return {100, 200, 255, 255};
        case CursorState::DRAGGING_GATE:  return {255, 200, 50, 255};
        case CursorState::DRAGGING_WIRE:  return {0, 245, 212, 255};
        case CursorState::FORBIDDEN:      return {255, 60, 80, 255};
        }
        return {200, 220, 255, 255};
    }

    Vector2 BezierPoint(Vector2 p1, Vector2 p2, float t, float sway)
    {
        float dist = fabsf(p2.x - p1.x) * 0.5f;
        float offset = fmaxf(dist, 40.0f);
        float sway_y = sinf(t * 3.14159f * 2.0f + sway) * WIRE_SWAY_AMOUNT;
        Vector2 cp1 = {p1.x + offset, p1.y + sway_y};
        Vector2 cp2 = {p2.x - offset, p2.y - sway_y * 0.5f};
        float u = 1.0f - t;
        float uu = u * u;
        float uuu = uu * u;
        float tt = t * t;
        float ttt = tt * t;
        return
        {
            uuu * p1.x + 3.0f * uu * t * cp1.x + 3.0f * u * tt * cp2.x + ttt * p2.x,
            uuu * p1.y + 3.0f * uu * t * cp1.y + 3.0f * u * tt * cp2.y + ttt * p2.y,
        };
    }
}

void t_CustomCursor::SpawnParticle
(
    Vector2 pos,
    Vector2 vel,
    float life,
    Color color,
    float size
)
{
    for (auto& p : particles)
    {
        if (p.life <= 0.0f)
        {
            p.pos = pos;
            p.vel = vel;
            p.life = life;
            p.max_life = life;
            p.color = color;
            p.size = size;
            return;
        }
    }
}

void t_CustomCursor::SpawnBurst
(
    Vector2 pos,
    int count,
    float burst_speed,
    float life,
    Color color,
    float size
)
{
    for (int i = 0; i < count; i++)
    {
        float angle = static_cast<float>(i) / static_cast<float>(count) * 6.28318f;
        float spd = burst_speed * (0.5f + 0.5f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
        Vector2 vel = {cosf(angle) * spd, sinf(angle) * spd};
        float sz = size * (0.6f + 0.4f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
        SpawnParticle(pos, vel, life, color, sz);
    }
}

void t_CustomCursor::UpdateParticles(float dt)
{
    for (auto& p : particles)
    {
        if (p.life <= 0.0f) continue;
        p.pos.x += p.vel.x * dt;
        p.pos.y += p.vel.y * dt;
        p.vel.x *= 0.96f;
        p.vel.y *= 0.96f;
        p.life -= dt;
    }
}

void t_CustomCursor::Update
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
)
{
    prev_smooth = smooth_pos;
    raw_pos = mouse_pos;

    float dx = mouse_pos.x - smooth_pos.x;
    float dy = mouse_pos.y - smooth_pos.y;
    speed = sqrtf(dx * dx + dy * dy) / fmaxf(dt, 0.001f);

    smooth_pos.x += dx * fminf(dt * 28.0f, 1.0f);
    smooth_pos.y += dy * fminf(dt * 28.0f, 1.0f);

    CursorState new_state = CursorState::DEFAULT;
    if (wire_dragging)
        new_state = CursorState::DRAGGING_WIRE;
    else if (gate_dragging)
        new_state = CursorState::DRAGGING_GATE;
    else if (is_forbidden)
        new_state = CursorState::FORBIDDEN;
    else if (pin_hovered)
        new_state = CursorState::HOVER_PIN;
    else if (gate_hovered)
        new_state = CursorState::HOVER_GATE;
    else if (button_hovered)
        new_state = CursorState::HOVER_BUTTON;

    if (new_state != current_state)
    {
        prev_state = current_state;
        current_state = new_state;
        state_time = 0.0f;
        scale = 1.15f;

        Color burst_color = GetStateColor(current_state);
        SpawnBurst(smooth_pos, 6, 50.0f, 0.35f, burst_color, 2.0f);
    }

    if (wire_dragging)
    {
        wire_source = wire_start;
        wire_signal = wire_sig;
    }

    state_time += dt;

    trail_timer += dt;
    if (trail_timer >= static_cast<float>(TRAIL_SKIP_FRAMES) * (1.0f / 60.0f))
    {
        trail[trail_write] = smooth_pos;
        trail_write = (trail_write + 1) % CURSOR_TRAIL_LENGTH;
        trail_timer = 0.0f;
    }

    if (left_clicked)
    {
        ripple_radius = 2.0f;
        ripple_life = RIPPLE_DURATION;
        ripple_color = GetStateColor(current_state);
        SpawnBurst(smooth_pos, 10, 70.0f, 0.45f, ripple_color, 2.0f);
    }

    if (ripple_life > 0.0f)
    {
        ripple_life -= dt;
        ripple_radius += (RIPPLE_MAX_RADIUS - 2.0f) * (dt / RIPPLE_DURATION);
    }

    if (speed > 120.0f && current_state != CursorState::DRAGGING_WIRE)
    {
        Vector2 dir = {dx / fmaxf(dt, 0.001f), dy / fmaxf(dt, 0.001f)};
        float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
        if (len > 0.01f)
        {
            dir.x /= len;
            dir.y /= len;
        }
        Vector2 p_vel = {-dir.x * 25.0f, -dir.y * 25.0f};
        Color trail_col = GetStateColor(current_state);
        trail_col.a = 100;
        SpawnParticle(smooth_pos, p_vel, 0.25f, trail_col, 1.2f);
    }

    if (current_state == CursorState::HOVER_PIN ||
        current_state == CursorState::HOVER_GATE ||
        current_state == CursorState::HOVER_BUTTON)
    {
        if (static_cast<int>(state_time * 60.0f) % 5 == 0)
        {
            float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 6.28318f;
            float r = 10.0f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 8.0f;
            Vector2 spark_pos = {smooth_pos.x + cosf(angle) * r, smooth_pos.y + sinf(angle) * r + 6.0f};
            Vector2 spark_vel = {cosf(angle) * 12.0f, sinf(angle) * 12.0f - 15.0f};
            Color spark_col = GetStateColor(current_state);
            spark_col.a = 160;
            SpawnParticle(spark_pos, spark_vel, 0.35f, spark_col, 1.2f);
        }
    }

    if (current_state == CursorState::DRAGGING_WIRE)
    {
        if (static_cast<int>(state_time * 60.0f) % 3 == 0)
        {
            float t = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            Vector2 wire_pt = BezierPoint(wire_source, smooth_pos, t, state_time * 4.0f);
            Vector2 spark_vel = {
                (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f) * 35.0f,
                (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f) * 35.0f
            };
            Color spark_col = wire_signal == 1
                ? Color{0, 255, 255, 200}
                : Color{120, 140, 180, 150};
            SpawnParticle(wire_pt, spark_vel, 0.3f, spark_col, 1.5f);
        }
    }

    if (current_state == CursorState::DRAGGING_GATE)
    {
        if (static_cast<int>(state_time * 60.0f) % 6 == 0)
        {
            float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 6.28318f;
            Vector2 p_vel = {cosf(angle) * 20.0f, sinf(angle) * 20.0f};
            SpawnParticle(smooth_pos, p_vel, 0.25f, {255, 200, 50, 180}, 1.2f);
        }
    }

    if (current_state == CursorState::FORBIDDEN)
    {
        if (static_cast<int>(state_time * 60.0f) % 5 == 0)
        {
            float angle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 6.28318f;
            Vector2 p_vel = {cosf(angle) * 18.0f, sinf(angle) * 18.0f - 12.0f};
            SpawnParticle(smooth_pos, p_vel, 0.35f, {255, 80, 80, 160}, 1.8f);
        }
    }

    UpdateParticles(dt);

    float target_scale = 1.0f;
    if (current_state == CursorState::HOVER_BUTTON ||
        current_state == CursorState::HOVER_PIN)
    {
        target_scale = 1.06f + 0.02f * sinf(state_time * 6.0f);
    }
    else if (current_state == CursorState::DRAGGING_GATE)
    {
        target_scale = 0.94f + 0.04f * sinf(state_time * 8.0f);
    }
    else if (current_state == CursorState::FORBIDDEN)
    {
        target_scale = 0.90f + 0.04f * sinf(state_time * 20.0f);
    }

    if (scale > 1.01f)
    {
        scale += (1.0f - scale) * fminf(dt * 20.0f, 1.0f);
        if (fabsf(scale - 1.0f) < 0.003f) scale = 1.0f;
    }
    else
    {
        scale += (target_scale - scale) * fminf(dt * 14.0f, 1.0f);
    }
}

void t_CustomCursor::DrawTrail() const
{
    for (int i = 0; i < CURSOR_TRAIL_LENGTH; i++)
    {
        int idx = (trail_write - 1 - i + CURSOR_TRAIL_LENGTH * 2) % CURSOR_TRAIL_LENGTH;
        const auto& pos = trail[idx];
        if (pos.x < -50.0f) continue;

        float fade = 1.0f - static_cast<float>(i) / static_cast<float>(CURSOR_TRAIL_LENGTH);
        fade *= fade;
        float alpha = fade * 0.12f;
        float sz = fade * 4.0f;

        Color col = GetStateColor(current_state);
        col.a = static_cast<unsigned char>(255 * alpha);
        DrawCircleV(pos, sz, col);
    }
}

void t_CustomCursor::DrawRipple() const
{
    if (ripple_life <= 0.0f) return;
    float progress = 1.0f - (ripple_life / RIPPLE_DURATION);
    float radius = 2.0f + (RIPPLE_MAX_RADIUS - 2.0f) * progress;
    float alpha = (1.0f - progress) * 0.6f;

    Color col = ripple_color;
    col.a = static_cast<unsigned char>(255 * alpha);
    DrawCircleLines
    (
        static_cast<int>(smooth_pos.x),
        static_cast<int>(smooth_pos.y),
        radius, col
    );

    col.a = static_cast<unsigned char>(255 * alpha * 0.3f);
    DrawCircleLines
    (
        static_cast<int>(smooth_pos.x),
        static_cast<int>(smooth_pos.y),
        radius + 4.0f, col
    );
}

void t_CustomCursor::DrawParticles() const
{
    for (const auto& p : particles)
    {
        if (p.life <= 0.0f) continue;
        float life_ratio = p.life / p.max_life;
        Color col = p.color;
        col.a = static_cast<unsigned char>(col.a * life_ratio);
        float sz = p.size * (0.3f + 0.7f * life_ratio);
        DrawCircleV(p.pos, sz, col);
    }
}

void t_CustomCursor::DrawHand(float anim_time) const
{
    Color state_col = GetStateColor(current_state);

    float glow_alpha = 1.0f;
    if (current_state == CursorState::HOVER_PIN ||
        current_state == CursorState::HOVER_GATE ||
        current_state == CursorState::HOVER_BUTTON)
    {
        glow_alpha = 1.3f + 0.3f * sinf(state_time * 5.0f);
    }

    Vector2 move_dir = {0.0f, 0.0f};
    float dx = smooth_pos.x - prev_smooth.x;
    float dy = smooth_pos.y - prev_smooth.y;
    float len = sqrtf(dx * dx + dy * dy);
    if (len > 0.5f)
    {
        move_dir = {dx / len, dy / len};
    }

    DrawArrowCursor(smooth_pos, scale, state_col, glow_alpha, anim_time, speed, move_dir);
}

void t_CustomCursor::DrawWireCarry(float anim_time) const
{
    float sway = state_time * 4.0f;

    float dist = fabsf(smooth_pos.x - wire_source.x) * 0.5f;
    float offset = fmaxf(dist, 40.0f);
    Vector2 cp1 = {wire_source.x + offset, wire_source.y};
    Vector2 cp2 = {smooth_pos.x - offset, smooth_pos.y};

    bool active = wire_signal == 1;
    Color wire_col = active
        ? ColorAlpha({0, 255, 255, 255}, 0.9f)
        : ColorAlpha({140, 160, 190, 255}, 0.8f);
    Color wire_glow = active
        ? ColorAlpha({0, 255, 255, 255}, 0.25f)
        : ColorAlpha({80, 100, 140, 255}, 0.15f);
    Color wire_base = ColorAlpha({15, 25, 45, 255}, 0.7f);

    int segments = 32;
    Vector2 prev = wire_source;
    for (int i = 1; i <= segments; i++)
    {
        float t = static_cast<float>(i) / static_cast<float>(segments);
        Vector2 cur = BezierPoint(wire_source, smooth_pos, t, sway);

        float width_mod = 1.0f + 0.15f * sinf(t * 6.28f + anim_time * 3.0f);
        DrawLineEx(prev, cur, 8.0f * width_mod, wire_glow);
        DrawLineEx(prev, cur, 5.5f, wire_base);
        DrawLineEx(prev, cur, 2.8f, wire_col);
        prev = cur;
    }

    if (active)
    {
        float t_off = fmodf(anim_time * 2.5f, 1.0f);
        for (int i = 0; i < 7; i++)
        {
            float t = fmodf(t_off + static_cast<float>(i) * 0.143f, 1.0f);
            Vector2 dot = BezierPoint(wire_source, smooth_pos, t, sway);
            float sz = 4.5f + 1.5f * sinf(anim_time * 8.0f + static_cast<float>(i));
            DrawCircleV(dot, sz, ColorAlpha({0, 255, 255, 255}, 0.35f));
            DrawCircleV(dot, sz * 0.45f, ColorAlpha(WHITE, 0.85f));
        }
    }
    else
    {
        float t_off = fmodf(anim_time * 1.5f, 1.0f);
        for (int i = 0; i < 5; i++)
        {
            float t = fmodf(t_off + static_cast<float>(i) * 0.2f, 1.0f);
            Vector2 dot = BezierPoint(wire_source, smooth_pos, t, sway);
            DrawCircleV(dot, 3.0f, ColorAlpha({120, 140, 180, 255}, 0.25f));
            DrawCircleV(dot, 1.5f, ColorAlpha(WHITE, 0.5f));
        }
    }

    float src_pulse = 0.5f + 0.5f * sinf(anim_time * 8.0f);
    DrawCircleV(wire_source, 7.0f + src_pulse * 2.5f, ColorAlpha({0, 200, 255, 255}, 0.25f));
    DrawCircleV(wire_source, 3.5f, ColorAlpha(WHITE, 0.6f));

    Vector2 grip = {smooth_pos.x, smooth_pos.y};
    float grip_pulse = 0.5f + 0.5f * sinf(anim_time * 6.0f);
    DrawCircleV(grip, 5.0f + grip_pulse * 2.0f, ColorAlpha({0, 245, 212, 255}, 0.2f));
    DrawPoly(grip, 6, 4.0f, anim_time * 30.0f, ColorAlpha({0, 245, 212, 255}, 0.7f));
    DrawPolyLines(grip, 6, 4.0f, anim_time * 30.0f, ColorAlpha(WHITE, 0.8f));

    float bob = sinf(anim_time * 2.5f) * 0.8f;
    Vector2 tip = {smooth_pos.x, smooth_pos.y + bob};
    float grip_r = 3.5f + grip_pulse * 1.0f;
    DrawCircleV(tip, grip_r + 2.0f, ColorAlpha({0, 245, 212, 255}, 0.15f));
    DrawCircleV(tip, grip_r, ColorAlpha({0, 245, 212, 255}, 0.4f));
    DrawCircleV(tip, grip_r * 0.5f, ColorAlpha(WHITE, 0.8f));

    Vector2 tension_dir = {wire_source.x - smooth_pos.x, wire_source.y - smooth_pos.y};
    float tension_len = sqrtf(tension_dir.x * tension_dir.x + tension_dir.y * tension_dir.y);
    if (tension_len > 1.0f)
    {
        tension_dir.x /= tension_len;
        tension_dir.y /= tension_len;
    }
    float tension_strength = fminf(tension_len / 300.0f, 1.0f);
    Vector2 tension_end = {tip.x + tension_dir.x * 12.0f, tip.y + tension_dir.y * 12.0f};
    Color tension_col = active
        ? ColorAlpha({0, 255, 255, 255}, 0.3f * tension_strength)
        : ColorAlpha({120, 140, 180, 255}, 0.2f * tension_strength);
    DrawLineEx(tip, tension_end, 1.5f, tension_col);
}

void t_CustomCursor::DrawStateEffects(float anim_time) const
{
    Color state_col = GetStateColor(current_state);

    if (current_state == CursorState::HOVER_PIN)
    {
        float ring_t = fmodf(state_time * 3.0f, 1.0f);
        float ring_r = 10.0f + ring_t * 16.0f;
        float ring_a = (1.0f - ring_t) * 0.4f;
        DrawCircleLines
        (
            static_cast<int>(smooth_pos.x),
            static_cast<int>(smooth_pos.y + 7.0f),
            ring_r, ColorAlpha(state_col, ring_a)
        );

        float pulse = 0.5f + 0.5f * sinf(state_time * 8.0f);
        DrawCircleLines
        (
            static_cast<int>(smooth_pos.x),
            static_cast<int>(smooth_pos.y + 7.0f),
            15.0f + pulse * 3.0f,
            ColorAlpha(state_col, 0.3f + 0.2f * pulse)
        );
    }
    else if (current_state == CursorState::HOVER_GATE)
    {
        float pulse = 0.5f + 0.5f * sinf(state_time * 4.0f);
        DrawCircleV({smooth_pos.x, smooth_pos.y + 7.0f}, 22.0f, ColorAlpha(state_col, 0.06f * pulse));
        DrawCircleV({smooth_pos.x, smooth_pos.y + 7.0f}, 15.0f, ColorAlpha(state_col, 0.1f * pulse));

        for (int i = 0; i < 3; i++)
        {
            float angle = anim_time * 2.0f + static_cast<float>(i) * 2.094f;
            float r = 18.0f;
            Vector2 spark = {smooth_pos.x + cosf(angle) * r, smooth_pos.y + 7.0f + sinf(angle) * r};
            DrawCircleV(spark, 1.5f, ColorAlpha(state_col, 0.5f * pulse));
        }
    }
    else if (current_state == CursorState::HOVER_BUTTON)
    {
        float pulse = 0.5f + 0.5f * sinf(state_time * 5.0f);
        DrawCircleV({smooth_pos.x, smooth_pos.y + 7.0f}, 18.0f, ColorAlpha(state_col, 0.05f * pulse));
    }
    else if (current_state == CursorState::DRAGGING_GATE)
    {
        float bob = sinf(state_time * 10.0f) * 1.5f;
        Vector2 bob_pos = {smooth_pos.x, smooth_pos.y + 7.0f + bob};
        DrawCircleV(bob_pos, 18.0f, ColorAlpha(state_col, 0.08f));
        DrawCircleV(bob_pos, 12.0f, ColorAlpha(state_col, 0.12f));
    }
    else if (current_state == CursorState::FORBIDDEN)
    {
        float shake_x = sinf(state_time * 30.0f) * 2.5f;
        Vector2 sp = {smooth_pos.x + shake_x, smooth_pos.y + 7.0f};

        DrawCircleV(sp, 16.0f, ColorAlpha(state_col, 0.12f));

        float cs = 8.0f;
        DrawLineEx
        (
            {sp.x - cs, sp.y - cs}, {sp.x + cs, sp.y + cs},
            2.5f, ColorAlpha(state_col, 0.85f)
        );
        DrawLineEx
        (
            {sp.x + cs, sp.y - cs}, {sp.x - cs, sp.y + cs},
            2.5f, ColorAlpha(state_col, 0.85f)
        );
    }
}

void t_CustomCursor::Draw(float anim_time) const
{
    DrawTrail();
    DrawRipple();

    if (current_state == CursorState::DRAGGING_WIRE)
    {
        DrawWireCarry(anim_time);
    }

    DrawStateEffects(anim_time);
    DrawHand(anim_time);
    DrawParticles();
}
