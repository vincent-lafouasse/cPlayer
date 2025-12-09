#include "wav_internals.h"

#include <stdlib.h>

#include "Error.h"
#include "log.h"

static Error readMonoPCM(Reader* reader,
                         const WavFormatInfo* format,
                         AudioData* out);
static Error readStereoPCM(Reader* reader,
                           const WavFormatInfo* format,
                           AudioData* out);

Error readWavData(Reader* reader, const WavFormatInfo* format, AudioData* out)
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
                         AudioData* out)
{
    Error err = err_Ok();

    float* left = calloc(format->nFrames, sizeof(float));
    if (left == NULL) {
        err = err_Err(E_System, ESys_OutOfMemory);
        goto out;
    }

    for (uint32_t i = 0; i < format->nFrames; ++i) {
        err = readSample(reader, format, left + i);
        if (!err_isOk(err)) {
            goto out;
        }
    }
    dumpFloatCsv(left, format->nFrames, DUMP_PREFIX "float" DUMP_SUFFIX);

out:
    if (!err_isOk(err)) {
        *out = (AudioData){.left = left,
                           .right = left,
                           .size = format->nFrames,
                           .sampleRate = format->sampleRate};
        return err_Ok();
    } else {
        free(left);
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
                           AudioData* out)
{
    Error err = err_Ok();

    float* left = calloc(format->nFrames, sizeof(float));
    float* right = calloc(format->nFrames, sizeof(float));
    if (left == NULL || right == NULL) {
        err = err_Err(E_System, ESys_OutOfMemory);
        goto out;
    }

    for (uint32_t i = 0; i < format->nFrames; ++i) {
        err = readStereoFrame(reader, format, left + i, right + i);
        if (!err_isOk(err)) {
            goto out;
        }
    }

out:
    if (!err_isOk(err)) {
        free(left);
        free(right);
        return err;
    } else {
        *out = (AudioData){.left = left,
                           .right = right,
                           .size = format->nFrames,
                           .sampleRate = format->sampleRate};
        return err_Ok();
    }
}
