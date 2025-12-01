#include "Error.h"
#include "audio.h"
#include "int24.h"
#include "wav_internals.h"

#include <stdlib.h>

AudioDataResult decodeWav(FileReader* reader)
{
    const WavHeaderResult maybeHeader = readWavHeader(reader);
    if (maybeHeader.err != NoError) {
        return AudioDataResult_Err(maybeHeader.err);
    }

    const Header header = maybeHeader.header;
    logHeader(&header);

    return readWavData(reader, header);
}

static AudioDataResult readWavDataMono(FileReader* reader, Header h);
static AudioDataResult readWavDataStereo(FileReader* reader, Header h);

AudioDataResult readWavData(FileReader* reader, Header h)
{
    if (h.nChannels == 1) {
        return readWavDataMono(reader, h);
    } else if (h.nChannels == 2) {
        return readWavDataStereo(reader, h);
    } else {
        return AudioDataResult_Err(E_Codec_UnsupportedChannelLayout);
    }
}

static AudioDataResult readWavDataMono(FileReader* reader, Header h)
{
    Error err = NoError;

    float* left = malloc(h.size * sizeof(float));
    if (left == NULL) {
        err = E_OOM;
        goto out;
    }

    for (uint32_t i = 0; i < h.size; ++i) {
        FloatResult maybeSample = readSample(reader, h.sampleFormat);
        if (maybeSample.err != NoError) {
            err = maybeSample.err;
            goto out;
        }
        left[i] = maybeSample.f;
    }
    dumpFloatCsv(left, h.size, DUMP_PREFIX "float" DUMP_SUFFIX);

out:
    if (err == NoError) {
        const AudioData track = (AudioData){.left = left,
                                            .right = left,
                                            .size = h.size,
                                            .sampleRate = h.sampleRate};
        return AudioDataResult_Ok(track);
    } else {
        free(left);
        return AudioDataResult_Err(err);
    }
}

static Error readStereoFrame(FileReader* reader,
                             SampleFormat format,
                             float* left,
                             float* right)
{
    FloatResult maybeLeft = readSample(reader, format);
    if (maybeLeft.err != NoError) {
        return maybeLeft.err;
    }

    FloatResult maybeRight = readSample(reader, format);
    if (maybeRight.err != NoError) {
        return maybeRight.err;
    }

    *left = maybeLeft.f;
    *right = maybeRight.f;
    return NoError;
}

static AudioDataResult readWavDataStereo(FileReader* reader, Header h)
{
    Error err = NoError;

    float* left = malloc(h.size * sizeof(float));
    float* right = malloc(h.size * sizeof(float));
    if (left == NULL || right == NULL) {
        err = E_OOM;
        goto out;
    }

    for (uint32_t i = 0; i < h.size; ++i) {
        Error readError =
            readStereoFrame(reader, h.sampleFormat, left + i, right + i);
        if (readError != NoError) {
            err = readError;
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
                                            .size = h.size,
                                            .sampleRate = h.sampleRate};
        return AudioDataResult_Ok(track);
    }
}
