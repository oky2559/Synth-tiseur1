#ifndef WAV_HELPER_H
#define WAV_HELPER_H

#include <stdio.h>
#include <stdint.h>

void write_wav_header(FILE *f, int num_samples) {
    int32_t byte_rate = SAMPLE_RATE * 2; // 16 bits = 2 octets
    int32_t data_chunk_size = num_samples * 2;
    
    fwrite("RIFF", 1, 4, f);
    int32_t file_size = 36 + data_chunk_size;
    fwrite(&file_size, 4, 1, f);
    fwrite("WAVEfmt ", 1, 8, f);
    
    int32_t fmt_size = 16;
    int16_t audio_format = 1; // PCM
    int16_t num_channels = 1; // Mono
    int32_t sample_rate = SAMPLE_RATE;
    int16_t bits_per_sample = 16;
    int16_t block_align = 2;
    
    fwrite(&fmt_size, 4, 1, f);
    fwrite(&audio_format, 2, 1, f);
    fwrite(&num_channels, 2, 1, f);
    fwrite(&sample_rate, 4, 1, f);
    fwrite(&byte_rate, 4, 1, f);
    fwrite(&block_align, 2, 1, f);
    fwrite(&bits_per_sample, 2, 1, f);
    fwrite("data", 1, 4, f);
    fwrite(&data_chunk_size, 4, 1, f);
}

#endif
