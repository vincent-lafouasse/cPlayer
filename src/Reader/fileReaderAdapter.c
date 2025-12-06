#include "ReaderAdapters.h"

#include <string.h>

#include "Error.h"
#include "FileReader.h"

static Error reader_FileReaderPeekSlice(Reader* reader, size_t n, Slice* out)
{
    FileReader* fileReader = (FileReader*)reader->ctx;

    SliceResult slice = fr_peekSlice(fileReader, n);
    if (slice.status == ReadStatus_ReadErr) {
        return err_Err(E_Read, ERd_ReadFailed);
    } else if (slice.status == ReadStatus_EOF) {
        return err_Err(E_Read, ERd_UnexpectedEOF);
    }

    *out = (Slice){.slice = slice.slice, .len = slice.len};
    return err_Ok();
}

static Error reader_FileReaderSkip(Reader* reader, size_t n)
{
    FileReader* fileReader = (FileReader*)reader->ctx;

    ReadStatus status = fr_skip(fileReader, n);
    if (status == ReadStatus_ReadErr) {
        return err_Err(E_Read, ERd_ReadFailed);
    } else if (status == ReadStatus_EOF) {
        return err_Err(E_Read, ERd_UnexpectedEOF);
    } else {
        reader->offset += n;
        return err_Ok();
    }
}

Reader reader_fromFileReader(FileReader* fileReader)
{
    return (Reader){
        .ctx = fileReader,
        .peekSlice = reader_FileReaderPeekSlice,
        .skip = reader_FileReaderSkip,
        .offset = 0,
    };
}
