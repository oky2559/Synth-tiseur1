#ifndef SYNTH_CHAIN_H
#define SYNTH_CHAIN_H

#include <stdint.h>

/* Clamp simple pour éviter les dépassements avant conversion int16 */
static inline float clampf(float x, float lo, float hi)
{
    if (x < lo)
        return lo;
    if (x > hi)
        return hi;
    return x;
}

/* Volume 0..100 -> gain 0.0..1.0 */
static inline float volume_to_gain(int volume_percent)
{
    if (volume_percent < 0)
        volume_percent = 0;
    if (volume_percent > 100)
        volume_percent = 100;
    return (float)volume_percent / 100.0f;
}

/*
 * Enveloppe AR (Attack/Release) :
 * - attack_s : montée linéaire de 0 à 1
 * - release_s : descente linéaire vers 0 sur la fin du son
 */
static inline float envelope_ar(int i, int total_samples, int sample_rate,
                                float attack_s, float release_s)
{
    int attack_n = (int)(attack_s * sample_rate);
    int release_n = (int)(release_s * sample_rate);

    if (attack_n < 0)
        attack_n = 0;
    if (release_n < 0)
        release_n = 0;

    float g = 1.0f;

    /* Attack */
    if (attack_n > 0 && i < attack_n)
    {
        g = (float)i / (float)attack_n; /* 0 -> 1 */
    }

    /* Release (sur la fin) */
    if (release_n > 0)
    {
        int release_start = total_samples - release_n;
        if (release_start < 0)
            release_start = 0;
        if (i >= release_start)
        {
            int t = i - release_start;                        /* 0 -> release_n */
            float rel = 1.0f - ((float)t / (float)release_n); /* 1 -> 0 */
            g = (g < rel) ? g : rel;                          /* combine sans dépasser */
        }
    }

    return clampf(g, 0.0f, 1.0f);
}

/*
 * Enveloppe ADSR :
 * - attack_s : 0 -> 1
 * - decay_s : 1 -> sustain_level
 * - sustain_level : niveau 0..1
 * - release_s : sustain_level -> 0 sur la fin
 *
 * Remarque : la phase sustain "remplit" le temps restant.
 */
static inline float envelope_adsr(int i, int total_samples, int sample_rate,
                                  float attack_s, float decay_s,
                                  float sustain_level, float release_s)
{
    int a = (int)(attack_s * sample_rate);
    int d = (int)(decay_s * sample_rate);
    int r = (int)(release_s * sample_rate);

    if (a < 0)
        a = 0;
    if (d < 0)
        d = 0;
    if (r < 0)
        r = 0;

    sustain_level = clampf(sustain_level, 0.0f, 1.0f);

    int release_start = total_samples - r;
    if (release_start < 0)
        release_start = 0;

    /* Phase Release (prioritaire sur la fin) */
    if (r > 0 && i >= release_start)
    {
        int t = i - release_start; /* 0..r */
        float g = sustain_level * (1.0f - (float)t / (float)r);
        return clampf(g, 0.0f, 1.0f);
    }

    /* Attack */
    if (a > 0 && i < a)
    {
        float g = (float)i / (float)a;
        return clampf(g, 0.0f, 1.0f);
    }

    /* Decay */
    int decay_start = a;
    int decay_end = a + d;
    if (d > 0 && i >= decay_start && i < decay_end)
    {
        int t = i - decay_start; /* 0..d */
        float g = 1.0f + (sustain_level - 1.0f) * ((float)t / (float)d);
        return clampf(g, 0.0f, 1.0f);
    }

    /* Sustain */
    return sustain_level;
}

#endif
