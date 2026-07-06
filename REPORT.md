# raylib 6.x Game Jam — Report

## Event Overview

| Field | Detail |
|-------|--------|
| **Host** | Ray (Ramon Santamaria, @raysan5) |
| **Duration** | 6 days — Jul 6–12, 2026 |
| **Participants** | 576+ joined |
| **Hashtag** | `#raylibgamejam` (BlueSky) |
| **Celebration** | raylib 6.0 release, 16K+ Discord, 30K+ GitHub stars |

## Theme (Voted)

Two most voted themes from a Twitter/X poll:

| Theme | Votes |
|-------|-------|
| **hex** | **34.4%** ← winner |
| **merge** | **26.5%** ← runner-up |
| wonky | 21.1% |
| steps | 18.0% |

The game must incorporate both **hex** and **merge** in its design/objective.

---

## Hard Constraints (Mandatory)

1. **Resolution** — must be **720×720 pixels** (can render to smaller multiple like 360×360, 180×180 and scale up)
2. **WebAssembly** — game **must run on Web** via WASM (Emscripten)
3. **Size limit** — **under 64MB** total (`.wasm` + `.data`)

## Soft Constraints (Recommended)

- Use raylib template
- Keyboard/Mouse + Touch controls
- Open source on GitHub

## Rules

- itch.io account required to submit
- Teams of **up to 4 members**
- Assets must be royalty-free or self-created
- No NSFW/hateful/discriminatory content
- Must be created within the jam timeframe
- No late entries

## Rating System

**5-star scale** across 3 categories:

| Category | Description |
|----------|-------------|
| **Theme** | How well is the theme used? |
| **Fun** | Is it enjoyable to play? |
| **Polish** | Overall quality & time well spent |

**Two-round voting model:**
1. **Public votes** — any itch.io user plays the web build and rates
2. **Jury votes** — top-rated public entries evaluated by a jury (disclosed at jam end)

## Prizes

| Rank | Prize |
|------|-------|
| 1st–3rd place | Steam gift cards |
| **All qualifiers** | 1 free itch.io key for a raylibtech tool (rFXGen, etc.) |

## Key Dates

| Event | Date |
|-------|------|
| Submissions open | Jul 6, 18:00 |
| Submissions close | Jul 12, 18:00 |
| Rating period | Jul 12–18 (6 days) |

## Submission Requirements

- Upload to itch.io as an **HTML5/Web game**
- Only the **WASM build** is evaluated (desktop builds optional)
- Tag itch.io game with **raylib** under Engines & Tools
- Game cannot be updated during voting period

## Recommended Tools & Assets

| Category | Resources |
|----------|-----------|
| **Palettes** | Lospec Palette List |
| **Art assets** | Kenney Assets, Kay Lousberg, OpenGameArt |
| **Sprite editor** | Piskel |
| **Image editor** | Photopea |
| **Color tools** | Paletton, Colormind |
| **SFX** | rFXGen, ChipTone |
| **Music** | Bosca Ceoil, BeepBox, JummBox |

## FAQ Highlights

- **AI use** — Discouraged (defeats the purpose of a low-level coding challenge)
- **Smaller resolution** — OK, use RenderTexture scaling (360×360 recommended)
- **Pre-existing code/assets** — Only your own work allowed; spirit is to make something new
- **Updates during voting** — Not allowed

## Team Checklist

| Task | Status |
|------|--------|
| C++ template ready (game/ split) | ✅ |
| CMake desktop build | ✅ |
| WebAssembly build | ✅ |
| Keyboard/Mouse controls | ⬜ |
| Touch controls | ⬜ |
| Theme integration (hex + merge) | ⬜ |
| itch.io page & upload | ⬜ |
| GitHub Actions CI | ✅ |
