#pragma once
#include <raylib.h>

constexpr int     SCREEN_WIDTH     = 720;
constexpr int     SCREEN_HEIGHT    = 720;
constexpr int     K_GRID_COLS      = 8;
constexpr int     K_GRID_ROWS      = 8;
constexpr float   K_HEX_SIZE       = 38.0f;
constexpr float   K_SQRT3          = 1.73205080757f;
constexpr float   K_SPACING_X      = K_HEX_SIZE * K_SQRT3;
constexpr float   K_SPACING_Y      = K_HEX_SIZE * 1.5f;
constexpr float   K_GRID_X         = 100.0f;
constexpr float   K_GRID_Y         = 106.0f;
constexpr float   K_GRID_W         = (K_GRID_COLS - 1) * K_SPACING_X + K_SPACING_X / 2 + K_HEX_SIZE * K_SQRT3;
constexpr float   K_GRID_H         = (K_GRID_ROWS - 1) * K_SPACING_Y + K_HEX_SIZE * 2;

constexpr float   K_INFO_BAR_H     = 48.0f;
constexpr float   K_PALETTE_H      = 56.0f;
constexpr float   K_PALETTE_Y      = K_GRID_Y + K_GRID_H - 10;
constexpr float   K_INPUT_X        = 36.0f;
constexpr float   K_INPUT_PIN_X    = 56.0f;
constexpr float   K_OUTPUT_CENTER_X = 676.0f;
constexpr float   K_OUTPUT_CENTER_Y = K_GRID_Y + K_GRID_H / 2;
constexpr float   K_OUTPUT_PIN_X   = 642.0f;
constexpr float   K_PIN_RADIUS     = 7.0f;
constexpr float   K_PIN_HIT_RADIUS = 12.0f;

struct HexCell 
{
    int row = -1;
    int col = -1;

    bool IsValid() const { return row >= 0 && row < K_GRID_ROWS && col >= 0 && col < K_GRID_COLS; }
    bool operator==(const HexCell& o) const { return row == o.row && col == o.col; }
    bool operator!=(const HexCell& o) const { return !(*this == o); }
};

Vector2  GetHexCenter(int row, int col);
void     DrawFilledHexagon(Vector2 center, float size, Color color);
void     DrawHexOutline(Vector2 center, float size, float thickness, Color color);
bool     IsPointInHex(Vector2 point, Vector2 center, float size);
void     DrawGrid(const HexCell& hovered_cell, bool show_placement, bool is_occupied, float anim_time);
HexCell  GetGridCell(Vector2 mouse_pos);
Rectangle GetGridRect();
