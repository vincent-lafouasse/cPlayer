#include "Reader.h"

#include <string.h>

static bool readFromFileReader(Reader* reader, uint8_t* buffer, size_t n)
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

static bool skipFromFileReader(Reader* reader, size_t n)
{
    FileReader* fileReader = reader->ctx;

    SliceResult slice = fr_takeSlice(fileReader, n);

    return slice.status == ReadStatus_Ok;
}

Reader reader_fromFileReader(FileReader* fileReader)
{
    (void)fileReader;
    return (Reader){0};
}
