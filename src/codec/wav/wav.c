#include "Error.h"
#include "audio.h"
#include "int24.h"
#include "wav_internals.h"

#include <assert.h>
#include <stdlib.h>

AudioDataResult decodeWav(FileReader* reader)
{
    const WavHeaderResult maybeHeader = readWavHeader(reader);
    if (maybeHeader.err != NoError) {
        return (AudioDataResult){.err = maybeHeader.err};
    }

    const Header header = maybeHeader.header;
    logHeader(&header);

    return readWavData(reader, header);
}

AudioDataResult readWavData(FileReader* reader, Header h)
{
    float* left = malloc(h.size * sizeof(float));
    float* right;
    if (h.nChannels > 1) {
        right = malloc(h.size * sizeof(float));
    } else {
        right = NULL;
    }

    for (uint32_t i = 0; i < h.size; ++i) {
        FloatResult maybeSample = readSample(reader, h.sampleFormat);
        left[i] = maybeSample.f;
        if (h.nChannels > 1) {
            maybeSample = readSample(reader, h.sampleFormat);
            right[i] = maybeSample.f;
        }
    }
    dumpFloatCsv(left, h.size, DUMP_PREFIX "float" DUMP_SUFFIX);

    if (h.nChannels == 1) {
        right = left;
    }
    const AudioData track = (AudioData){.left = left,
                                        .right = right,
                                        .size = h.size,
                                        .sampleRate = h.sampleRate};
    return (AudioDataResult){.track = track, .err = NoError};
}
