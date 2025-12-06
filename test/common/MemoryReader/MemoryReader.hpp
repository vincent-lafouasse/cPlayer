#pragma once

#include <string>
#include <vector>

extern "C" {
#include "Reader.h"
}

using Byte = uint8_t;

class MemoryReader {
   private:
    std::vector<Byte> data;
    size_t pos;

   public:
    MemoryReader(const std::vector<Byte>& v);
    MemoryReader(const std::string& s);

    Error64 peekSlice(size_t size, Slice* out) const;
    Error64 peekInto(size_t size, Byte* out) const;
    Error64 skip(size_t size);
};

Error64 memoryReaderPeekSlice(Reader* reader, size_t n, Slice* out);
Error64 memoryReaderPeekInto(Reader* reader, size_t n, uint8_t* out);
Error64 memoryReaderSkip(Reader* reader, size_t n);
Reader memoryReaderInterface(MemoryReader* memoryReader);
