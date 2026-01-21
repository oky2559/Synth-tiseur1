#include <stdio.h>
#include <stdlib.h>
#include "oscillateurs.h"
#include "wav_helper.h"

int main(void)
{
  float freq, amplitude, duree, volume;
  int onde_type;
  //float freq, amplitude, duree, onde_type, volume;

  printf("Entrez la fréquence (Hz) : ");
  scanf("%f", &freq);

  printf("Entrez l'amplitude (0.0 à 1.0) : ");
  scanf("%f", &amplitude); 

  printf("Entrez la durée (secondes) : ");
  scanf("%f", &duree);
  
  printf("Entrez volume (0.0 a 100) : ");
  scanf("%f", &volume);

  int num_samples = (int)(SAMPLE_RATE * duree);
  FILE *f = fopen("output.wav", "wb");
  
  if (f == NULL) {
    printf("Erreur: impossible d'ouvrir le fichier\n");
    return 1;
  }

  printf("Entrez le type d'onde (1=sinusoïde, 2=carrée, 3=dents de scie, 4=triangulaire) : ");
  scanf("%d", &onde_type);

  // 1. Allouer le buffer en mémoire
  float *buffer = malloc(num_samples * sizeof(float));
  if (buffer == NULL)
    return 1;

  // 2. Remplir le buffer avec les calculs
  for (int i = 0; i < num_samples; i++)
  {
    float sample = sine_wave(freq, i);

    if (onde_type == 2)
      sample = square_wave(freq, i);
    else if (onde_type == 3)
      sample = sawtooth_wave(freq, i);
    else if (onde_type == 4)
      sample = triangle_wave(freq, i);

    buffer[i] = sample * amplitude * volume; // On stocke dans le buffer
  }

  // 3. Produire la sortie à partir du buffer
  write_wav_header(f, num_samples);
  for (int i = 0; i < num_samples; i++)
  {
    int16_t int_sample = (int16_t)(buffer[i] * 32767);
    fwrite(&int_sample, 2, 1, f);
  }

  free(buffer); // Ne pas oublier de libérer la mémoire

  fclose(f);
  printf("Fichier output.wav généré !\n");
  return 0;
}
