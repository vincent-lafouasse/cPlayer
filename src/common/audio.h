#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    float** data;
    uint32_t size;
    uint32_t nChannels;
    uint32_t sampleRate;
} AudioData;

AudioData audiodata_new(uint32_t size, uint32_t nChannels, uint32_t sampleRate);
void audiodata_destroy(AudioData* track);

#ifdef __cplusplus
}
#endif
