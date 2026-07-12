#pragma once
#include <raylib.h>
#include <string>

void InitGameAssets();
void UnloadGameAssets();

Font GetGameFont();
Shader GetBloomShader();
std::string GetResourcePath(const std::string& filename);
