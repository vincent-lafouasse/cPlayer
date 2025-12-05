#include "Error.h"
#include "wav_internals.h"

#include <stdlib.h>

static AudioDataResult readMonoPCM(Reader* reader, const WavFormatInfo* format);
static AudioDataResult readStereoPCM(Reader* reader,
                                     const WavFormatInfo* format);

AudioDataResult readWavData(Reader* reader, const WavFormatInfo* format)
{
    if (format->formatTag != WAVE_FORMAT_PCM) {
        return AudioDataResult_Err(E_Wav_UnsupportedSampleFormat);
    }

    if (format->nChannels == 1) {
        return readMonoPCM(reader, format);
    } else if (format->nChannels == 2) {
        return readStereoPCM(reader, format);
    } else {
        return AudioDataResult_Err(E_Codec_UnsupportedChannelLayout);
    }
}

static AudioDataResult readMonoPCM(Reader* reader, const WavFormatInfo* format)
{
    Error err = NoError;

    float* left = calloc(format->nFrames, sizeof(float));
    if (left == NULL) {
        err = E_OOM;
        goto out;
    }

    for (uint32_t i = 0; i < format->nFrames; ++i) {
        err = readSample(reader, format, left + i);
        if (err != NoError) {
            goto out;
        }
    }
    dumpFloatCsv(left, format->nFrames, DUMP_PREFIX "float" DUMP_SUFFIX);

out:
    if (err == NoError) {
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

static Error readStereoFrame(Reader* reader,
                             const WavFormatInfo* format,
                             float* left,
                             float* right)
{
    TRY(readSample(reader, format, left));
    TRY(readSample(reader, format, right));
    return NoError;
}

static AudioDataResult readStereoPCM(Reader* reader,
                                     const WavFormatInfo* format)
{
    Error err = NoError;

    float* left = calloc(format->nFrames, sizeof(float));
    float* right = calloc(format->nFrames, sizeof(float));
    if (left == NULL || right == NULL) {
        err = E_OOM;
        goto out;
    }

    for (uint32_t i = 0; i < format->nFrames; ++i) {
        err = readStereoFrame(reader, format, left + i, right + i);
        if (err != NoError) {
            goto out;
        }
    }

out:
    if (err != NoError) {
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
