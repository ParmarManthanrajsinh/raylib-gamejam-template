#pragma once
#include <raylib.h>

constexpr int SCREEN_WIDTH = 720;
constexpr int SCREEN_HEIGHT = 720;
constexpr int GRID_COLS = 7;
constexpr int GRID_ROWS = 8;
constexpr float HEX_SIZE = 40.0f;
constexpr float SQRT3 = 1.73205080757f;
constexpr float SPACING_X = HEX_SIZE * SQRT3;
constexpr float SPACING_Y = HEX_SIZE * 1.5f;
constexpr float GRID_X = 105.0f;
constexpr float GRID_Y = 110.0f;
constexpr float GRID_W = (GRID_COLS - 1) * SPACING_X + SPACING_X / 2 + HEX_SIZE * SQRT3;
constexpr float GRID_H = (GRID_ROWS - 1) * SPACING_Y + HEX_SIZE * 2;

constexpr float INFO_BAR_H = 36.0f;
constexpr float PALETTE_H = 48.0f;
constexpr float PALETTE_Y = 660.0f;
constexpr float INPUT_X = 32.0f;
constexpr float INPUT_PIN_X = 52.0f;
constexpr float OUTPUT_CENTER_X = 665.0f;
constexpr float OUTPUT_CENTER_Y = GRID_Y + GRID_H / 2;
constexpr float OUTPUT_PIN_X = 615.0f;
constexpr float PIN_RADIUS = 7.0f;
constexpr float PIN_HIT_RADIUS = 12.0f;

struct t_HexCell
{
    int row = -1;
    int col = -1;

    bool IsValid() const
    {
        return row >= 0 && row < GRID_ROWS && col >= 0 && col < GRID_COLS;
    }
    bool operator==(const t_HexCell& o) const
    {
        return row == o.row && col == o.col;
    }
    bool operator!=(const t_HexCell& o) const
    {
        return !(*this == o);
    }
};

Vector2 GetHexCenter(int row, int col);
void DrawFilledHexagon(Vector2 center, float size, Color color);
void DrawHexOutline(Vector2 center, float size, float thickness, Color color);
bool IsPointInHex(Vector2 point, Vector2 center, float size);
void DrawGrid(const t_HexCell& hovered_cell, bool show_placement, bool is_occupied, float anim_time);
t_HexCell GetGridCell(Vector2 mouse_pos);
Rectangle GetGridRect();
