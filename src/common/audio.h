#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "Error.h"

typedef struct {
    float* left;
    float* right;
    uint32_t size;
    uint32_t sampleRate;
} AudioData;

typedef struct {
    AudioData track;
    Error err;
    const char* fault;
} AudioDataResult;

AudioDataResult AudioDataResult_Ok(AudioData track);
AudioDataResult AudioDataResult_Err(Error err);

#ifdef __cplusplus
}
#endif
