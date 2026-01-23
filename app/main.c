#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "oscillateurs.h"
#include "wav_helper.h"

#define FADE_DURATION 1.5f

/* Conversion volume (0–100) -> gain (0.0–1.0) */
static float volume_to_gain(int volume_percent)
{
  if (volume_percent < 0)
    volume_percent = 0;
  if (volume_percent > 100)
    volume_percent = 100;
  return (float)volume_percent / 100.0f;
}

/* Clamp sécurité */
static float clampf(float x, float lo, float hi)
{
  if (x < lo)
    return lo;
  if (x > hi)
    return hi;
  return x;
}

/*
 * Enveloppe simple : fade-in / fade-out
 * fade_s = durée du fondu (secondes)
 */
static float envelope_fade(int i, int total_samples, int sample_rate, float fade_s)
{
  int fade_samples = (int)(fade_s * sample_rate);

  if (fade_samples <= 0)
    return 1.0f;

  /* Fade-in */
  if (i < fade_samples)
  {
    return (float)i / (float)fade_samples;
  }

  /* Fade-out */
  if (i >= total_samples - fade_samples)
  {
    return (float)(total_samples - i) / (float)fade_samples;
  }

  /* Plateau */
  return 1.0f;
}

int main(void)
{
  float freq, amplitude, duree;
  int onde_type;
  int volume_percent;

  /* --- Entrées utilisateur --- */
  printf("Entrez la fréquence (Hz) : ");
  scanf("%f", &freq);

  printf("Entrez l'amplitude (0.0 à 1.0) : ");
  scanf("%f", &amplitude);

  printf("Entrez la durée (secondes) : ");
  scanf("%f", &duree);

  printf("Entrez le volume du WAV (0 à 100) : ");
  scanf("%d", &volume_percent);

  printf("Entrez le type d'onde (1=sinusoïde, 2=carrée, 3=dents de scie, 4=triangulaire) : ");
  scanf("%d", &onde_type);

  float volume_gain = volume_to_gain(volume_percent);

  int num_samples = (int)(SAMPLE_RATE * duree);

  FILE *f = fopen("output.wav", "wb");
  if (f == NULL)
  {
    printf("Erreur : impossible d'ouvrir le fichier output.wav\n");
    return 1;
  }

  float *buffer = malloc(num_samples * sizeof(float));
  if (buffer == NULL)
  {
    fclose(f);
    return 1;
  }

  /* --- Génération du signal : chaîne de synthèse --- */
  for (int i = 0; i < num_samples; i++)
  {
    /* 1) Oscillateur */
    float sample = sine_wave(freq, i);
    if (onde_type == 2)
      sample = square_wave(freq, i);
    else if (onde_type == 3)
      sample = sawtooth_wave(freq, i);
    else if (onde_type == 4)
      sample = triangle_wave(freq, i);

    /* 2) Enveloppe simple (fade-in / fade-out) */
    float env = envelope_fade(i, num_samples, SAMPLE_RATE, FADE_DURATION);

    /* 3) Contrôle du volume du WAV */
    buffer[i] = sample * amplitude * env * volume_gain;
  }

  /* --- Écriture du WAV --- */
  write_wav_header(f, num_samples);

  for (int i = 0; i < num_samples; i++)
  {
    float s = clampf(buffer[i], -1.0f, 1.0f);
    int16_t int_sample = (int16_t)(s * 32767.0f);
    fwrite(&int_sample, sizeof(int16_t), 1, f);
  }

  free(buffer);
  fclose(f);

  printf("Fichier output.wav généré !\n");
  return 0;
}
