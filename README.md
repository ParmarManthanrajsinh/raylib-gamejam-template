## $(Game Title)

![$(Game Title)](screenshots/screenshot000.png "$(Game Title)")

### Description

$(Your Game Description)

### Features

- $(Game Feature 01)
- $(Game Feature 02)
- $(Game Feature 03)

### Controls

Keyboard:
- $(Game Control 01)
- $(Game Control 02)
- $(Game Control 03)

### Build & Run

#### CMake (all platforms, recommended)

Raylib is fetched automatically by CMake — no manual install needed.

```sh
cmake -B build
cmake --build build
./build/raylib-game-template/raylib-game-template
```

#### WebAssembly (Emscripten)

Requires [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html) installed.

```powershell
# Activate Emscripten environment
C:\raylib\emsdk\emsdk_env.ps1

# Configure with emcmake wrapper
mkdir -Force build-web
cd build-web
emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build .
```

Output files in `build-web/raylib-game-template/`:
- `raylib-game-template.html` — open in browser, or serve locally
- `raylib-game-template.wasm` — WebAssembly binary
- `raylib-game-template.js` — Emscripten loader
- `raylib-game-template.data` — game resources

To serve locally:

```powershell
npx serve build-web/raylib-game-template
```

Then open `http://localhost:3000/raylib-game-template.html`.

#### Visual Studio 2022 (Windows)

Open `projects/VS2022/raylib_game.sln`, set `raylib_game` as startup project, and press **Local Windows Debugger**.

### Screenshots

_TODO: Show your game to the world, animated GIFs recommended!._

### Developers

- Manthan - $(Role/Tasks Developed)
- Dhaval - $(Role/Tasks Developed)
- Nagendra - $(Role/Tasks Developed)
- Nandana - $(Role/Tasks Developed)

### Links

- YouTube Gameplay: $(YouTube Link)
- itch.io Release: $(itch.io Game Page)
- Steam Release: $(Steam Game Page)

### License

This project sources are licensed under an unmodified zlib/libpng license, which is an OSI-certified, BSD-like license that allows static linking with closed source software. Check [LICENSE](LICENSE) for further details.

$(Additional Licenses)

*Copyright (c) $(Year) $(User Name) ($(User Twitter/GitHub Name))*
