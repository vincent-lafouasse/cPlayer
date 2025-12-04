#include <cstdint>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "MemoryReader.hpp"

extern "C" {
#include "wav_internals.h"
}

// Helper to write u32 little-endian
std::vector<Byte> le32(uint32_t x)
{
    return {Byte(x & 0xFF), Byte((x >> 8) & 0xFF), Byte((x >> 16) & 0xFF),
            Byte((x >> 24) & 0xFF)};
}

// make a chunk
void appendChunk(std::vector<Byte>& buf,
                 const std::string& fourcc,
                 uint32_t size,
                 const std::vector<Byte>& content)
{
    buf.insert(buf.end(), fourcc.begin(), fourcc.end());
    auto sz = le32(size);
    buf.insert(buf.end(), sz.begin(), sz.end());
    buf.insert(buf.end(), content.begin(), content.end());
}

// ----------------------------
// skipChunkUntil tests
// ----------------------------
TEST(WavReader, SkipChunkUntil_Success)
{
    std::vector<Byte> data;
    appendChunk(data, "JUNK", 4, {'a', 'b', 'c', 'd'});
    appendChunk(data, "fmt ", 16, std::vector<Byte>(16, 0x11));

    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    ASSERT_EQ(skipChunkUntil(&r, "fmt "), NoError);
    Slice s;
    ASSERT_EQ(reader_takeSlice(&r, 4, &s), NoError);
    EXPECT_EQ(std::string((char*)s.slice, s.len), "fmt ");
}

TEST(WavReader, SkipChunkUntil_UnexpectedEOF)
{
    std::vector<Byte> data;
    appendChunk(data, "JUNK", 10, {});  // content missing
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    EXPECT_EQ(skipChunkUntil(&r, "fmt "), E_UnexpectedEOF);
}

TEST(WavReader, SkipChunkUntil_MultipleJunk)
{
    std::vector<Byte> data;
    appendChunk(data, "JUNK", 2, {0xAA, 0xBB});
    appendChunk(data, "FAKE", 4, {0x01, 0x02, 0x03, 0x04});
    appendChunk(data, "TRSH", 1, {0x00});
    appendChunk(data, "fmt ", 16, std::vector<Byte>(16, 0x22));
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    ASSERT_EQ(skipChunkUntil(&r, "fmt "), NoError);

    Slice s;
    ASSERT_EQ(reader_takeSlice(&r, 4, &s), NoError);
    EXPECT_EQ(std::string((char*)s.slice, s.len), "fmt ");
}

TEST(WavReader, SkipChunkUntil_ImmediateFmt)
{
    std::vector<Byte> data;
    appendChunk(data, "fmt ", 16, std::vector<Byte>(16, 0x33));
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    ASSERT_EQ(skipChunkUntil(&r, "fmt "), NoError);
    Slice s;
    ASSERT_EQ(reader_takeSlice(&r, 4, &s), NoError);
    EXPECT_EQ(std::string((char*)s.slice, s.len), "fmt ");
}

TEST(WavReader, SkipChunkUntil_EmptyReader)
{
    std::vector<Byte> data;  // empty
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);
    EXPECT_EQ(skipChunkUntil(&r, "fmt "), E_UnexpectedEOF);
}
TEST(WavReader, SkipChunkUntil_MalformedSize)

{
    std::vector<Byte> data;
    appendChunk(data, "JUNK", 0xFFFFFFF0, {});  // huge size (beyond buffer)
    appendChunk(data, "fmt ", 16, std::vector<Byte>(16, 0x44));
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    // Should fail due to trying to skip beyond EOF
    EXPECT_EQ(skipChunkUntil(&r, "fmt "), E_UnexpectedEOF);
}
