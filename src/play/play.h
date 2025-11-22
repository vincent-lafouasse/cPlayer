#pragma once

// non owning view
typedef struct {
    const float* left;
    const float* right;
    size_t head;
    size_t len;
} AudioPlayer;
