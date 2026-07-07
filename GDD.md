# Game Design Document: Hex Merge

## 1. Overview

| Field | Value |
|---|---|
| **Title** | Hex Merge |
| **Genre** | Puzzle / Logic Simulator |
| **Platform** | Web (WASM), Windows, Linux, macOS |
| **Resolution** | 720×720 pixels (jam requirement) |
| **Target Size** | <5MB (.wasm + .data) |
| **Engine** | raylib 6.x (C++20) |
| **Jam** | raylib 6.x gamejam — themes: **hex** + **merge** |

## 2. Theme Integration

| Theme | How It's Used |
|---|---|
| **Hex** | Every puzzle has a target hexadecimal output. Players build circuits to produce it. |
| **Merge** | Logic gates merge multiple binary inputs into a single output through combinatory logic. |

## 3. Core Mechanics

### 3.1 Grid & Placement
- 8×8 snap grid for gate placement
- Input nodes on left edge (2/4/8 bits depending on puzzle)
- Output node on right edge
- Gates dragged from palette, snapped to grid

### 3.2 Gates Available

| Gate | Inputs | Output | Function |
|---|---|---|---|
| AND | 2 | 1 | Both inputs = 1 |
| OR | 2 | 1 | At least one input = 1 |
| NOT | 1 | 1 | Invert input |
| XOR | 2 | 1 | Inputs differ |
| NAND | 2 | 1 | NOT of AND |
| NOR | 2 | 1 | NOT of OR |
| XNOR | 2 | 1 | Inputs same |

### 3.3 Wiring
- Click output pin → click input pin to connect
- Right-click to delete wire
- Wire colors match signal state (red=1, blue=0)
- Auto-routing with orthogonal bends

### 3.4 Evaluation
- Topological sort of gates each frame
- Instant propagation on any change
- Output hex updates live

## 4. Game Flow

```
Title Screen → Level Select → Puzzle Gameplay → Score Screen → Level Select
                                   ↑                    |
                                   └────────────────────┘
```

### 4.1 Title Screen
- Animated hex logo
- "Play" and "Sandbox" buttons
- Sound toggle

### 4.2 Level Select
- 30+ puzzles in 3 tiers (Easy: 2-bit, Medium: 4-bit, Hard: 8-bit)
- Each shows puzzle #, target hex, star rating
- Locked until previous solved
- Sandbox entry

### 4.3 Puzzle Gameplay
- Left panel: input toggles (click to flip 0/1)
- Center: grid area for gate placement
- Right: live output hex + target hex display
- Bottom: gate palette
- "Clear" button to reset grid
- Win when output == target

### 4.4 Score Screen
- Star rating (1-3): gates used vs par
- Time taken
- "Retry" / "Next" / "Level Select" buttons

### 4.5 Sandbox Mode
- Unlimited grid (scrollable)
- All gates unlocked
- No target, no timer
- Export circuit as shareable string

## 5. Controls

| Action | Keyboard/Mouse | Touch |
|---|---|---|
| Place gate | Click palette → click grid | Tap palette → tap grid |
| Drag gate | Click + drag | Long press + drag |
| Create wire | Click output pin → click input pin | Tap output → tap input |
| Delete | Right-click gate/wire | Double-tap gate/wire |
| Toggle input | Click input node | Tap input node |
| Clear all | Ctrl+C / button | Button |
| Undo | Ctrl+Z | Button |

## 6. Visual Style

- **Palette:** Monochrome background with 2-3 accent colors (neon green/cyan on dark)
- **Gates:** Simple geometric shapes with labeled symbols
- **Wires:** Thin lines with animated pulse when active
- **UI:** Minimal, pixel font for hex values
- **Screenshots:** Clean grid, clear visual feedback

## 7. Audio

- **Sound effects:** ChipTone/rFXGen generated
  - Gate place/delete click
  - Wire connect pop
  - Input toggle beep
  - Puzzle solve jingle
  - Star award chime

## 8. Technical Constraints

| Requirement | Compliance |
|---|---|
| 720×720 resolution | RenderTexture at 720×720, scaled if needed |
| Web (WASM) | Emscripten build via Makefile/CMake |
| <64MB | Minimal assets, no large textures |
| Keyboard/Mouse + Touch | Dual input handling |
| Open source | MIT/zlib license |

## 9. Development Roadmap (6 days)

| Day | Focus |
|---|---|
| **1** | Grid rendering, gate placement, drag, delete |
| **2** | Logic evaluation engine, wiring system |
| **3** | UI screens (title, level select, score), save/load progress |
| **4** | 30 puzzle levels, difficulty tuning |
| **5** | Audio, polish, touch controls, sandbox mode |
| **6** | WASM testing, bug fixes, itch.io submission |

## 10. Scoring & Rating Fit

| Category | How Hex Merge Scores |
|---|---|
| **Theme** | 5/5 — hex and merge are the entire game |
| **Fun** | Satisfying puzzle-solving, clear feedback loop |
| **Polish** | Clean visuals, smooth wire animations, sound |
