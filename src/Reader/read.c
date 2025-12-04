#include "Reader.h"

#include <string.h>

#include "bitcast.h"

Error reader_peekFourCC(Reader* reader, uint8_t* out)
{
    Slice slice;
    Error err = reader->peekSlice(reader, 4, &slice);
    if (err != NoError) {
        return err;
    }

    memcpy(out, slice.slice, 4);
    return NoError;
}

Error reader_takeFourCC(Reader* reader, uint8_t* out)
{
    Error err = reader_peekFourCC(reader, out);
    if (err != NoError) {
        return NoError;
    }

    return reader->skip(reader, 4);
}

Error reader_takeU16_LE(Reader* reader, uint16_t* out)
{
    Slice slice;
    Error err = reader->peekSlice(reader, 2, &slice);
    if (err != NoError) {
        return err;
    }

    *out = bitcastU16_LE(slice.slice);
    return NoError;
}

Error reader_takeU32_LE(Reader* reader, uint32_t* out)
{
    Slice slice;
    Error err = reader->peekSlice(reader, 4, &slice);
    if (err != NoError) {
        return err;
    }

    *out = bitcastU32_LE(slice.slice);
    return NoError;
}

Error reader_takeI24_LE(Reader* reader, Int24* out)
{
    Slice slice;
    Error err = reader->peekSlice(reader, 4, &slice);
    if (err != NoError) {
        return err;
    }

    *out = bitcastI24_LE(slice.slice);
    return NoError;
}
