#pragma once

enum class GameState
{
    TITLE_SCREEN,
    TITLE_TO_PLAY_TRANSITION,
    TITLE_TO_HOW_TO_PLAY_TRANSITION,
    HOW_TO_PLAY,
    HOW_TO_PLAY_TO_PLAY_TRANSITION,
    HOW_TO_PLAY_TO_TITLE_TRANSITION,
    PLAYING
};

GameState UpdateTitleScreen(float anim_time);
GameState UpdateHowToPlay(float anim_time);
void DrawTitleScreen(float anim_time, float transition_time = 0.0f);
void DrawHowToPlay(float anim_time, float transition_time = 0.0f);
