#include "audio.hpp"
#include <cmath>
#include <cstdlib>
#include <raylib.h>

#define SAMPLE_RATE 44100

static Sound sounds[(int)SfxType::COUNT];

static Sound GenerateSfx(float duration, float freqStart, float freqEnd, float amp, int waveType) {
    int n = (int)(SAMPLE_RATE * duration);

    Wave wave = { 0 };
    wave.data = malloc(n * sizeof(short));
    wave.frameCount = n;
    wave.sampleRate = SAMPLE_RATE;
    wave.sampleSize = 16;
    wave.channels = 1;
    short* samples = (short*)wave.data;

    for (int i = 0; i < n; i++) {
        float t = (float)i / SAMPLE_RATE;
        float frac = (float)i / n;
        float freq = freqStart + (freqEnd - freqStart) * frac;
        float env = 1.0f - frac;

        float val;
        if (waveType == 1) {
            val = (sinf(2 * PI * freq * t) >= 0) ? 1.0f : -1.0f;
        } else if (waveType == 2) {
            val = (float)rand() / RAND_MAX * 2.0f - 1.0f;
        } else {
            val = sinf(2 * PI * freq * t);
        }

        samples[i] = (short)(val * env * amp * 30000);
    }

    Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);
    return sound;
}

static Sound GenerateArpeggio() {
    float notes[] = { 400.0f, 600.0f, 900.0f };
    float noteDur = 0.08f;
    float gap = 0.02f;
    int noteSamples = (int)(SAMPLE_RATE * noteDur);
    int gapSamples = (int)(SAMPLE_RATE * gap);
    int total = (int)(3 * (noteSamples + gapSamples));

    Wave wave = { 0 };
    wave.data = malloc(total * sizeof(short));
    wave.frameCount = total;
    wave.sampleRate = SAMPLE_RATE;
    wave.sampleSize = 16;
    wave.channels = 1;
    short* samples = (short*)wave.data;
    int cursor = 0;

    for (int n = 0; n < 3; n++) {
        for (int i = 0; i < noteSamples; i++) {
            float t = (float)i / SAMPLE_RATE;
            float frac = (float)i / noteSamples;
            float env = 1.0f - frac * 0.5f;
            float val = sinf(2 * PI * notes[n] * t);
            samples[cursor++] = (short)(val * env * 30000);
        }
        for (int i = 0; i < gapSamples; i++) {
            samples[cursor++] = 0;
        }
    }

    Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);
    return sound;
}

void InitAudio() {
    InitAudioDevice();
    sounds[(int)SfxType::PLACE_GATE]     = GenerateSfx(0.06f, 600, 200, 1.0f, 0);
    sounds[(int)SfxType::REMOVE_GATE]    = GenerateSfx(0.05f, 100, 100, 0.4f, 2);
    sounds[(int)SfxType::CONNECT_WIRE]   = GenerateSfx(0.05f, 800, 800, 0.8f, 1);
    sounds[(int)SfxType::DISCONNECT_WIRE] = GenerateSfx(0.04f, 600, 200, 0.6f, 0);
    sounds[(int)SfxType::TOGGLE_INPUT]   = GenerateSfx(0.03f, 1000, 1000, 0.7f, 0);
    sounds[(int)SfxType::SOLVED]         = GenerateArpeggio();
    sounds[(int)SfxType::ERROR]          = GenerateSfx(0.08f, 150, 150, 0.5f, 1);
}

void PlaySfx(SfxType type) {
    PlaySound(sounds[(int)type]);
}

void ShutdownAudio() {
    for (int i = 0; i < (int)SfxType::COUNT; i++) {
        UnloadSound(sounds[i]);
    }
    CloseAudioDevice();
}
