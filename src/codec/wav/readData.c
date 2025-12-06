#include "wav_internals.h"

#include <stdlib.h>

#include "Error64.h"

static AudioDataResult readMonoPCM(Reader* reader, const WavFormatInfo* format);
static AudioDataResult readStereoPCM(Reader* reader,
                                     const WavFormatInfo* format);

AudioDataResult readWavData(Reader* reader, const WavFormatInfo* format)
{
    if (format->formatTag != WAVE_FORMAT_PCM) {
        return AudioDataResult_Err(err_withCtx(
            E64_Wav, EWav_UnsupportedSampleFormat, format->formatTag));
    }

    if (format->nChannels == 1) {
        return readMonoPCM(reader, format);
    } else if (format->nChannels == 2) {
        return readStereoPCM(reader, format);
    } else {
        return AudioDataResult_Err(err_withCtx(
            E64_Codec, ECdc_UnsupportedChannelLayout, format->nChannels));
    }
}

static AudioDataResult readMonoPCM(Reader* reader, const WavFormatInfo* format)
{
    Error64 err = err_Ok();

    float* left = calloc(format->nFrames, sizeof(float));
    if (left == NULL) {
        err = err_Err(E64_System, ESys_OutOfMemory);
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
        const AudioData track = (AudioData){.left = left,
                                            .right = left,
                                            .size = format->nFrames,
                                            .sampleRate = format->sampleRate};
        return AudioDataResult_Ok(track);
    } else {
        free(left);
        return AudioDataResult_Err(err);
    }
}

static Error64 readStereoFrame(Reader* reader,
                               const WavFormatInfo* format,
                               float* left,
                               float* right)
{
    TRY64(readSample(reader, format, left));
    TRY64(readSample(reader, format, right));
    return err_Ok();
}

static AudioDataResult readStereoPCM(Reader* reader,
                                     const WavFormatInfo* format)
{
    Error64 err = err_Ok();

    float* left = calloc(format->nFrames, sizeof(float));
    float* right = calloc(format->nFrames, sizeof(float));
    if (left == NULL || right == NULL) {
        err = err_Err(E64_System, ESys_OutOfMemory);
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
        return AudioDataResult_Err(err);
    } else {
        const AudioData track = (AudioData){.left = left,
                                            .right = right,
                                            .size = format->nFrames,
                                            .sampleRate = format->sampleRate};
        return AudioDataResult_Ok(track);
    }
}
