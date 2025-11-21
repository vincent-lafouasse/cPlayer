#include "wav_internals.h"

#include <assert.h>
#include <stdlib.h>

#include "log.h"

// unsafe to use with actual i32 but i24 will never reach i32 min/max so it's
// fine
static int32_t maxAbs(int32_t* data, uint32_t sz)
{
    int32_t out = -1;

    for (uint32_t i = 0; i < sz; ++i) {
        int32_t abs = data[i] > 0 ? data[i] : -data[i];
        if (abs > out) {
            out = abs;
        }
    }

    return out;
}

AudioData decodeWav(FileReader* reader)
{
    Header header = readWavHeader(reader);
    logHeader(&header);

    return readWavData(reader, header);
}

AudioData readWavData(FileReader* reader, Header h)
{
    int32_t* intData = malloc(h.size * sizeof(int32_t));

    for (uint32_t i = 0; i < h.size; ++i) {
        assert(readI24AsI32LE(reader, intData + i) == Read_Ok);
        uint8_t garbage;
        assert(fr_takeByte(reader, &garbage) == Read_Ok);
        assert(fr_takeByte(reader, &garbage) == Read_Ok);
        assert(fr_takeByte(reader, &garbage) == Read_Ok);
    }
    dumpIntCsv(intData, h.size, DUMP_PREFIX "i24" DUMP_SUFFIX);

    float* data = malloc(h.size * sizeof(float));
    const float normalisationFactor = (float)maxAbs(intData, h.size);
    for (uint32_t i = 0; i < h.size; ++i) {
        data[i] = (float)intData[i] / normalisationFactor;
    }
    dumpFloatCsv(data, h.size, DUMP_PREFIX "float" DUMP_SUFFIX);

    return (AudioData){.left = data,
                       .right = data,
                       .size = h.size,
                       .sampleRate = h.sampleRate};
}
