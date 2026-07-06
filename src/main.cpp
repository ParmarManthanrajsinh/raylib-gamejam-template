#include "game.hpp"

static Game *gamePtr = nullptr;

static void UpdateDrawFrame()
{
    if (gamePtr)
    {
        gamePtr->Update();
        gamePtr->Draw();
    }
}

int main()
{
#if !defined(_DEBUG)
    SetTraceLogLevel(LOG_NONE);
#endif

    InitWindow(screenWidth, screenHeight, "Our raylib gamejam template");

    Game game;
    gamePtr = &game;

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);

    while (!game.ShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    gamePtr = nullptr;

    CloseWindow();

    return 0;
}
