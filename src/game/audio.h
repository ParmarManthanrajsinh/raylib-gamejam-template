#pragma once
#include <raylib.h>

enum class SfxType
{
    PLACE_GATE,
    REMOVE_GATE,
    CONNECT_WIRE,
    DISCONNECT_WIRE,
    TOGGLE_INPUT,
    SOLVED,
    ERROR,
    ROBOT_TALK,
    ROBOT_BOOP,
    COUNT
};

void InitAudio();
void PlaySfx(SfxType type);
void UpdateAudio();
void SetMusicPlaying(bool playing);
bool IsMusicPlaying();
void ShutdownAudio();
