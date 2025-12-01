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
    float* left = malloc(h.size * sizeof(float));
    if (left == NULL) {
        return AudioDataResult_Err(E_OOM);
    }

    for (uint32_t i = 0; i < h.size; ++i) {
        FloatResult maybeSample = readSample(reader, h.sampleFormat);
        if (maybeSample.err != NoError) {
            free(left);
            return AudioDataResult_Err(maybeSample.err);
        }
        left[i] = maybeSample.f;
    }
    dumpFloatCsv(left, h.size, DUMP_PREFIX "float" DUMP_SUFFIX);

    const AudioData track = (AudioData){.left = left,
                                        .right = left,
                                        .size = h.size,
                                        .sampleRate = h.sampleRate};
    return AudioDataResult_Ok(track);
}

static AudioDataResult readWavDataStereo(FileReader* reader, Header h)
{
    float* left = malloc(h.size * sizeof(float));
    if (left == NULL) {
        return AudioDataResult_Err(E_OOM);
    }
    float* right = malloc(h.size * sizeof(float));
    if (right == NULL) {
        free(left);
        return AudioDataResult_Err(E_OOM);
    }

    for (uint32_t i = 0; i < h.size; ++i) {
        FloatResult maybeSample = readSample(reader, h.sampleFormat);
        if (maybeSample.err != NoError) {
            free(left);
            free(right);
            return AudioDataResult_Err(maybeSample.err);
        }
        left[i] = maybeSample.f;

        maybeSample = readSample(reader, h.sampleFormat);
        if (maybeSample.err != NoError) {
            free(left);
            free(right);
            return AudioDataResult_Err(maybeSample.err);
        }
        right[i] = maybeSample.f;
    }
    dumpFloatCsv(left, h.size, DUMP_PREFIX "float" DUMP_SUFFIX);

    const AudioData track = (AudioData){.left = left,
                                        .right = right,
                                        .size = h.size,
                                        .sampleRate = h.sampleRate};
    return AudioDataResult_Ok(track);
}
