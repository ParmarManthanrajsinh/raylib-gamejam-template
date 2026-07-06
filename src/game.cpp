#include "game.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

Game::Game()
{
    target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);
}

Game::~Game()
{
    UnloadRenderTexture(target);
}

void Game::Update()
{
    frameCounter++;
}

void Game::Draw()
{
    BeginTextureMode(target);
    ClearBackground(RAYWHITE);

    DrawRectangle(70, 90, 200, 200, BLACK);
    DrawRectangle(70 + 16, 90 + 16, 200 - 32, 200 - 32, RAYWHITE);
    DrawText("raylib", 70 + 200 - MeasureText("raylib", 40) - 32,
             90 + 200 - 40 - 24, 40, BLACK);

    DrawText("6.x", 290, 90 - 26, 280, BLACK);
    DrawText("GAMEJAM", 70, 90 + 210, 120, MAROON);

    if ((frameCounter / 20) % 2)
        DrawText("are you ready?", 160, 500, 50, BLACK);

    DrawRectangleLinesEx({0, 0, (float)screenWidth, (float)screenHeight}, 16, BLACK);

    EndTextureMode();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawTexturePro(target.texture,
                   {0, 0, (float)target.texture.width, (float)-target.texture.height},
                   {0, 0, (float)target.texture.width, (float)target.texture.height},
                   {0, 0}, 0.0f, WHITE);

    EndDrawing();
}

bool Game::ShouldClose() const
{
    return WindowShouldClose();
}
