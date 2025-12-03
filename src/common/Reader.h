#pragma once

#include <string.h>
#include <sys/types.h>

#include "FileReader.h"

typedef struct Reader Reader;

typedef ssize_t (*ReadFn)(Reader*, uint8_t*, size_t);  // POSIX read-esque
typedef ssize_t (*SkipFn)(Reader*, size_t);            // POSIX lseek-esque

struct Reader {
    void* ctx;
    ReadFn read;
    SkipFn skip;
    size_t offset;
};

ssize_t readFromFileReader(Reader* reader, uint8_t* buffer, size_t n)
{
    FileReader* fileReader = reader->ctx;

    SliceResult slice = fr_takeSlice(fileReader, n);
    if (slice.status == ReadStatus_ReadErr) {
        return -1;
    } else if (slice.status == ReadStatus_EOF) {
        return 0;
    } else {
        memcpy(buffer, slice.slice, slice.len);
        reader->offset += slice.len;
        return slice.len;
    }
}

ssize_t skipFromFileReader(Reader* reader, size_t n)
{
    FileReader* fileReader = reader->ctx;
    ssize_t bytesSkipped = 0;

    for (size_t i = 0; i < n; ++i) {
        ByteResult tmp = fr_takeByte(fileReader);
        if (tmp.status == ReadStatus_ReadErr) {
            return -1;
        } else if (tmp.status == ReadStatus_EOF) {
            break;
        }

        bytesSkipped++;
        reader->offset++;
    }

    return bytesSkipped;
}

Reader reader_fromFileReader(FileReader* fileReader)
{
    return (Reader){.ctx = fileReader,
                    .read = readFromFileReader,
                    .skip = skipFromFileReader,
                    .offset = 0};
}
