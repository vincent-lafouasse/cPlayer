#include "MockReader.hpp"

#include <cstring>
#include "Reader.h"

MockReader::MockReader(const std::vector<Byte>& v) : data(v), pos(0) {}

MockReader::MockReader(const std::string& s) : data(), pos(0)
{
    for (Byte b : s) {
        this->data.push_back(b);
    }
}

ReadStatus MockReader::peekSlice(size_t size, Slice* out) const
{
    if (this->pos + size > this->data.size()) {
        return ReadStatus_UnexpectedEOF;
    }

    *out = {.slice = this->data.data() + this->pos, .len = size};
    return ReadStatus_Ok;
}

ReadStatus MockReader::skip(size_t size)
{
    if (this->pos + size > this->data.size()) {
        return ReadStatus_UnexpectedEOF;
    } else {
        this->pos += size;
        return ReadStatus_Ok;
    }
}

ReadStatus mockReaderPeekSlice(Reader* reader, size_t n, Slice* out)
{
    MockReader* mockReader = static_cast<MockReader*>(reader->ctx);

    return mockReader->peekSlice(n, out);
}

ReadStatus mockReaderSkip(Reader* reader, size_t n)
{
    MockReader* mockReader = static_cast<MockReader*>(reader->ctx);

    ReadStatus err = mockReader->skip(n);
    if (err != ReadStatus_Ok) {
        return err;
    } else {
        reader->offset += n;
        return ReadStatus_Ok;
    }
}

Reader mockReaderInterface(MockReader* mockReader)
{
    return {
        .ctx = static_cast<void*>(mockReader),
        .peekSlice = mockReaderPeekSlice,
        .skip = mockReaderSkip,
        .offset = 0,
    };
}
