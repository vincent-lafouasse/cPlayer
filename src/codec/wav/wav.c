#include "Error.h"
#include "audio.h"
#include "int24.h"
#include "wav_internals.h"

#include <stdlib.h>

AudioDataResult decodeWav(Reader* reader)
{
    WavFormatInfo format;
    const Error err = readWavFormatInfo(reader, &format);
    if (err != NoError) {
        return AudioDataResult_Err(err);
    }
    logWavFormatInfo(&format);

    return readWavData(reader, &format);
}

static AudioDataResult readWavDataMono(Reader* reader,
                                       const WavFormatInfo* format);
static AudioDataResult readWavDataStereo(Reader* reader,
                                         const WavFormatInfo* format);

AudioDataResult readWavData(Reader* reader, const WavFormatInfo* format)
{
    if (format->nChannels == 1) {
        return readWavDataMono(reader, format);
    } else if (format->nChannels == 2) {
        return readWavDataStereo(reader, format);
    } else {
        return AudioDataResult_Err(E_Codec_UnsupportedChannelLayout);
    }
}

static AudioDataResult readWavDataMono(Reader* reader,
                                       const WavFormatInfo* format)
{
    Error err = NoError;

    float* left = malloc(format->nFrames * sizeof(float));
    if (left == NULL) {
        err = E_OOM;
        goto out;
    }

    for (uint32_t i = 0; i < format->nFrames; ++i) {
        err = readSample(reader, format->nFrames, left + i);
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
                             SampleFormat format,
                             float* left,
                             float* right)
{
    TRY(readSample(reader, format, left));
    TRY(readSample(reader, format, right));
    return NoError;
}

static AudioDataResult readWavDataStereo(Reader* reader,
                                         const WavFormatInfo* format)
{
    Error err = NoError;

    float* left = malloc(format->nFrames * sizeof(float));
    float* right = malloc(format->nFrames * sizeof(float));
    if (left == NULL || right == NULL) {
        err = E_OOM;
        goto out;
    }

    for (uint32_t i = 0; i < format->nFrames; ++i) {
        err =
            readStereoFrame(reader, format->sampleFormat, left + i, right + i);
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
