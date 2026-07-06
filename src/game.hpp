#pragma once

#include <raylib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

constexpr int screenWidth = 720;
constexpr int screenHeight = 720;

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
    int frameCounter{};
    GameScreen currentScreen{GameScreen::Logo};
};
