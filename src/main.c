#include <stdlib.h>

#include "FileReader.h"

#include "codec/decode.h"
#include "common/log.h"
#include "play/play.h"

#define STREAM_BUFFER_SIZE 256

int main(int ac, char** av)
{
    if (ac != 2) {
        logFn(LogLevel_Error, "Usage: %s track.wav\n", av[0]);
        exit(1);
    }
    const char* path = av[1];
    logFn(LogLevel_Debug, "-----Reading file\t%s-----\n", path);

    FileReader reader = fr_open(path);
    if (!fr_isOpened(&reader)) {
        logFn(LogLevel_Error, "Failed to open file %s\n", path);
        exit(1);
    }

    DecodingResult maybeTrack = decodeAudio(&reader);
    fr_close(&reader);
    if (maybeTrack.err != NoError) {
        logFn(LogLevel_Error, "Decoding went wrong\n");
        exit(1);
    }

    const AudioData track = maybeTrack.track;
    AudioPlayer player = (AudioPlayer){
        .left = track.left, .right = track.right, .head = 0, .len = track.size};

    Pa_Initialize();

    const PaDeviceIndex device = Pa_GetDefaultOutputDevice();
    if (device == paNoDevice) {
        logFn(LogLevel_Error, "No output device.\n");
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
    Pa_OpenStream(&stream, NULL, &outParams, track.sampleRate, 256, paNoFlag,
                  stereoOutputCallback, &player);

    Pa_StartStream(stream);

    while (Pa_IsStreamActive(stream) == 1)
        Pa_Sleep(20);

    Pa_CloseStream(stream);
    Pa_Terminate();

    logFn(LogLevel_Debug, "ok\n");
}
