#pragma once

#include <stdint.h>

typedef struct {
    float* left;
    float* right;
    uint32_t size;
    uint32_t sampleRate;
} AudioData;
