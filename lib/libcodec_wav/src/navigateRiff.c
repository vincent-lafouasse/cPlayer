#include "libcodec_wav.h"
#include "wav_internals.h"

#include <string.h>

#include "bitcast.h"

#include "log.h"

WavError skipChunkUntil(Reader* reader, const char* expectedId)
{
    uint8_t id[5] = {0};

    TRY_IO(reader_peekFourCC(reader, id));
    logFn(LogLevel_Debug, "%s chunk found at offset %u\n", id, reader->offset);
    while (memcmp(id, expectedId, 4) != 0) {
        // skip the fourCC we just peeked
        TRY_IO(reader_skip(reader, 4));

        uint32_t chunkSize = 0;
        TRY_IO(reader_takeU32_LE(reader, &chunkSize));
        logFn(LogLevel_Debug, "Skipping chunk %s of size %u\n", id, chunkSize);
        TRY_IO(reader_skip(reader, chunkSize));
        TRY_IO(reader_peekFourCC(reader, id));
        logFn(LogLevel_Debug, "%s chunk found at offset %u\n", id,
              reader->offset);
    }
    return EWav_Ok;
}

/*static*/ uint32_t fourCC_asU32(const uint8_t fourcc[4])
{
    return bitcastU32_BE(fourcc);
}

WavError getToFormatChunk(Reader* reader)
{
    uint8_t id[5] = {0};
    TRY_IO(reader_takeFourCC(reader, id));
    if (memcmp(id, "RIFF", 4) != 0) {
        return EWav_UnknownFourCC;
    }

    uint32_t size;
    TRY_IO(reader_takeU32_LE(reader, &size));
    logFn(LogLevel_Debug, "master RIFF chunk:\t %u bytes\n", size);

    TRY_IO(reader_takeFourCC(reader, id));
    if (memcmp(id, "WAVE", 4) != 0) {
        return EWav_UnknownFourCC;
    }

    return skipChunkUntil(reader, "fmt ");
}
