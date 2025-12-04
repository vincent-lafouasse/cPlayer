#include "Error.h"
#include "FileReader.h"
#include "ReaderAdapters.h"

#include <string.h>

static Error reader_FileReaderPeekSlice(Reader* reader, size_t n, Slice* out)
{
    FileReader* fileReader = (FileReader*)reader->ctx;

    SliceResult slice = fr_peekSlice(fileReader, n);
    if (slice.status == ReadStatus_ReadErr) {
        return E_FailedRead;
    } else if (slice.status == ReadStatus_EOF) {
        return E_UnexpectedEOF;
    }

    *out = (Slice){.slice = slice.slice, .len = slice.len};
    return NoError;
}

static Error reader_FileReaderPeekInto(Reader* reader, size_t n, uint8_t* out)
{
    FileReader* fileReader = (FileReader*)reader->ctx;

    SliceResult slice = fr_peekSlice(fileReader, n);
    if (slice.status == ReadStatus_ReadErr) {
        return E_FailedRead;
    } else if (slice.status == ReadStatus_EOF) {
        return E_UnexpectedEOF;
    }

    memcpy(out, slice.slice, slice.len);
    return NoError;
}

static Error reader_FileReaderSkip(Reader* reader, size_t n)
{
    FileReader* fileReader = (FileReader*)reader->ctx;

    ReadStatus status = fr_skip(fileReader, n);
    if (status == ReadStatus_ReadErr) {
        return E_FailedRead;
    } else if (status == ReadStatus_EOF) {
        return E_UnexpectedEOF;
    } else {
        reader->offset += n;
        return NoError;
    }
}

Reader reader_fromFileReader(FileReader* fileReader)
{
    return (Reader){
        .ctx = fileReader,
        .peekSlice = reader_FileReaderPeekSlice,
        .peekInto = reader_FileReaderPeekInto,
        .skip = reader_FileReaderSkip,
        .offset = 0,
    };
}
