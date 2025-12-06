#include "MemoryReader.hpp"

#include <cstring>
#include "Error.h"

MemoryReader::MemoryReader(const std::vector<Byte>& v) : data(v), pos(0) {}

MemoryReader::MemoryReader(const std::string& s) : data(), pos(0)
{
    for (Byte b : s) {
        this->data.push_back(b);
    }
}

Error MemoryReader::peekSlice(size_t size, Slice* out) const
{
    if (this->pos + size > this->data.size()) {
        return err_Err(E_Read, ERd_UnexpectedEOF);
    }

    *out = {.slice = this->data.data() + this->pos, .len = size};
    return err_Ok();
}

Error MemoryReader::skip(size_t size)
{
    if (this->pos + size > this->data.size()) {
        return err_Err(E_Read, ERd_UnexpectedEOF);
    } else {
        this->pos += size;
        return err_Ok();
    }
}

Error memoryReaderPeekSlice(Reader* reader, size_t n, Slice* out)
{
    MemoryReader* memoryReader = static_cast<MemoryReader*>(reader->ctx);

    return memoryReader->peekSlice(n, out);
}

Error memoryReaderSkip(Reader* reader, size_t n)
{
    MemoryReader* memoryReader = static_cast<MemoryReader*>(reader->ctx);

    Error err = memoryReader->skip(n);
    if (!err_isOk(err)) {
        return err;
    } else {
        reader->offset += n;
        return err_Ok();
    }
}

Reader memoryReaderInterface(MemoryReader* memoryReader)
{
    return {
        .ctx = static_cast<void*>(memoryReader),
        .peekSlice = memoryReaderPeekSlice,
        .skip = memoryReaderSkip,
        .offset = 0,
    };
}
