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
    float* right = malloc(h.size * sizeof(float));

    for (uint32_t i = 0; i < h.size; ++i) {
        (void)readSample(reader, Signed24, left + i);
        (void)readSample(reader, Signed24, right + i);
    }
    dumpFloatCsv(left, h.size, DUMP_PREFIX "float" DUMP_SUFFIX);

    return (AudioData){.left = left,
                       .right = right,
                       .size = h.size,
                       .sampleRate = h.sampleRate};
}
