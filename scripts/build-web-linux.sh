#!/usr/bin/env bash
set -euo pipefail

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build-web"
GAME_DIR="$BUILD_DIR/raylib-game-template"
GAME_HTML="$GAME_DIR/raylib-game-template.html"
PORT="${PORT:-3000}"
URL="http://localhost:${PORT}/raylib-game-template.html"
SERVER_PID=""

print_step()
{
    printf '%b[INFO]%b %s\n' "${CYAN}${BOLD}" "${NC}" "$1"
}

print_success()
{
    printf '%b[OK]%b %s\n' "${GREEN}${BOLD}" "${NC}" "$1"
}

print_warning()
{
    printf '%b[WARN]%b %s\n' "${YELLOW}${BOLD}" "${NC}" "$1"
}

print_error()
{
    printf '%b[ERROR]%b %s\n' "${RED}${BOLD}" "${NC}" "$1" >&2
}

open_browser()
{
    if command -v firefox >/dev/null 2>&1; then
        firefox --new-window "$URL" >/dev/null 2>&1 &
        return 0
    fi

    if command -v google-chrome >/dev/null 2>&1; then
        google-chrome --new-window "$URL" >/dev/null 2>&1 &
        return 0
    fi

    if command -v chromium >/dev/null 2>&1; then
        chromium --new-window "$URL" >/dev/null 2>&1 &
        return 0
    fi

    if command -v chromium-browser >/dev/null 2>&1; then
        chromium-browser --new-window "$URL" >/dev/null 2>&1 &
        return 0
    fi

    if command -v brave-browser >/dev/null 2>&1; then
        brave-browser --new-window "$URL" >/dev/null 2>&1 &
        return 0
    fi

    if command -v xdg-open >/dev/null 2>&1; then
        xdg-open "$URL" >/dev/null 2>&1 &
        return 0
    fi

    return 1
}

require_command()
{
    if ! command -v "$1" >/dev/null 2>&1; then
        print_error "Missing required command: $1"
        printf '%bInstall/activate it, then run this script again.%b\n' "${YELLOW}" "${NC}" >&2
        exit 1
    fi
}

cleanup()
{
    if [[ -n "$SERVER_PID" ]] && kill -0 "$SERVER_PID" >/dev/null 2>&1; then
        printf '\n'
        print_warning "Stopping local web server..."
        kill "$SERVER_PID" >/dev/null 2>&1 || true
    fi
}
trap cleanup EXIT INT TERM

print_step "Checking required tools"
require_command emcmake
require_command cmake
require_command ninja
require_command npx
print_success "Required build tools found"

print_step "Configuring WebAssembly build"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
emcmake cmake .. -G Ninja -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release
print_success "CMake configure complete"

print_step "Building with Ninja"
ninja
print_success "Web build complete"

if [[ ! -f "$GAME_HTML" ]]; then
    print_error "Expected HTML file not found: $GAME_HTML"
    exit 1
fi

print_step "Starting local server"
printf '%bServing:%b %s\n' "${BLUE}" "${NC}" "$GAME_DIR"
printf '%bURL:%b     %s\n' "${BLUE}" "${NC}" "$URL"

cd "$PROJECT_ROOT"
npx -y serve "$GAME_DIR" -l "$PORT" &
SERVER_PID="$!"

sleep 2

print_step "Opening browser"
if ! open_browser; then
    print_warning "No supported browser command was found. Open this URL manually: $URL"
fi

print_success "Server is running. Press Ctrl+C to stop it."
wait "$SERVER_PID"
