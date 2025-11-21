#include <stdlib.h>

#include <portaudio.h>

#include "FileReader.h"

#include "log.h"
#include "wav.h"

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

    Pa_Initialize();

    PaStream* stream;
    PaStreamParameters outParams;

    outParams.device = Pa_GetDefaultOutputDevice();
    if (outParams.device == paNoDevice) {
        logFn("No output device.\n");
        exit(1);
    }

    outParams.channelCount = 2;          // stereo
    outParams.sampleFormat = paFloat32;  // float samples
    outParams.suggestedLatency =
        Pa_GetDeviceInfo(outParams.device)->defaultLowOutputLatency;
    outParams.hostApiSpecificStreamInfo = NULL;

    Pa_OpenStream(&stream,
                  NULL,               // no input
                  &outParams,         // output
                  header.sampleRate,  // sample rate
                  256,                // frames per buffer (suggested)
                  paNoFlag, callback, &player);

    Pa_StartStream(stream);

    /* ----------- BLOCK UNTIL DONE ----------- */
    while (Pa_IsStreamActive(stream) == 1)
        Pa_Sleep(20);

    Pa_CloseStream(stream);

    Pa_Terminate();

    fr_close(&reader);
    logFn("ok\n");
}
