#include "hex_grid.hpp"
#include <cmath>
#include <raylib.h>

static constexpr float k_bg_margin = 10.0f;

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
    return 
    {
        K_GRID_X - k_bg_margin,
        K_GRID_Y - k_bg_margin,
        K_GRID_W + k_bg_margin * 2,
        K_GRID_H + k_bg_margin * 2
    };
}

void DrawGrid(const HexCell& hovered_cell, bool show_placement, bool is_occupied)
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

            // Center dot
            DrawCircleV(c, 1.5f, dot_color);
        }
    }

    // Hover highlight
    if (hovered_cell.IsValid()) 
    {
        Vector2 c = GetHexCenter(hovered_cell.row, hovered_cell.col);
        if (show_placement && !is_occupied) 
        {
            Color highlight_fill = ColorAlpha(SKYBLUE, 0.25f);
            DrawFilledHexagon(c, K_HEX_SIZE - 2, highlight_fill);
            DrawHexOutline(c, K_HEX_SIZE - 2, 2.5f, SKYBLUE);
        } 
        else if (!is_occupied) 
        {
            Color hover_fill = ColorAlpha(RAYWHITE, 0.06f);
            DrawFilledHexagon(c, K_HEX_SIZE - 2, hover_fill);
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
