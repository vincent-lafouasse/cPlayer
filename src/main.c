#include <stdlib.h>

#include "FileReader.h"

#include "common/log.h"
#include "play/play.h"
#include "wav/wav.h"

#define STREAM_BUFFER_SIZE 256

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

    AudioPlayer track = (AudioPlayer){
        .left = audio.left, .right = audio.right, .head = 0, .len = audio.size};

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
                  stereoOutputCallback, &track);

    Pa_StartStream(stream);

    while (Pa_IsStreamActive(stream) == 1)
        Pa_Sleep(20);

    Pa_CloseStream(stream);
    Pa_Terminate();

    logFn(Debug, "ok\n");
}
