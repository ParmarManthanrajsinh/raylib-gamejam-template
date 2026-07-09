#pragma once
#include <raylib.h>
#include <string> // IWYU pragma: keep

enum class GateType
{
    AND,
    OR,
    NOT,
    XOR,
    NAND,
    NOR,
    XNOR
};

constexpr int GATE_COUNT = 7;

inline const char* GateTypeToString(GateType t)
{
    switch (t)
    {
    case GateType::AND:
        return "AND";
    case GateType::OR:
        return "OR";
    case GateType::NOT:
        return "NOT";
    case GateType::XOR:
        return "XOR";
    case GateType::NAND:
        return "NAND";
    case GateType::NOR:
        return "NOR";
    case GateType::XNOR:
        return "XNOR";
    }
    return "";
}

struct t_Gate
{
    int id = 0;
    GateType type = GateType::AND;
    int row = 0;
    int col = 0;
    float spawn_time = 0;
};

struct t_Pin
{
    int source_type; // 0=input_node, 1=gate, 2=output_node
    int source_id;   // input index (0-3), gate id, or 0
    int pin_index;   // pin number
    bool is_input;   // true if this is an input pin
};

// Gate pin queries
int GetGateInputCount(GateType type);
Vector2 GetGateInputPinPos(const t_Gate& gate, int pin_index);
Vector2 GetGateOutputPinPos(const t_Gate& gate);

// Input node pins
Vector2 GetInputNodeOutputPin(int index);
float GetInputNodeY(int index);

// Output node pins
Vector2 GetOutputNodeInputPin(int bit_index);

// Rendering
void DrawGateShape
(
    const t_Gate& gate,
    float x,
    float y,
    float w,
    float h,
    int output_val,
    float alpha = 1.0f
);
