#include <stdlib.h>

#include <portaudio.h>

#include "FileReader.h"

#include "log.h"
#include "wav.h"

typedef struct {
    const AudioData* data;
    size_t head;
} AudioPlayer;

AudioPlayer ap_init(const AudioData* data)
{
    return (AudioPlayer){.data = data, .head = 0};
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
    const float* left = player->data->left;
    const float* right = player->data->right;

    float* buffer = (float*)output;
    for (unsigned long _ = 0; _ < frameCount; ++_) {
        if (player->head < player->data->h.size) {
            *buffer++ = left[player->head];
            *buffer++ = right[player->head];
            player->head++;
        } else {
            *buffer++ = 0.0;
            *buffer++ = 0.0;
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

    if (Pa_Initialize() != paNoError) {
        logFn("Failed to init portaudio\n");
        exit(1);
    }

    if (Pa_Terminate() != paNoError) {
        logFn("Failed to terminate portaudio\n");
        exit(1);
    }

    PaStream* stream;
    Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, header.sampleRate,
                         STREAM_BUFFER_SIZE, callback, &player);
    Pa_StartStream(stream);
    Pa_Sleep(header.runtimeMs + 300);

    logFn("ok\n");
    fr_close(&reader);
}
