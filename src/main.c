#include <stdlib.h>

#include "Error.h"
#include "FileReader.h"

#include "audio.h"
#include "codec/decode.h"
#include "common/log.h"
#include "play/play.h"

#include "options/Options.h"

#define STREAM_BUFFER_SIZE 256

// also cleanup portaudio later
static void cleanup(FileReader* reader, AudioData* track)
{
    if (reader && fr_isOpened(reader)) {
        fr_close(reader);
    }

    if (track) {
        if (track->left == track->right) {
            free(track->left);
        } else {
            free(track->left);
            free(track->right);
        }
    }
}

int main(int ac, char** av)
{
    const OptionsResult maybeOptions =
        parseOptions((const char**)av + 1, ac - 1);
    if (maybeOptions.err == E_HelpRequested) {
        printHelp(av[0]);
        return 0;
    }
    if (maybeOptions.err != NoError) {
        logFn(LogLevel_Error, "Failed to parse flags: %s\n",
              errorRepr(maybeOptions.err));
        if (maybeOptions.fault) {
            logFn(LogLevel_Error, "Error at: %s\n", maybeOptions.fault);
        }
        logFn(LogLevel_Error, "Usage: %s track.wav\n", av[0]);
        return 1;
    }
    const Options* options = &maybeOptions.options;
    logOptions(options);

    const char* path = options->input;
    logFn(LogLevel_Debug, "-----Reading file\t%s-----\n", path);

    FileReader reader = fr_open(path);
    if (!fr_isOpened(&reader)) {
        logFn(LogLevel_Error, "Failed to open file %s\n", path);
        return 1;
    }
    logFn(LogLevel_Debug, "FileReader buffer size: %zu\n",
          FILE_READER_BUFFER_SIZE);

    AudioDataResult maybeTrack = decodeAudio(&reader);
    fr_close(&reader);
    if (maybeTrack.err != NoError) {
        logFn(LogLevel_Error, "%s\n", errorRepr(maybeTrack.err));
        return 1;
    }

    const AudioData track = maybeTrack.track;
    AudioPlayer player = (AudioPlayer){
        .left = track.left, .right = track.right, .head = 0, .len = track.size};

    Pa_Initialize();

    const PaDeviceIndex device = Pa_GetDefaultOutputDevice();
    if (device == paNoDevice) {
        logFn(LogLevel_Error, "No output device.\n");
        return 1;
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

    cleanup(&reader, (AudioData*)&track);
    logFn(LogLevel_Debug, "ok\n");
}
