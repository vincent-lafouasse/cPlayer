#pragma once

#include <stdbool.h>
#include <string.h>

#include "FileReader.h"

typedef struct Reader Reader;

// true means a complete read has happened
// false means partial read or read error
typedef bool (*ReadFn)(Reader*, uint8_t*, size_t);
typedef bool (*SkipFn)(Reader*, size_t);

struct Reader {
    void* ctx;
    ReadFn read;
    SkipFn skip;
    size_t offset;
};

bool readFromFileReader(Reader* reader, uint8_t* buffer, size_t n)
{
    FileReader* fileReader = reader->ctx;

    SliceResult slice = fr_takeSlice(fileReader, n);
    if (slice.status != ReadStatus_Ok) {
        return false;
    }

    memcpy(buffer, slice.slice, slice.len);
    reader->offset += slice.len;
    return true;
}

bool skipFromFileReader(Reader* reader, size_t n)
{
    FileReader* fileReader = reader->ctx;

    SliceResult slice = fr_takeSlice(fileReader, n);

    return slice.status == ReadStatus_Ok;
}

Reader reader_fromFileReader(FileReader* fileReader)
{
    return (Reader){.ctx = fileReader,
                    .read = readFromFileReader,
                    .skip = skipFromFileReader,
                    .offset = 0};
}

#define FIXED_BUFFER_SIZE 4096

typedef struct {
    uint8_t buffer[FIXED_BUFFER_SIZE];
    size_t head;
} FixedBufferReader;

bool readFromFixedBufferReader(Reader* reader, uint8_t* buffer, size_t n)
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

bool skipFromFixedBufferReader(Reader* reader, size_t n)
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
