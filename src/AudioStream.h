#pragma once

#include <stdint.h>

#include <portaudio.h>

#include "log.h"  // TODO: this should not be here
/*

typedef struct {
    uint32_t nChannels;
    uint32_t sampleRate;
} StreamConfig;

typedef struct {
    StreamConfig cfg;
    PaStream* paStream;
} AudioStream;

void stream_initBackend(void)
{
    // TODO: return Error
    Pa_Initialize();
}

void stream_deinitBackend(void)
{
    // TODO: return Error
    Pa_Terminate();
}


AudioStream stream_loadStereo(StreamConfig cfg)
{
    // TODO: return a Result instead
    const PaDeviceIndex device = Pa_GetDefaultOutputDevice();
    if (device == paNoDevice) {
        logFn(LogLevel_Error, "No output device.\n");
        return 1;
    }

    const PaStreamParameters outParams = (PaStreamParameters){
        .device = device,
        .channelCount = cfg.nChannels,
        .sampleFormat = paFloat32,
        .suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowOutputLatency,
        .hostApiSpecificStreamInfo = NULL,
    };
}


void stream_close(AudioStream* stream)
{
    // TODO: return Error
    Pa_CloseStream(stream->paStream);
}

void stream_start(AudioStream* stream)
{
    Pa_StartStream(stream->stream);
}

void stream_stop(AudioStream* stream)
{
    Pa_CloseStream(stream->stream);
}
*/
