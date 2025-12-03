#include <stdlib.h>

#include "Error.h"
#include "FileReader.h"
#include "Reader.h"
#include "audio.h"
#include "codec/decode.h"
#include "common/log.h"
#include "options/Options.h"
#include "play/play.h"

#define STREAM_BUFFER_SIZE 256

static Options parseFlagsOrExit(int ac, char** av)
{
    const OptionsResult maybeOptions =
        parseOptions((const char**)av + 1, ac - 1);
    if (maybeOptions.err == E_HelpRequested) {
        printHelp(av[0]);
        exit(0);
    }
    if (maybeOptions.err != NoError) {
        logFn(LogLevel_Error, "Failed to parse flags: %s\n",
              errorRepr(maybeOptions.err));
        if (maybeOptions.fault) {
            logFn(LogLevel_Error, "Error at: %s\n", maybeOptions.fault);
        }
        logFn(LogLevel_Error, "Usage: %s track.wav\n", av[0]);
        exit(1);
    }

    return maybeOptions.options;
}

static FileReader openFileOrExit(const char* path)
{
    FileReader reader = fr_open(path);
    if (!fr_isOpened(&reader)) {
        logFn(LogLevel_Error, "Failed to open file %s\n", path);
        exit(1);
    }
    logFn(LogLevel_Debug, "FileReader buffer size: %zu\n",
          FILE_READER_BUFFER_SIZE);

    return reader;
}

// the function takes ownership of the reader
static AudioData decodeAudioOrExit(FileReader* reader)
{
    AudioDataResult maybeTrack = decodeAudio(reader);
    fr_close(reader);  // this isn't needed anymore
    if (maybeTrack.err != NoError) {
        logFn(LogLevel_Error, "%s\n", errorRepr(maybeTrack.err));
        exit(1);
    }

    return maybeTrack.track;
}

int main(int ac, char** av)
{
    const Options options = parseFlagsOrExit(ac, av);
    logOptions(&options);

    logFn(LogLevel_Debug, "-----Reading file\t%s-----\n", options.input);
    FileReader reader = openFileOrExit(options.input);

    // decodeOrExit() takes ownership of the reader
    const AudioData track = decodeAudioOrExit(&reader);
    AudioPlayer player = (AudioPlayer){
        .left = track.left, .right = track.right, .head = 0, .len = track.size};

    // ----- start stream -----
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

    if (track.left != track.right) {
        free(track.right);
    }
    free(track.left);
    logFn(LogLevel_Debug, "ok\n");
}
