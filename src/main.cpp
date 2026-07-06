#include "game/game.hpp"

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

    InitWindow(screen_width, screen_height, "Our raylib gamejam template");

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
