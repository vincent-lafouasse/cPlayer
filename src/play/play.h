#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include <portaudio.h>

// non owning view
typedef struct {
    const float* left;
    const float* right;
    size_t head;
    size_t len;
} AudioPlayer;

typedef enum {
    Audio_Mono,
    Audio_Stereo,
} StreamChannelLayout;

typedef struct {
    StreamChannelLayout layout;
    double sampleRate;
} StreamConfig;

int stereoOutputCallback(const void* input,
                         void* output,
                         unsigned long frameCount,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void* userData);

int monoOutputCallback(const void* input,
                       void* output,
                       unsigned long frameCount,
                       const PaStreamCallbackTimeInfo* timeInfo,
                       PaStreamCallbackFlags statusFlags,
                       void* userData);

#ifdef __cplusplus
}
#endif
