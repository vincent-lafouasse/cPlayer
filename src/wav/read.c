#include "wav_internals.h"

#include "int24.h"

ReadResult readFourCC(FileReader* reader, uint8_t* out)
{
    for (size_t i = 0; i < 4; ++i) {
        const ReadResult res = fr_takeByte(reader, out + i);
        if (res != Read_Ok) {
            return res;
        }
    }
    return Read_Ok;
}

ReadResult readI24AsI32LE(FileReader* reader, int32_t* out)
{
    uint8_t bytes[3];
    for (size_t i = 0; i < 3; ++i) {
        const ReadResult res = fr_takeByte(reader, bytes + i);
        if (res != Read_Ok) {
            return res;
        }
    }
    *out = i24_asI32((Int24){bytes[2], bytes[1], bytes[0]});
    return Read_Ok;
}
