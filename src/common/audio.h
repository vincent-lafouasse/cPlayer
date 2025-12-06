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

static inline AudioDataResult AudioDataResult_Ok(AudioData track)
{
    return (AudioDataResult){.track = track, .err = err_Ok()};
}

static inline AudioDataResult AudioDataResult_Err(Error err)
{
    return (AudioDataResult){.err = err};
}

#ifdef __cplusplus
}
#endif
