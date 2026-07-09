#include "audio.h"
#include <cmath>
#include <cstdlib>
#include <raylib.h>

namespace
{
    constexpr int SAMPLE_RATE = 44100;
    Sound sounds[static_cast<int>(SfxType::COUNT)];
    Sound GenerateSfx
    (
        float duration,
        float freq_start,
        float freq_end,
        float amp,
        int wave_type
    )
    {
        int n = static_cast<int>(SAMPLE_RATE * duration);

        Wave wave{};
        wave.data = malloc(n * sizeof(short));
        wave.frameCount = n;
        wave.sampleRate = SAMPLE_RATE;
        wave.sampleSize = 16;
        wave.channels = 1;
        short* samples = static_cast<short*>(wave.data);

        for (int i = 0; i < n; i++)
        {
            float t = static_cast<float>(i) / SAMPLE_RATE;
            float frac = static_cast<float>(i) / n;
            float freq = freq_start + (freq_end - freq_start) * frac;
            float env = 1.0f - frac;

            float val;
            if (wave_type == 1)
            {
                val = (sinf(2 * PI * freq * t) >= 0) ? 1.0f : -1.0f;
            }
            else if (wave_type == 2)
            {
                val = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f;
            }
            else
            {
                val = sinf(2 * PI * freq * t);
            }

            samples[i] = static_cast<short>(val * env * amp * 30000);
        }

        Sound sound = LoadSoundFromWave(wave);
        UnloadWave(wave);
        return sound;
    }

    Sound GenerateArpeggio()
    {
        float notes[] = {400.0f, 600.0f, 900.0f};
        float note_dur = 0.08f;
        float gap = 0.02f;
        int note_samples = static_cast<int>((SAMPLE_RATE * note_dur));
        int gap_samples = static_cast<int>((SAMPLE_RATE * gap));
        int total = static_cast<int>((3 * (note_samples + gap_samples)));

        Wave wave{};
        wave.data = malloc(total * sizeof(short));
        wave.frameCount = total;
        wave.sampleRate = SAMPLE_RATE;
        wave.sampleSize = 16;
        wave.channels = 1;
        short* samples = static_cast<short*>(wave.data);
        int cursor = 0;

        for (int n = 0; n < 3; n++)
        {
            for (int i = 0; i < note_samples; i++)
            {
                float t = static_cast<float>(i) / SAMPLE_RATE;
                float frac = static_cast<float>(i) / note_samples;
                float env = 1.0f - frac * 0.5f;
                float val = sinf(2 * PI * notes[n] * t);
                samples[cursor++] = static_cast<short>(val * env * 30000);
            }
            for (int i = 0; i < gap_samples; i++)
            {
                samples[cursor++] = 0;
            }
        }

        Sound sound = LoadSoundFromWave(wave);
        UnloadWave(wave);
        return sound;
    }
}

void InitAudio()
{
    InitAudioDevice();
    sounds[static_cast<int>(SfxType::PLACE_GATE)] =
        GenerateSfx(0.06f, 600, 200, 1.0f, 0);
    sounds[static_cast<int>(SfxType::REMOVE_GATE)] =
        GenerateSfx(0.05f, 100, 100, 0.4f, 2);
    sounds[static_cast<int>(SfxType::CONNECT_WIRE)] =
        GenerateSfx(0.05f, 800, 800, 0.8f, 1);
    sounds[static_cast<int>(SfxType::DISCONNECT_WIRE)] =
        GenerateSfx(0.04f, 600, 200, 0.6f, 0);
    sounds[static_cast<int>(SfxType::TOGGLE_INPUT)] =
        GenerateSfx(0.03f, 1000, 1000, 0.7f, 0);
    sounds[static_cast<int>(SfxType::SOLVED)] =
        GenerateArpeggio();
    sounds[static_cast<int>(SfxType::ERROR)] =
        GenerateSfx(0.08f, 150, 150, 0.5f, 1);
}

void PlaySfx(SfxType type)
{
    float pitch = 0.9f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 0.2f;
    SetSoundPitch(sounds[static_cast<int>(type)], pitch);
    PlaySound(sounds[static_cast<int>(type)]);
}

void ShutdownAudio()
{
    for (int i = 0; i < static_cast<int>(SfxType::COUNT); i++)  UnloadSound(sounds[i]);
    CloseAudioDevice();
}
