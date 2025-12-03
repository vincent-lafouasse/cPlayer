#include "Reader.h"

static bool readFromFixedBufferReader(Reader* reader, uint8_t* buffer, size_t n)
{
    FixedBufferReader* fbReader = reader->ctx;

    if (fbReader->head + n < FIXED_BUFFER_SIZE) {
        return false;
    }

    memcpy(buffer, fbReader->buffer + fbReader->head, n);
    reader->offset += n;
    fbReader->head += n;
    return true;
}

static bool skipFromFixedBufferReader(Reader* reader, size_t n)
{
    FixedBufferReader* fbReader = reader->ctx;

    if (fbReader->head + n < FIXED_BUFFER_SIZE) {
        return false;
    }

    fbReader->head += n;
    return true;
}

Reader readerFromFixedBufferReader(FixedBufferReader* fbReader)
{
    return (Reader){.ctx = fbReader,
                    .read = readFromFixedBufferReader,
                    .skip = skipFromFixedBufferReader,
                    .offset = 0};
}
