#include <stdlib.h>

#include "Error64.h"
#include "FileReader.h"
#include "Reader/ReaderAdapters.h"
#include "audio.h"
#include "codec/decode.h"
#include "common/log.h"
#include "options/Options.h"
#include "play/play.h"

#define STREAM_BUFFER_SIZE 256

Error64 parseFlags(int ac, char** av, Options* out)
{
    const OptionsResult maybeOptions =
        parseOptions((const char**)av + 1, ac - 1);
    if (err_subCategory(maybeOptions.err) == EOpt_HelpRequested) {
        printHelp(av[0]);
        exit(0);
    }
    if (!err_isOk(maybeOptions.err)) {
        return maybeOptions.err;
    }

    logOptions(&maybeOptions.options);
    *out = maybeOptions.options;
    return err_Ok();
}

static Error64 loadAudioOrExit(const char* path, AudioData* out)
{
    logFn(LogLevel_Debug, "-----Reading file\t%s-----\n", path);
    FileReader fileReader = fr_open(path);
    if (!fr_isOpened(&fileReader)) {
        return err_Err(E64_Read, (uint16_t)ERd_OpenFailed);
    }
    logFn(LogLevel_Debug, "FileReader buffer size: %zu\n",
          FILE_READER_BUFFER_SIZE);

    Reader reader = reader_fromFileReader(&fileReader);

    AudioDataResult maybeTrack = decodeAudio(&reader);
    fr_close(&fileReader);  // this isn't needed anymore
    if (maybeTrack.err != NoError) {
        return maybeTrack.err;
    }

    *out = maybeTrack.track;
    return err_Ok();
}

ErrorLogCtx makeLogCtx(int ac, char** av, const Options* opts)
{
    return (ErrorLogCtx){
        .argc = ac, .argv = (const char* const*)av, .options = opts};
}

int main(int ac, char** av)
{
    Error64 err;

    Options options;
    err = parseFlags(ac, av, &options);
    if (!err_isOk(err)) {
        ErrorLogCtx ctx = makeLogCtx(ac, av, NULL);
        logError(err, &ctx);
        return 1;
    }

    AudioData track;
    err = loadAudioOrExit(options.input, &track);
    if (!err_isOk(err)) {
        ErrorLogCtx ctx = makeLogCtx(ac, av, &options);
        logError(err, &ctx);
        return 1;
    }

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
