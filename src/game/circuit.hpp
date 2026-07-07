#pragma once
#include "gates.hpp"
#include <vector>
#include <unordered_map>

struct Wire {
    int from_type;  // 0=input_node, 1=gate
    int from_id;    // input index or gate id
    int from_pin;   // always 0
    int to_type;    // 0=gate_input, 1=output_node
    int to_id;      // gate id or 0
    int to_pin;     // input pin index (0/1 for gates, 0-3 for output)
};

int EvaluateCircuit(
    const std::vector<Gate>& gates,
    const std::vector<Wire>& wires,
    const int input_bits[4],
    std::unordered_map<int, int>& gate_outputs,
    int output_bits[4]
);
