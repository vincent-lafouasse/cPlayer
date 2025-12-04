#pragma once

#include <stddef.h>
#include <stdint.h>

#define FIXED_BUFFER_SIZE 4096

typedef struct {
    uint8_t buffer[FIXED_BUFFER_SIZE];
    size_t head;
} MemoryReader;
