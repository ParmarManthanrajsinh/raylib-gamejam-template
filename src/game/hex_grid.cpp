#include "hex_grid.hpp"
#include <cmath>
#include <raylib.h>

static constexpr float k_bg_margin = 16.0f;

void DrawFilledHexagon(Vector2 center, float size, Color fill) 
{
    Vector2 v[6] = {};

    for (int i = 0; i < 6; i++) 
    {
        float a = PI / 6 + (PI / 3) * i;
        v[i] = { center.x + size * cosf(a), center.y + size * sinf(a) };
    }

    DrawTriangle(v[0], v[1], v[2], fill);
    DrawTriangle(v[0], v[2], v[3], fill);
    DrawTriangle(v[0], v[3], v[4], fill);
    DrawTriangle(v[0], v[4], v[5], fill);
}

void DrawHexOutline(Vector2 center, float size, float thickness, Color color) 
{
    for (int i = 0; i < 6; i++) 
    {
        float a1 = PI / 6 + (PI / 3) * i;
        float a2 = PI / 6 + (PI / 3) * ((i + 1) % 6);
        Vector2 p1 = { center.x + size * cosf(a1), center.y + size * sinf(a1) };
        Vector2 p2 = { center.x + size * cosf(a2), center.y + size * sinf(a2) };
        DrawLineEx(p1, p2, thickness, color);
    }
}

Vector2 GetHexCenter(int row, int col) 
{
    float x = K_GRID_X + col * K_SPACING_X;
    if (row % 2 != 0) x += K_SPACING_X / 2;
    return { x, K_GRID_Y + row * K_SPACING_Y };
}

bool IsPointInHex(Vector2 point, Vector2 center, float size)
{
    float dx = fabsf(point.x - center.x);
    float dy = fabsf(point.y - center.y);
    float half_w = size * K_SQRT3 / 2;

    if (dx > half_w || dy > size) return false;
    if (dy <= size / 2) return true;

    return dx <= K_SQRT3 * (size - dy);
}

Rectangle GetGridRect() 
{
    float hex_half_w = K_HEX_SIZE * K_SQRT3 / 2;
    float left   = K_GRID_X - hex_half_w;
    float right  = K_GRID_X + (K_GRID_COLS - 1) * K_SPACING_X + hex_half_w;
    if ((K_GRID_ROWS - 1) % 2 != 0) right += K_SPACING_X / 2;

    float top    = K_GRID_Y - K_HEX_SIZE;
    float bottom = K_GRID_Y + (K_GRID_ROWS - 1) * K_SPACING_Y + K_HEX_SIZE;

    return 
    {
        left  - k_bg_margin,
        top   - k_bg_margin,
        (right - left)   + k_bg_margin * 2,
        (bottom - top)   + k_bg_margin * 2
    };
}

void DrawGrid(const HexCell& hovered_cell, bool show_placement, bool is_occupied, float anim_time)
{
    Color fill_even = { 21, 21, 48, 255 };
    Color fill_odd  = { 18, 18, 40, 255 };
    Color outline   = { 42, 58, 74, 255 };
    Color dot_color = { 42, 58, 90, 255 };

    for (int row = 0; row < K_GRID_ROWS; row++) 
    {
        for (int col = 0; col < K_GRID_COLS; col++) 
        {
            Vector2 c = GetHexCenter(row, col);
            Color fill = (row + col) % 2 == 0 ? fill_even : fill_odd;

            DrawFilledHexagon(c, K_HEX_SIZE - 1, fill);
            DrawHexOutline(c, K_HEX_SIZE, 1.5f, outline);

            DrawCircleV(c, 1.5f, dot_color);
        }
    }

    if (hovered_cell.IsValid()) 
    {
        Vector2 c = GetHexCenter(hovered_cell.row, hovered_cell.col);
        float pulse = 0.7f + 0.3f * sinf(anim_time * 4.0f);

        DrawFilledHexagon(c, K_HEX_SIZE + 3, ColorAlpha(SKYBLUE, 0.04f * pulse));

        if (show_placement && !is_occupied) 
        {
            DrawFilledHexagon(c, K_HEX_SIZE - 2, ColorAlpha(SKYBLUE, 0.22f * pulse));
            DrawHexOutline(c, K_HEX_SIZE - 1, 2.5f, ColorAlpha(SKYBLUE, 0.7f * pulse));
            DrawCircleV(c, 3, ColorAlpha(SKYBLUE, 0.9f));
            DrawCircleLines((int)c.x, (int)c.y, 6, ColorAlpha(SKYBLUE, 0.4f * pulse));
        } 
        else if (show_placement && is_occupied) 
        {
            DrawFilledHexagon(c, K_HEX_SIZE - 2, ColorAlpha(RED, 0.12f));
            DrawHexOutline(c, K_HEX_SIZE - 1, 2.0f, ColorAlpha(RED, 0.5f * pulse));
        } 
        else if (!is_occupied) 
        {
            DrawHexOutline(c, K_HEX_SIZE - 1, 2.0f, ColorAlpha(RAYWHITE, 0.25f * pulse));
            DrawCircleV(c, 2, ColorAlpha(RAYWHITE, 0.4f));
        } 
        else 
        {
            DrawHexOutline(c, K_HEX_SIZE - 1, 1.5f, ColorAlpha(RAYWHITE, 0.10f));
        }
    }
}

HexCell GetGridCell(Vector2 mouse_pos) 
{
    int approx_col = (int)floorf((mouse_pos.x - K_GRID_X) / K_SPACING_X);
    int approx_row = (int)floorf((mouse_pos.y - K_GRID_Y) / K_SPACING_Y);

    for (int dr = -1; dr <= 1; dr++) 
    {
        for (int dc = -1; dc <= 1; dc++) 
        {
            int row = approx_row + dr;
            int col = approx_col + dc;

            if (row < 0 || row >= K_GRID_ROWS || col < 0 || col >= K_GRID_COLS) 
            {
                continue;
            }

            Vector2 c = GetHexCenter(row, col);

            if (IsPointInHex(mouse_pos, c, K_HEX_SIZE)) 
            {
                return { row, col };
            }
        }
    }
    return {};
}
