#pragma once

enum class GameState
{
    TITLE_SCREEN,
    TITLE_TO_PLAY_TRANSITION,
    TITLE_TO_HOW_TO_PLAY_TRANSITION,
    HOW_TO_PLAY,
    HOW_TO_PLAY_TO_PLAY_TRANSITION,
    HOW_TO_PLAY_TO_TITLE_TRANSITION,
    PLAYING,
    PLAYING_TO_TITLE_TRANSITION,
    PLAYING_TO_LEVEL_COMPLETE_TRANSITION,
    LEVEL_COMPLETE,
    LEVEL_COMPLETE_TO_PLAY_TRANSITION,
    LEVEL_COMPLETE_TO_TITLE_TRANSITION
};

struct GameStats
{
    int gates_used;
    int wires_used;
    float time_taken;
    int efficiency_score;
};

GameState UpdateTitleScreen(float anim_time);
GameState UpdateHowToPlay(float anim_time);
GameState UpdateLevelComplete(float anim_time, const GameStats& stats);
void DrawTitleScreen(float anim_time, float transition_time = 0.0f);
void DrawHowToPlay(float anim_time, float transition_time = 0.0f);
void DrawLevelComplete
(
    float anim_time,
    float transition_time,
    const GameStats& stats,
    GameState game_state
);
