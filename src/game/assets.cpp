#include "assets.h"
#include <string>

namespace
{
    Font game_font;
    Shader bloom_shader;
    std::string resource_prefix;
}

static void InitResourcePrefix()
{
#if defined(PLATFORM_WEB)
    resource_prefix = "resources/";
#else
    resource_prefix = GetApplicationDirectory();
    resource_prefix += "resources/";

    if (!FileExists((resource_prefix + "Ranade-Variable.ttf").c_str()))
    {
        resource_prefix = "resources/";
    }
#endif
}

std::string GetResourcePath(const std::string &filename)
{
    if (resource_prefix.empty()) { InitResourcePrefix(); }
    return resource_prefix + filename;
}

void InitGameAssets()
{
    InitResourcePrefix();

    game_font = LoadFontEx(GetResourcePath("Ranade-Variable.ttf").data(), 64, 0, 250);
    bloom_shader = LoadShader(0, GetResourcePath("bloom.fs").data());
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
