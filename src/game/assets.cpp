#include "assets.h"

namespace
{
    Font game_font;
    Shader bloom_shader;
}

void InitGameAssets()
{
    game_font = LoadFontEx("resources/Ranade-Variable.ttf", 64, 0, 250);
    bloom_shader = LoadShader(0, "resources/bloom.fs");
}

void UnloadGameAssets()
{
    UnloadFont(game_font);
    UnloadShader(bloom_shader);
}

Font GetGameFont()
{
    return game_font;
}

Shader GetBloomShader()
{
    return bloom_shader;
}
