#include <stdlib.h>

#include <portaudio.h>

#include "FileReader.h"

#include "common/log.h"
#include "play/play.h"
#include "wav/wav.h"

AudioPlayer ap_init(const AudioData* data)
{
    return (AudioPlayer){
        .left = data->left, .right = data->right, .head = 0, .len = data->size};
}

#define STREAM_BUFFER_SIZE 256

int callback(const void* input,
             void* output,
             unsigned long frameCount,
             const PaStreamCallbackTimeInfo* timeInfo,
             PaStreamCallbackFlags statusFlags,
             void* userData)
{
    (void)input;
    (void)timeInfo;
    (void)statusFlags;

    AudioPlayer* player = (AudioPlayer*)userData;

    float* buffer = (float*)output;
    for (unsigned long i = 0; i < frameCount; ++i) {
        if (player->head < player->len) {
            buffer[2 * i] = player->left[player->head];
            buffer[2 * i + 1] = player->right[player->head];
            player->head += 1;
        } else {
            buffer[2 * i] = 0.0f;
            buffer[2 * i + 1] = 0.0f;
        }
    }

    return 0;
}

int main(int ac, char** av)
{
    if (ac != 2) {
        logFn(Error, "Usage: %s track.wav\n", av[0]);
        exit(1);
    }
    const char* path = av[1];
    logFn(Debug, "-----Reading file\t%s-----\n", path);

    FileReader reader = fr_open(path);
    if (!fr_isOpened(&reader)) {
        logFn(Error, "Failed to open file %s\n", path);
        exit(1);
    }

    const AudioData audio = decodeWav(&reader);
    fr_close(&reader);

    AudioPlayer track = ap_init(&audio);

    Pa_Initialize();

    const PaDeviceIndex device = Pa_GetDefaultOutputDevice();
    if (device == paNoDevice) {
        logFn(Error, "No output device.\n");
        exit(1);
    }

    const PaStreamParameters outParams = (PaStreamParameters){
        .device = device,
        .channelCount = 2,
        .sampleFormat = paFloat32,
        .suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowOutputLatency,
        .hostApiSpecificStreamInfo = NULL,
    };

    PaStream* stream;
    Pa_OpenStream(&stream, NULL, &outParams, audio.sampleRate, 256, paNoFlag,
                  callback, &track);

    Pa_StartStream(stream);

    while (Pa_IsStreamActive(stream) == 1)
        Pa_Sleep(20);

    Pa_CloseStream(stream);
    Pa_Terminate();

    logFn(Debug, "ok\n");
}
