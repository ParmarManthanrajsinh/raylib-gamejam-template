#pragma once
#include "gates.h"
#include <raylib.h>

void DrawBackground();
void DrawInfoBar(int target_hex, int current_hex, bool solved, float anim_time);
void DrawInputNodes(int input_bits[4], const t_Pin* hovered_pin, float anim_time);
void DrawOutputNode(int output_bits[4], int target_hex, const t_Pin* hovered_pin, bool has_wire, float anim_time);
int PickPaletteGate(Vector2 mouse_pos);
void DrawPalette(int selected_index); // -1 = none
Rectangle GetClearButtonRect();
Rectangle GetMenuButtonRect();
bool CheckMenuButtonClick(Vector2 mouse_pos);
void DrawMenuButton(float anim_time);
