#ifndef OSCILLATEURS_H
#define OSCILLATEURS_H

#include <math.h>
#include <stdint.h>

#define PI 3.14159265358979323846
#define SAMPLE_RATE 44100 // Fréquence d'échantillonnage

// Génère une onde sinusoïdale
float sine_wave(float freq, int i) {
    // Formule : sin(2 * PI * fréquence * temps)
    return sinf(2.0f * PI * freq * i / SAMPLE_RATE);
}

// Génère une onde carrée
float square_wave(float freq, int i) {
    // Si la sinusoïde est positive, on renvoie 1, sinon -1
    return (sinf(2.0f * PI * freq * i / SAMPLE_RATE) >= 0) ? 1.0f : -1.0f;
}

// Génère une onde en dents de scie
float sawtooth_wave(float freq, int i) {
    float period = (float)SAMPLE_RATE / freq;
    // On calcule la position dans le cycle (modulo) pour créer la rampe
    return 2.0f * (fmodf(i, period) / period) - 1.0f;
}

// Génère une onde triangulaire
float triangle_wave(float freq, int i) {
    float period = (float)SAMPLE_RATE / freq;
    float x = fmodf(i, period) / period;
    // On utilise la valeur absolue pour créer le triangle
    return 2.0f * fabsf(2.0f * x - 1.0f) - 1.0f;
}

#endif
