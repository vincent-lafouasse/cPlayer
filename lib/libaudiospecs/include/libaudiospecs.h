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
} AudioBuffer;

AudioBuffer audiobuffer_new(uint32_t size,
                            uint32_t nChannels,
                            uint32_t sampleRate);
void audiobuffer_destroy(AudioBuffer* track);

#ifdef __cplusplus
}
#endif
