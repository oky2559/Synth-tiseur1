#include <stdio.h>
#include <stdlib.h>
#include "oscillateurs.h"
#include "wav_helper.h"

int main(void) {
    float freq, amplitude, duree, onde_type;
    
    printf("Entrez la fréquence (Hz) : ");
    scanf("%f", &freq);
    
    printf("Entrez l'amplitude (0.0 à 1.0) : ");
    scanf("%f", &amplitude);
    
    printf("Entrez la durée (secondes) : ");
    scanf("%f", &duree);
    
    int num_samples = (int)(SAMPLE_RATE * duree);
    FILE *f = fopen("output.wav", "wb");
    
    if (!f) return 1;

    write_wav_header(f, num_samples);

    printf("Entrez le type d'onde (1=sinusoïde, 2=carrée, 3=dents de scie, 4=triangulaire) : ");
    scanf("%f", &onde_type);

    for (int i = 0; i < num_samples; i++) {
        // 1. Génération de l'onde
        float sample = sine_wave(freq, i); 
        
        // 2. Application du type d'onde
        if (onde_type == 2) {
            sample = square_wave(freq, i);
        } else if (onde_type == 3) {
            sample = sawtooth_wave(freq, i);
        } else if (onde_type == 4) {
            sample = triangle_wave(freq, i);
        }
        // 2. Application de l'amplitude (Volume)
        sample *= amplitude;
        
        // 3. Conversion Float -> Int16 pour le fichier
        int16_t int_sample = (int16_t)(sample * 32767);
        fwrite(&int_sample, 2, 1, f);
    }

    fclose(f);
    printf("Fichier output.wav généré !\n");
    return 0;
}
