#include "wav_internals.h"

#include <stdlib.h>

#include "Error.h"
#include "log.h"

static Error readMonoPCM(Reader* reader,
                         const WavFormatInfo* format,
                         AudioBuffer* out);
static Error readStereoPCM(Reader* reader,
                           const WavFormatInfo* format,
                           AudioBuffer* out);

Error readWavData(Reader* reader, const WavFormatInfo* format, AudioBuffer* out)
{
    if (format->formatTag != WAVE_FORMAT_PCM) {
        err_withCtx(E_Wav, EWav_UnsupportedSampleFormat, format->formatTag);
    }

    logFn(LogLevel_Debug,
          "start of the data _segment_, (data data not data chunk): %u\n",
          reader->offset);

    if (format->nChannels == 1) {
        return readMonoPCM(reader, format, out);
    } else if (format->nChannels == 2) {
        return readStereoPCM(reader, format, out);
    } else {
        return err_withCtx(E_Codec, ECdc_UnsupportedChannelLayout,
                           format->nChannels);
    }
}

static Error readMonoPCM(Reader* reader,
                         const WavFormatInfo* format,
                         AudioBuffer* out)
{
    Error err = err_Ok();

    AudioBuffer track = audiobuffer_new(format->nFrames, 1, format->sampleRate);
    if (track.data == NULL) {
        err = err_Err(E_System, ESys_OutOfMemory);
        goto out;
    }

    for (uint32_t i = 0; i < format->nFrames; ++i) {
        err = readSample(reader, format, track.data[0] + i);
        if (!err_isOk(err)) {
            goto out;
        }
    }
    dumpFloatCsv(track.data[0], format->nFrames,
                 DUMP_PREFIX "float" DUMP_SUFFIX);

out:
    if (!err_isOk(err)) {
        *out = track;
        return err_Ok();
    } else {
        audiobuffer_destroy(&track);
        return err;
    }
}

static Error readStereoFrame(Reader* reader,
                             const WavFormatInfo* format,
                             float* left,
                             float* right)
{
    TRY(readSample(reader, format, left));
    TRY(readSample(reader, format, right));
    return err_Ok();
}

static Error readStereoPCM(Reader* reader,
                           const WavFormatInfo* format,
                           AudioBuffer* out)
{
    Error err = err_Ok();

    AudioBuffer track = audiobuffer_new(format->nFrames, 2, format->sampleRate);
    if (track.data == NULL) {
        err = err_Err(E_System, ESys_OutOfMemory);
        goto out;
    }

    float* left = track.data[0];
    float* right = track.data[1];

    for (uint32_t i = 0; i < format->nFrames; ++i) {
        err = readStereoFrame(reader, format, left + i, right + i);
        if (!err_isOk(err)) {
            goto out;
        }
    }

out:
    if (!err_isOk(err)) {
        audiobuffer_destroy(&track);
        return err;
    } else {
        *out = track;
        return err_Ok();
    }
}
