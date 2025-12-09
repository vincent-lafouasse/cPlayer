#include "ReaderAdapters.h"

#include "FileReader.h"

static LibStream_ReadStatus ls_Ok(void)
{
    return LibStream_ReadStatus_Ok;
}

static LibStream_ReadStatus ls_EOF(void)
{
    return LibStream_ReadStatus_UnexpectedEOF;
}

static bool ls_isOk(LibStream_ReadStatus err)
{
    return err == ls_Ok();
}

static LibStream_ReadStatus reader_FileReaderPeekSlice(Reader* reader,
                                                       size_t n,
                                                       Slice* out)
{
    FileReader* fileReader = (FileReader*)reader->ctx;

    SliceResult slice = fr_peekSlice(fileReader, n);
    if (slice.status == FileReader_ReadStatus_ReadErr) {
        return LibStream_ReadStatus_ReadFailed;
    } else if (slice.status == FileReader_ReadStatus_EOF) {
        return LibStream_ReadStatus_UnexpectedEOF;
    }

    *out = (Slice){.slice = slice.slice, .len = slice.len};
    return ls_Ok();
}

static LibStream_ReadStatus reader_FileReaderSkip(Reader* reader, size_t n)
{
    FileReader* fileReader = (FileReader*)reader->ctx;

    FileReader_ReadStatus status = fr_skip(fileReader, n);
    if (status == FileReader_ReadStatus_ReadErr) {
        return LibStream_ReadStatus_ReadFailed;
    } else if (status == FileReader_ReadStatus_EOF) {
        return LibStream_ReadStatus_UnexpectedEOF;
    } else {
        reader->offset += n;
        return ls_Ok();
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
