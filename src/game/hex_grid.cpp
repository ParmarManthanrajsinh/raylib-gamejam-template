#include "hex_grid.h"
#include <cmath>
#include <raylib.h>
static constexpr float bg_margin = 16.0f;

void DrawFilledHexagon(Vector2 center, float size, Color fill)
{
    Vector2 v[6] = {};

    for (int i = 0; i < 6; i++)
    {
        float a = PI / 6 + (PI / 3) * i;
        v[i] = {center.x + size * cosf(a), center.y + size * sinf(a)};
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
        Vector2 p1 = {center.x + size * cosf(a1), center.y + size * sinf(a1)};
        Vector2 p2 = {center.x + size * cosf(a2), center.y + size * sinf(a2)};
        DrawLineEx(p1, p2, thickness, color);
    }
}

Vector2 GetHexCenter(int row, int col)
{
    float x = GRID_X + col * SPACING_X;
    if (row % 2 != 0) x += SPACING_X / 2;
    return {x, GRID_Y + row * SPACING_Y};
}

bool IsPointInHex(Vector2 point, Vector2 center, float size)
{
    float dx = fabsf(point.x - center.x);
    float dy = fabsf(point.y - center.y);
    float half_w = size * SQRT3 / 2;
    if (dx > half_w || dy > size) return false;
    if (dy <= size / 2) return true;
    return dx <= SQRT3 * (size - dy);
}

Rectangle GetGridRect()
{
    float hex_half_w = HEX_SIZE * SQRT3 / 2;
    float left = GRID_X - hex_half_w;
    float right = GRID_X + (GRID_COLS - 1) * SPACING_X + hex_half_w;
    if ((GRID_ROWS - 1) % 2 != 0) right += SPACING_X / 2;
    float top = GRID_Y - HEX_SIZE;
    float bottom = GRID_Y + (GRID_ROWS - 1) * SPACING_Y + HEX_SIZE;

    return 
    {
        left - bg_margin, 
        top - bg_margin, 
        (right - left) + bg_margin * 2,
        (bottom - top) + bg_margin * 2
    };
}

void DrawGrid
(
    const t_HexCell& hovered_cell,
    bool show_placement,
    bool is_occupied,
    float anim_time
)
{
    Color fill_even = {10, 14, 28, 255};
    Color fill_odd = {8, 11, 22, 255};
    Color outline = {30, 48, 80, 255};
    Color dot_color = {40, 60, 100, 255};

    for (int row = 0; row < GRID_ROWS; row++)
    {
        for (int col = 0; col < GRID_COLS; col++)
        {
            Vector2 c = GetHexCenter(row, col);
            Color fill = (row + col) % 2 == 0 ? fill_even : fill_odd;
            DrawFilledHexagon(c, HEX_SIZE - 1, fill);
            DrawHexOutline(c, HEX_SIZE, 1.5f, outline);
            DrawCircleV(c, 1.5f, dot_color);
        }
    }

    if (hovered_cell.IsValid())
    {
        Vector2 c = GetHexCenter(hovered_cell.row, hovered_cell.col);
        float pulse = 0.7f + 0.3f * sinf(anim_time * 4.0f);

        DrawFilledHexagon(c, HEX_SIZE + 3, ColorAlpha(SKYBLUE, 0.04f * pulse));

        if (show_placement && !is_occupied)
        {
            DrawFilledHexagon(c, HEX_SIZE - 2, ColorAlpha(SKYBLUE, 0.22f * pulse));
            DrawHexOutline(c, HEX_SIZE - 1, 2.5f, ColorAlpha(SKYBLUE, 0.7f * pulse));
            DrawCircleV(c, 3, ColorAlpha(SKYBLUE, 0.9f));
            DrawCircleLines(static_cast<int>(c.x), static_cast<int>(c.y), 6, ColorAlpha(SKYBLUE, 0.4f * pulse));
        }
        else if (show_placement && is_occupied)
        {
            DrawFilledHexagon(c, HEX_SIZE - 2, ColorAlpha(RED, 0.12f));
            DrawHexOutline(c, HEX_SIZE - 1, 2.0f, ColorAlpha(RED, 0.5f * pulse));
        }
        else if (!is_occupied)
        {
            DrawFilledHexagon(c, HEX_SIZE - 2, ColorAlpha({0, 255, 255, 255}, 0.1f * pulse));
            DrawHexOutline(c, HEX_SIZE - 1, 2.5f, ColorAlpha({0, 255, 255, 255}, 0.5f * pulse));
            DrawCircleV(c, 2, ColorAlpha({0, 255, 255, 255}, 0.6f));
        }
        else
        {
            DrawHexOutline(c, HEX_SIZE - 1, 1.5f, ColorAlpha(RAYWHITE, 0.10f));
        }
    }
}

t_HexCell GetGridCell(Vector2 mouse_pos)
{
    int approx_col =
        static_cast<int>(floorf((mouse_pos.x - GRID_X) / SPACING_X));
    int approx_row =
        static_cast<int>(floorf((mouse_pos.y - GRID_Y) / SPACING_Y));

    for (int dr = -1; dr <= 1; dr++)
    {
        for (int dc = -1; dc <= 1; dc++)
        {
            int row = approx_row + dr;
            int col = approx_col + dc;

            if (row < 0 || row >= GRID_ROWS || col < 0 || col >= GRID_COLS)
            {
                continue;
            }

            Vector2 c = GetHexCenter(row, col);

            if (IsPointInHex(mouse_pos, c, HEX_SIZE))
            {
                return {row, col};
            }
        }
    }
    return {};
}
