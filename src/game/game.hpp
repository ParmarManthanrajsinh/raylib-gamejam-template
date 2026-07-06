#pragma once

#include <raylib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

constexpr int screen_width = 720;
constexpr int screen_height = 720;

enum class GameScreen 
{
    Logo,
    Title,
    Gameplay,
    Ending
};

class Game 
{
public:
    Game();
    ~Game();

    void Update();
    void Draw();
    bool ShouldClose() const;

private:
    RenderTexture2D target{};
    int frame_counter{};
    GameScreen current_screen{GameScreen::Logo};
};
