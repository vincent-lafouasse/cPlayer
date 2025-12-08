#pragma once

#include <string>
#include <vector>

#include "Reader.h"

using Byte = uint8_t;

class MockReader {
   private:
    std::vector<Byte> data;
    size_t pos;

   public:
    MockReader(const std::vector<Byte>& v);
    MockReader(const std::string& s);

    ReadStatus peekSlice(size_t size, Slice* out) const;
    ReadStatus skip(size_t size);
};

ReadStatus mockReaderPeekSlice(Reader* reader, size_t n, Slice* out);
ReadStatus mockReaderSkip(Reader* reader, size_t n);
Reader mockReaderInterface(MockReader* memoryReader);
