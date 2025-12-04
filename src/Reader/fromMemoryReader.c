#include "Reader.h"

static bool readFromMemoryReader(Reader* reader, uint8_t* buffer, size_t n)
{
    MemoryReader* memoryReader = reader->ctx;

    if (memoryReader->head + n > FIXED_BUFFER_SIZE) {
        return false;
    }

    memcpy(buffer, memoryReader->buffer + memoryReader->head, n);
    reader->offset += n;
    memoryReader->head += n;
    return true;
}

static bool skipFromMemoryReader(Reader* reader, size_t n)
{
    MemoryReader* memoryReader = reader->ctx;

    if (memoryReader->head + n > FIXED_BUFFER_SIZE) {
        return false;
    }

    memoryReader->head += n;
    return true;
}

Reader readerFromMemoryReader(MemoryReader* memoryReader)
{
    return (Reader){.ctx = memoryReader,
                    .read = readFromMemoryReader,
                    .skip = skipFromMemoryReader,
                    .offset = 0};
}
