#include "game/game.hpp"
#include "game/hex_grid.hpp"

static Game *GamePtr = nullptr;

static void UpdateDrawFrame()
{
    if (GamePtr)
    {
        GamePtr->Update();
        GamePtr->Draw();
    }
}

int main()
{
#if !defined(_DEBUG)
    SetTraceLogLevel(LOG_NONE);
#endif

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hex Merge");

    Game game;
    GamePtr = &game;

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);

    while (!game.ShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    GamePtr = nullptr;

    CloseWindow();

    return 0;
}
