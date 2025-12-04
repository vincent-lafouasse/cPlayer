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

std::vector<Byte> le16(uint16_t x)
{
    return {Byte(x & 0xFF), Byte((x >> 8) & 0xFF)};
}

void appendData(std::vector<Byte>& buf, const std::vector<Byte>& data)
{
    buf.insert(buf.end(), data.begin(), data.end());
}

void appendString(std::vector<Byte>& buf, const std::string& s)
{
    for (uint8_t byte : s) {
        buf.push_back(byte);
    }
}

void appendU16(std::vector<Byte>& buf, uint16_t x)
{
    appendData(buf, le16(x));
}

void appendU32(std::vector<Byte>& buf, uint32_t x)
{
    appendData(buf, le32(x));
}

// make a chunk
void appendChunk(std::vector<Byte>& buf,
                 const std::string& fourcc,
                 uint32_t size,
                 const std::vector<Byte>& content)
{
    appendString(buf, fourcc);
    appendU32(buf, size);
    appendData(buf, content);
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

// Helper: append RIFF/WAVE header
void appendRiffWave(std::vector<Byte>& buf, uint32_t totalSize)
{
    appendString(buf, "RIFF");
    appendU32(buf, totalSize);
    appendString(buf, "WAVE");
}

TEST(WavReader, GetToFormatChunk_SimpleFmt)
{
    std::vector<Byte> data;
    appendRiffWave(data, 20);
    appendString(data, "fmt ");

    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    ASSERT_EQ(getToFormatChunk(&r), NoError);

    Slice s;
    ASSERT_EQ(reader_takeSlice(&r, 4, &s), NoError);
    EXPECT_EQ(std::string((char*)s.slice, s.len), "fmt ");
}

TEST(WavReader, GetToFormatChunk_JunkBeforeFmt)
{
    std::vector<Byte> data;
    appendRiffWave(data, 40);
    appendChunk(data, "JUNK", 4, {0x01, 0x02, 0x03, 0x04});
    appendString(data, "fmt ");

    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    ASSERT_EQ(getToFormatChunk(&r), NoError);

    Slice s;
    ASSERT_EQ(reader_takeSlice(&r, 4, &s), NoError);
    EXPECT_EQ(std::string((char*)s.slice, s.len), "fmt ");
}

TEST(WavReader, GetToFormatChunk_MultipleJunk)
{
    std::vector<Byte> data;
    appendRiffWave(data, 60);
    appendChunk(data, "JUNK", 2, {0xAA, 0xBB});
    appendChunk(data, "FAKE", 4, {0x01, 0x02, 0x03, 0x04});
    appendString(data, "fmt ");

    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    ASSERT_EQ(getToFormatChunk(&r), NoError);

    Slice s;
    ASSERT_EQ(reader_takeSlice(&r, 4, &s), NoError);
    EXPECT_EQ(std::string((char*)s.slice, s.len), "fmt ");
}

TEST(WavReader, GetToFormatChunk_TruncatedRiff)
{
    std::vector<Byte> data;
    data.insert(data.end(), {'R', 'I', 'F'});  // only 3 bytes, truncated

    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    EXPECT_EQ(getToFormatChunk(&r), E_UnexpectedEOF);
}

TEST(WavReader, GetToFormatChunk_NotWave)
{
    std::vector<Byte> data;
    data.insert(data.end(), {'R', 'I', 'F', 'F', 0x10, 0x00, 0x00, 0x00, 'N',
                             'O', 'P', 'E'});

    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    EXPECT_EQ(getToFormatChunk(&r), E_Wav_UnknownFourCC);
}

TEST(WavReader, GetToFormatChunk_NoFmtChunk)
{
    std::vector<Byte> data;
    appendRiffWave(data, 12);
    appendChunk(data, "JUNK", 4, {0x00, 0x01, 0x02, 0x03});

    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    // Should fail because no fmt chunk exists
    EXPECT_EQ(getToFormatChunk(&r), E_UnexpectedEOF);
}

struct RawWavHeader {  // for reference
    uint32_t size;
    uint16_t formatTag;
    uint16_t nChannels;
    uint32_t sampleRate;
    uint32_t avgBytesPerSec;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    uint16_t extensionSize;
    // extension
    uint16_t validBitsPerSample;
    uint32_t channelMask;
    Byte subFormat[16];
};

// Helper: append format chunk
void appendFmtChunk(std::vector<Byte>& buf,
                    uint16_t formatTag,
                    uint16_t nChannels,
                    uint32_t sampleRate,
                    uint16_t bitDepth,
                    uint16_t blockSize)
{
    std::vector<Byte> content;
    appendU16(buf, formatTag);
    appendU16(buf, nChannels);
    appendU32(content, sampleRate);

    // ByteRate (sampleRate * nChannels * bitDepth/8)
    uint32_t byteRate = sampleRate * nChannels * (bitDepth / 8);
    appendU32(content, byteRate);

    // BlockAlign
    content.push_back(blockSize & 0xFF);
    content.push_back((blockSize >> 8) & 0xFF);

    // BitsPerSample
    content.push_back(bitDepth & 0xFF);
    content.push_back((bitDepth >> 8) & 0xFF);

    appendChunk(buf, "fmt ", content.size(), content);
}
