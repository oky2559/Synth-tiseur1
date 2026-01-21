#include <stdio.h>
#include <stdlib.h>
#include "oscillateurs.h"
#include "wav_helper.h"

/* AJOUT */
#include "synth_chain.h"

int main(void)
{
  float freq, amplitude, duree;
  int onde_type;

  /* AJOUT */
  int volume_percent;
  int env_type; /* 0=none, 1=AR, 2=ADSR */
  float attack_s = 0.0f, decay_s = 0.0f, sustain_level = 1.0f, release_s = 0.0f;

  printf("Entrez la fréquence (Hz) : ");
  scanf("%f", &freq);

  printf("Entrez l'amplitude (0.0 à 1.0) : ");
  scanf("%f", &amplitude);

  printf("Entrez la durée (secondes) : ");
  scanf("%f", &duree);

  /* AJOUT : volume logiciel */
  printf("Entrez le volume (0 à 100) : ");
  scanf("%d", &volume_percent);
  float volume_gain = volume_to_gain(volume_percent);

  int num_samples = (int)(SAMPLE_RATE * duree);
  FILE *f = fopen("output.wav", "wb");

  if (f == NULL)
  {
    printf("Erreur: impossible d'ouvrir le fichier\n");
    return 1;
  }

  printf("Entrez le type d'onde (1=sinusoïde, 2=carrée, 3=dents de scie, 4=triangulaire) : ");
  scanf("%d", &onde_type);

  /* AJOUT : choix enveloppe */
  printf("Choisir l'enveloppe (0=aucune, 1=Attack/Release, 2=ADSR) : ");
  scanf("%d", &env_type);

  if (env_type == 1)
  {
    printf("Attack (secondes) : ");
    scanf("%f", &attack_s);
    printf("Release (secondes) : ");
    scanf("%f", &release_s);
  }
  else if (env_type == 2)
  {
    printf("Attack (secondes) : ");
    scanf("%f", &attack_s);
    printf("Decay (secondes) : ");
    scanf("%f", &decay_s);
    printf("Sustain level (0.0 à 1.0) : ");
    scanf("%f", &sustain_level);
    printf("Release (secondes) : ");
    scanf("%f", &release_s);
  }

  // 1. Allouer le buffer en mémoire
  float *buffer = malloc(num_samples * sizeof(float));
  if (buffer == NULL)
    return 1;

  // 2. Remplir le buffer avec les calculs + enveloppe + volume
  for (int i = 0; i < num_samples; i++)
  {
    float sample = sine_wave(freq, i);

    if (onde_type == 2)
      sample = square_wave(freq, i);
    else if (onde_type == 3)
      sample = sawtooth_wave(freq, i);
    else if (onde_type == 4)
      sample = triangle_wave(freq, i);

    /* AJOUT : enveloppe */
    float env = 1.0f;
    if (env_type == 1)
    {
      env = envelope_ar(i, num_samples, SAMPLE_RATE, attack_s, release_s);
    }
    else if (env_type == 2)
    {
      env = envelope_adsr(i, num_samples, SAMPLE_RATE, attack_s, decay_s, sustain_level, release_s);
    }

    /* AJOUT : contrôle du volume (gain) */
    buffer[i] = sample * amplitude * env * volume_gain;
  }

  // 3. Produire la sortie à partir du buffer
  write_wav_header(f, num_samples);
  for (int i = 0; i < num_samples; i++)
  {
    float s = clampf(buffer[i], -1.0f, 1.0f); /* AJOUT : sécurité */
    int16_t int_sample = (int16_t)(s * 32767);
    fwrite(&int_sample, 2, 1, f);
  }

  free(buffer); // Ne pas oublier de libérer la mémoire

  fclose(f);
  printf("Fichier output.wav généré !\n");
  return 0;
}
