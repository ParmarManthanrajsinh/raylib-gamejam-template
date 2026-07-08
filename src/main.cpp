#include "game/game.h"
#include "game/hex_grid.h"
#include "game/audio.h"

static void UpdateDrawFrame(Game *game)
{
    game->Update();
    game->Draw();
}

#if defined(PLATFORM_WEB)
static void UpdateDrawFrameWeb(void *arg)
{
    UpdateDrawFrame(static_cast<Game *>(arg));
}
#endif

int main()
{
#if !defined(_DEBUG)
    SetTraceLogLevel(LOG_NONE);
#endif

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hex Merge");
    InitAudio();
    Game game;

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop_arg(UpdateDrawFrameWeb, &game, 60, 1);
#else

    SetTargetFPS(60);
    while (!game.ShouldClose()) UpdateDrawFrame(&game);
#endif
    ShutdownAudio();
    CloseWindow();
    return 0;
}
