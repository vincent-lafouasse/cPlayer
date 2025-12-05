#pragma once

#include <stdint.h>

#include "Error64.h"

typedef struct {
    float* left;
    float* right;
    uint32_t size;
    uint32_t sampleRate;
} AudioData;

typedef struct {
    AudioData track;
    Error64 err;
    const char* fault;
} AudioDataResult;

static inline AudioDataResult AudioDataResult_Ok(AudioData track)
{
    return (AudioDataResult){.track = track, .err = err_Ok()};
}

static inline AudioDataResult AudioDataResult_Err(Error64 err)
{
    return (AudioDataResult){.err = err};
}
