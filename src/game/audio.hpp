#pragma once

enum class SfxType {
    PLACE_GATE,
    REMOVE_GATE,
    CONNECT_WIRE,
    DISCONNECT_WIRE,
    TOGGLE_INPUT,
    SOLVED,
    ERROR,
    COUNT
};

void InitAudio();
void PlaySfx(SfxType type);
void ShutdownAudio();
