#include "int24.h"
#include "wav_internals.h"

#include <assert.h>
#include <stdlib.h>

AudioData decodeWav(FileReader* reader)
{
    const Header header = readWavHeader(reader);
    logHeader(&header);

    return readWavData(reader, header);
}

AudioData readWavData(FileReader* reader, Header h)
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
    return (AudioData){.left = left,
                       .right = right,
                       .size = h.size,
                       .sampleRate = h.sampleRate};
}
