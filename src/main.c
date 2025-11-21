#include <stdlib.h>

#include <portaudio.h>

#include "FileReader.h"

#include "common/log.h"
#include "wav/wav.h"

typedef struct {
    const float* left;
    const float* right;
    size_t head;
    size_t len;
} AudioPlayer;

AudioPlayer ap_init(const AudioData* data)
{
    return (AudioPlayer){.left = data->left,
                         .right = data->right,
                         .head = 0,
                         .len = data->h.size};
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
            buffer[2 * i] = 0.0;
            buffer[2 * i + 1] = 0.0;
        }
    }

    return 0;
}

int main(void)
{
    const char* path = "./wav/f1_24bit.wav";
    logFn("-----Reading file\t%s-----\n", path);
    FileReader reader = fr_open(path);
    if (!fr_isOpened(&reader)) {
        logFn("Failed to open file %s\n", path);
        exit(1);
    }

    Header header = readWavHeader(&reader);
    logHeader(&header, path);

    AudioData audio = readWavData(&reader, header);
    AudioPlayer player = ap_init(&audio);
    fr_close(&reader);

    Pa_Initialize();

    PaDeviceIndex device = Pa_GetDefaultOutputDevice();
    if (device == paNoDevice) {
        logFn("No output device.\n");
        exit(1);
    }

    PaStreamParameters outParams = (PaStreamParameters){
        .device = device,
        .channelCount = 2,
        .sampleFormat = paFloat32,
        .suggestedLatency = Pa_GetDeviceInfo(device)->defaultLowOutputLatency,
        .hostApiSpecificStreamInfo = NULL,
    };

    PaStream* stream;
    Pa_OpenStream(&stream, NULL, &outParams, header.sampleRate, 256, paNoFlag,
                  callback, &player);

    Pa_StartStream(stream);

    /* ----------- BLOCK UNTIL DONE ----------- */
    while (Pa_IsStreamActive(stream) == 1)
        Pa_Sleep(20);

    Pa_CloseStream(stream);
    Pa_Terminate();

    logFn("ok\n");
}
