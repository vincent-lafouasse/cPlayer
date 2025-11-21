#pragma once

#include <stdint.h>

// non owning
typedef struct {
    float* left;
    float* right;
    uint32_t size;
    uint32_t sampleRate;
} AudioData;
