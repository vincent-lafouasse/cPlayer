#include "Reader.h"

#include <string.h>

#include "bitcast.h"

Error reader_takeSlice(Reader* reader, size_t n, Slice* out)
{
    Error err = reader->peekSlice(reader, n, out);

    if (err == NoError) {
        reader->skip(reader, n);
    }
    return err;
}

Error reader_peekFourCC(Reader* reader, uint8_t* out)
{
    Slice slice;
    TRY(reader->peekSlice(reader, 4, &slice));

    memcpy(out, slice.slice, 4);
    return NoError;
}

Error reader_takeFourCC(Reader* reader, uint8_t* out)
{
    TRY(reader_peekFourCC(reader, out));

    return reader->skip(reader, 4);
}

Error reader_takeU16_LE(Reader* reader, uint16_t* out)
{
    Slice slice;
    TRY(reader->peekSlice(reader, 2, &slice));

    *out = bitcastU16_LE(slice.slice);
    return NoError;
}

Error reader_takeU32_LE(Reader* reader, uint32_t* out)
{
    Slice slice;
    TRY(reader->peekSlice(reader, 4, &slice));

    *out = bitcastU32_LE(slice.slice);
    return NoError;
}

Error reader_takeI24_LE(Reader* reader, Int24* out)
{
    Slice slice;
    TRY(reader->peekSlice(reader, 4, &slice));

    *out = bitcastI24_LE(slice.slice);
    return NoError;
}
