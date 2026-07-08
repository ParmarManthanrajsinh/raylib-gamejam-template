#include "circuit.h"

namespace
{
    int EvaluateGate(GateType type, int a, int b)
    {
        switch (type)
        {
            case GateType::AND:
                return (a == 1 && b == 1) ? 1 : 0;
            case GateType::OR:
                return (a == 1 || b == 1) ? 1 : 0;
            case GateType::NOT:
                return a == 1 ? 0 : 1;
            case GateType::XOR:
                return (a != b) ? 1 : 0;
            case GateType::NAND:
                return (a == 1 && b == 1) ? 0 : 1;
            case GateType::NOR:
                return (a == 1 || b == 1) ? 0 : 1;
            case GateType::XNOR:
                return (a == b) ? 1 : 0;
        }
        return 0;
    }
}

int EvaluateCircuit
(
    const std::vector<t_Gate>& gates,
    const std::vector<t_Wire>& wires,
    const int input_bits[4],
    std::unordered_map<int, int>& gate_outputs,
    int output_bits[4]
)
{
    // Reset
    gate_outputs.clear();
    for (const auto& gate : gates) gate_outputs[gate.id] = 0;

    for (int i = 0; i < 4; i++) output_bits[i] = 0;
    struct t_SourceInfo { int type, id, pin; };
    
    std::unordered_map<int, t_SourceInfo> wire_targets{};
    for (const auto& w : wires)
    {
        int key = (w.to_type << 16) | (w.to_id << 8) | w.to_pin;
        wire_targets[key] = {w.from_type, w.from_id, w.from_pin};
    }

    auto get_source_value = [&](int src_type, int src_id) -> int
    {
        if (src_type == 0) return input_bits[src_id];

        if (src_type == 1)
        {
            auto it = gate_outputs.find(src_id);
            if (it != gate_outputs.end()) return it->second;
        }
        return 0;
    };

    // Iterative evaluation
    int max_iters = static_cast<int>(gates.size()) + 10;
    for (int iter = 0; iter < max_iters; iter++)
    {
        bool changed = false;
        for (const auto& gate : gates)
        {
            int input_count = GetGateInputCount(gate.type);
            int inputs[2] = {0, 0};

            for (int p = 0; p < input_count; p++)
            {
                int key = (0 << 16) | (gate.id << 8) | p;
                auto it = wire_targets.find(key);
                if (it != wire_targets.end())
                {
                    inputs[p] = get_source_value(it->second.type, it->second.id);
                }
            }

            int new_output;
            if (gate.type == GateType::NOT)
            {
                new_output = EvaluateGate(gate.type, inputs[0], 0);
            }
            else
            {
                new_output = EvaluateGate(gate.type, inputs[0], inputs[1]);
            }

            if (gate_outputs[gate.id] != new_output)
            {
                gate_outputs[gate.id] = new_output;
                changed = true;
            }
        }

        // Evaluate output bits
        for (int b = 0; b < 4; b++)
        {
            int key = (1 << 16) | (0 << 8) | b;
            auto it = wire_targets.find(key);
            int new_val = 0;
            if (it != wire_targets.end())
            {
                new_val = get_source_value(it->second.type, it->second.id);
            }
            if (output_bits[b] != new_val)
            {
                output_bits[b] = new_val;
                changed = true;
            }
        }

        if (!changed) break;
    }

    // Compute hex value
    return output_bits[0] + output_bits[1] * 2 +
           output_bits[2] * 4 + output_bits[3] * 8;
}
