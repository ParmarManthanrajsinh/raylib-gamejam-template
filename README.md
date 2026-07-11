## Hex Gate — Logic Gate Puzzle

![Hex Gate](screenshots/screenshot000.png "Hex Gate")

### Description

Build digital logic circuits on a hex grid to match target hex values. Connect input bits through gates (AND, OR, NOT, XOR, NAND, NOR, XNOR) and wire them to the output. Solve puzzles by constructing the correct logic network.

### Features

- Hex-based grid for gate placement and wiring
- 7 logic gate types with distinct visual shapes
- Drag-to-wire connection system between gates (Max 2 wires per output pin)
- Real-time circuit evaluation with animated signal propagation
- 4-bit input/output with hex value display
- Randomized fixed target values and input bits
- Minimum 3 gates required to solve puzzles
- Blocked middle node to increase routing difficulty
- Funny penalty ("Lazy Developer") for trying to direct wire input to output
- Retro pixel-art aesthetic with glow effects

### Controls

Keyboard: 
- `R` — Randomize target hex value and inputs
- `Esc` — Cancel selection / Back to Title

Mouse:
- Click a gate in the palette → click a hex cell to place it
- Click an output pin → click an input pin to create a wire
- Click an input pin that already has a wire → remove that wire
- Right-click a placed gate → delete it and its wires
- Click Clear button to wipe the board

### Play

Open `DemoPrototype.html` in any modern browser — no build step required.

### Screenshots

_TODO: Add gameplay screenshots or animated GIFs._

### Developers

- Manthan — Game Design & Logic
- Dhaval — Memory Management
- Nagendra — Circuit Evaluation
- Nandana — Visual Effects & Art

### Links

- YouTube Gameplay: $(YouTube Link)
- itch.io Release: $(itch.io Game Page)

### License

This project sources are licensed under an unmodified zlib/libpng license, which is an OSI-certified, BSD-like license that allows static linking with closed source software. Check [LICENSE](LICENSE) for further details.

*Copyright (c) 2025 Hex Gate Team*
