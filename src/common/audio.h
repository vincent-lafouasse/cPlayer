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

#ifdef __cplusplus
}
#endif
