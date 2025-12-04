#include <cstdint>
#include <string>

#include "gtest/gtest.h"

#include "MemoryReader.hpp"

extern "C" {
#include "wav_internals.h"
}

TEST(WavHeader, Foo)
{
    std::vector<Byte> data = {};
    MemoryReader memoryReader(data);
    Reader reader = memoryReaderInterface(&memoryReader);
    WavHeader header;
    (void)reader;
    (void)header;
    FAIL();
}
