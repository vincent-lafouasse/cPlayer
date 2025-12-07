#include <cstdint>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "MemoryReader.hpp"

#include "Error.h"
#include "wav_internals.h"

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

    ASSERT_TRUE(err_isOk(skipChunkUntil(&r, "fmt ")));
    Slice s;
    ASSERT_TRUE(err_isOk(reader_takeSlice(&r, 4, &s)));
    EXPECT_EQ(std::string((char*)s.slice, s.len), "fmt ");
}

TEST(WavReader, SkipChunkUntil_UnexpectedEOF)
{
    std::vector<Byte> data;
    appendChunk(data, "JUNK", 10, {});  // content missing
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    Error err = skipChunkUntil(&r, "fmt ");
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
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

    ASSERT_TRUE(err_isOk(skipChunkUntil(&r, "fmt ")));

    Slice s;
    ASSERT_TRUE(err_isOk(reader_takeSlice(&r, 4, &s)));
    EXPECT_EQ(std::string((char*)s.slice, s.len), "fmt ");
}

TEST(WavReader, SkipChunkUntil_ImmediateFmt)
{
    std::vector<Byte> data;
    appendChunk(data, "fmt ", 16, std::vector<Byte>(16, 0x33));
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    ASSERT_TRUE(err_isOk(skipChunkUntil(&r, "fmt ")));
    Slice s;
    ASSERT_TRUE(err_isOk(reader_takeSlice(&r, 4, &s)));
    EXPECT_EQ(std::string((char*)s.slice, s.len), "fmt ");
}

TEST(WavReader, SkipChunkUntil_EmptyReader)
{
    std::vector<Byte> data;  // empty
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    Error err = skipChunkUntil(&r, "fmt ");
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}
TEST(WavReader, SkipChunkUntil_MalformedSize)

{
    std::vector<Byte> data;
    appendChunk(data, "JUNK", 0xFFFFFFF0, {});  // huge size (beyond buffer)
    appendChunk(data, "fmt ", 16, std::vector<Byte>(16, 0x44));
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    // Should fail due to trying to skip beyond EOF
    Error err = skipChunkUntil(&r, "fmt ");
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
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

    ASSERT_TRUE(err_isOk(getToFormatChunk(&r)));

    Slice s;
    ASSERT_TRUE(err_isOk(reader_takeSlice(&r, 4, &s)));
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

    ASSERT_TRUE(err_isOk(getToFormatChunk(&r)));

    Slice s;
    ASSERT_TRUE(err_isOk(reader_takeSlice(&r, 4, &s)));
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

    ASSERT_TRUE(err_isOk(getToFormatChunk(&r)));

    Slice s;
    ASSERT_TRUE(err_isOk(reader_takeSlice(&r, 4, &s)));
    EXPECT_EQ(std::string((char*)s.slice, s.len), "fmt ");
}

TEST(WavReader, GetToFormatChunk_TruncatedRiff)
{
    std::vector<Byte> data;
    data.insert(data.end(), {'R', 'I', 'F'});  // only 3 bytes, truncated

    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    Error err = getToFormatChunk(&r);
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(WavReader, GetToFormatChunk_NotWave)
{
    std::vector<Byte> data;
    data.insert(data.end(), {'R', 'I', 'F', 'F', 0x10, 0x00, 0x00, 0x00, 'N',
                             'O', 'P', 'E'});

    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    Error err = getToFormatChunk(&r);
    ASSERT_EQ(err_category(err), E_Wav);
    ASSERT_EQ(err_subCategory(err), EWav_UnknownFourCC);
}

TEST(WavReader, GetToFormatChunk_NoFmtChunk)
{
    std::vector<Byte> data;
    appendRiffWave(data, 12);
    appendChunk(data, "JUNK", 4, {0x00, 0x01, 0x02, 0x03});

    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    // Should fail because no fmt chunk exists
    Error err = getToFormatChunk(&r);
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

/*
struct WavFormatChunk {  // for reference
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
*/

void appendFmtChunk(std::vector<Byte>& buf,
                    uint16_t formatTag,
                    uint16_t nChannels,
                    uint32_t sampleRate,
                    uint16_t bitDepth,
                    uint16_t blockAlign,
                    uint16_t extensionSize = 0,
                    uint16_t validBitsPerSample = 0,
                    uint32_t channelMask = 0,
                    const uint8_t subFormat[16] = nullptr)
{
    std::vector<Byte> content;
    appendU16(content, formatTag);
    appendU16(content, nChannels);
    appendU32(content, sampleRate);
    uint32_t byteRate = sampleRate * nChannels * (bitDepth / 8);
    appendU32(content, byteRate);
    appendU16(content, blockAlign);
    appendU16(content, bitDepth);
    if (extensionSize > 0 || validBitsPerSample > 0 || channelMask > 0 ||
        subFormat != nullptr) {
        appendU16(content, extensionSize);
    }
    if (extensionSize >= 2 || validBitsPerSample > 0)
        appendU16(content, validBitsPerSample);
    if (extensionSize >= 4 || channelMask > 0)
        appendU32(content, channelMask);
    if (extensionSize >= 16 && subFormat != nullptr)
        content.insert(content.end(), subFormat, subFormat + 16);

    appendChunk(buf, "fmt ", content.size(), content);
}

TEST(WavReader, ReadFormatChunk_PCM16)
{
    std::vector<Byte> data;
    appendFmtChunk(data, 1, 2, 44100, 16, 4);
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    WavFormatChunk fmt;
    ASSERT_TRUE(err_isOk(readFormatChunk(&r, &fmt)));

    EXPECT_EQ(fmt.formatTag, 1u);
    EXPECT_EQ(fmt.nChannels, 2u);
    EXPECT_EQ(fmt.sampleRate, 44100u);
    EXPECT_EQ(fmt.bytesPerSecond, 44100u * 2 * 16 / 8);
    EXPECT_EQ(fmt.blockAlign, 4u);
    EXPECT_EQ(fmt.bitDepth, 16u);

    // verify zeroed fields
    EXPECT_EQ(fmt.extensionSize, 0u);
    EXPECT_EQ(fmt.validBitsPerSample, 0u);
    EXPECT_EQ(fmt.channelMask, 0u);
    for (int i = 0; i < 16; i++) {
        EXPECT_EQ(fmt.subFormat[i], 0u);
    }
}
TEST(WavReader, ReadFormatChunk_PCM24_Extended)
{
    std::vector<Byte> data;
    uint8_t subFmt[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                          0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    appendFmtChunk(data, 1, 1, 48000, 24, 3, 16, 24, 0x03, subFmt);
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    WavFormatChunk fmt;
    ASSERT_TRUE(err_isOk(readFormatChunk(&r, &fmt)));

    EXPECT_EQ(fmt.formatTag, 1u);
    EXPECT_EQ(fmt.nChannels, 1u);
    EXPECT_EQ(fmt.sampleRate, 48000u);
    EXPECT_EQ(fmt.bytesPerSecond, 48000u * 1 * 24 / 8);
    EXPECT_EQ(fmt.blockAlign, 3u);
    EXPECT_EQ(fmt.bitDepth, 24u);

    EXPECT_EQ(fmt.extensionSize, 16u);
    EXPECT_EQ(fmt.validBitsPerSample, 24u);
    EXPECT_EQ(fmt.channelMask, 0x03u);
    for (int i = 0; i < 16; i++)
        EXPECT_EQ(fmt.subFormat[i], subFmt[i]);
}

TEST(WavReader, ReadFormatChunk_Truncated)
{
    std::vector<Byte> data;
    std::vector<Byte> content = {0x01, 0x00, 0x02};  // too short
    appendChunk(data, "fmt ", 6, content);
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    WavFormatChunk fmt;
    Error err = readFormatChunk(&r, &fmt);
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(WavReader, ReadFormatChunk_EmptyReader)
{
    std::vector<Byte> data;  // empty
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);
    WavFormatChunk fmt;
    Error err = readFormatChunk(&r, &fmt);
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(WavReader, ReadFormatChunk_CheckZeroingBeyondChunk)
{
    std::vector<Byte> data;
    appendFmtChunk(data, 1, 2, 44100, 16, 4);  // minimal 16 bytes
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    WavFormatChunk fmt;
    fmt.extensionSize = 0xFFFF;
    fmt.validBitsPerSample = 0xFFFF;
    fmt.channelMask = 0xFFFFFFFF;
    memset(fmt.subFormat, 0xFF, 16);

    ASSERT_TRUE(err_isOk(readFormatChunk(&r, &fmt)));

    // fields beyond 16 bytes should be zero
    EXPECT_EQ(fmt.extensionSize, 0u);
    EXPECT_EQ(fmt.validBitsPerSample, 0u);
    EXPECT_EQ(fmt.channelMask, 0u);
    for (int i = 0; i < 16; i++)
        EXPECT_EQ(fmt.subFormat[i], 0u);
}

TEST(WavReader, ReadFormatChunk_PCM16Stereo)
{
    std::vector<Byte> data;
    appendFmtChunk(data, WAVE_FORMAT_PCM, 2, 44100, 16, 4);
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    WavFormatChunk fmt{};
    ASSERT_TRUE(err_isOk(readFormatChunk(&r, &fmt)));
    EXPECT_EQ(fmt.formatTag, WAVE_FORMAT_PCM);
    EXPECT_EQ(fmt.nChannels, 2u);
    EXPECT_EQ(fmt.sampleRate, 44100u);
    EXPECT_EQ(fmt.bytesPerSecond, 44100u * 2 * 16 / 8);
    EXPECT_EQ(fmt.blockAlign, 4u);
    EXPECT_EQ(fmt.bitDepth, 16u);
}

TEST(WavReader, ReadFormatChunk_IEEEFloatStereo)
{
    std::vector<Byte> data;
    appendFmtChunk(data, WAVE_FORMAT_IEEE_FLOAT, 2, 48000, 32, 8);
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    WavFormatChunk fmt{};
    ASSERT_TRUE(err_isOk(readFormatChunk(&r, &fmt)));
    EXPECT_EQ(fmt.formatTag, WAVE_FORMAT_IEEE_FLOAT);
    EXPECT_EQ(fmt.nChannels, 2u);
    EXPECT_EQ(fmt.sampleRate, 48000u);
    EXPECT_EQ(fmt.bytesPerSecond, 48000u * 2 * 32 / 8);
    EXPECT_EQ(fmt.blockAlign, 8u);
    EXPECT_EQ(fmt.bitDepth, 32u);
}

TEST(WavReader, ReadFormatChunk_ALAWMono)
{
    std::vector<Byte> data;
    appendFmtChunk(data, WAVE_FORMAT_ALAW, 1, 44100, 8, 1);
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    WavFormatChunk fmt{};
    ASSERT_TRUE(err_isOk(readFormatChunk(&r, &fmt)));
    EXPECT_EQ(fmt.formatTag, WAVE_FORMAT_ALAW);
    EXPECT_EQ(fmt.nChannels, 1u);
    EXPECT_EQ(fmt.sampleRate, 44100u);
    EXPECT_EQ(fmt.bytesPerSecond, 44100u * 1 * 8 / 8);
    EXPECT_EQ(fmt.blockAlign, 1u);
    EXPECT_EQ(fmt.bitDepth, 8u);
}

TEST(WavReader, ReadFormatChunk_MULAWMono)
{
    std::vector<Byte> data;
    appendFmtChunk(data, WAVE_FORMAT_MULAW, 1, 44100, 8, 1);
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    WavFormatChunk fmt{};
    ASSERT_TRUE(err_isOk(readFormatChunk(&r, &fmt)));
    EXPECT_EQ(fmt.formatTag, WAVE_FORMAT_MULAW);
    EXPECT_EQ(fmt.nChannels, 1u);
    EXPECT_EQ(fmt.sampleRate, 44100u);
    EXPECT_EQ(fmt.bytesPerSecond, 44100u * 1 * 8 / 8);
    EXPECT_EQ(fmt.blockAlign, 1u);
    EXPECT_EQ(fmt.bitDepth, 8u);
}

TEST(WavReader, ReadFormatChunk_ExtensibleStereo)
{
    std::vector<Byte> data;
    uint8_t subFmt[16] = {0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00,
                          0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71, 0x00, 0x00};
    appendFmtChunk(data, WAVE_FORMAT_EXTENSIBLE, 2, 44100, 16, 4, 22, 16, 3,
                   subFmt);
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    WavFormatChunk fmt{};
    ASSERT_TRUE(err_isOk(readFormatChunk(&r, &fmt)));
    EXPECT_EQ(fmt.formatTag, WAVE_FORMAT_EXTENSIBLE);
    EXPECT_EQ(fmt.nChannels, 2u);
    EXPECT_EQ(fmt.sampleRate, 44100u);
    EXPECT_EQ(fmt.bytesPerSecond, 44100u * 2 * 16 / 8);
    EXPECT_EQ(fmt.blockAlign, 4u);
    EXPECT_EQ(fmt.bitDepth, 16u);
    EXPECT_EQ(fmt.extensionSize, 22u);
    EXPECT_EQ(fmt.validBitsPerSample, 16u);
    EXPECT_EQ(fmt.channelMask, 3u);
    for (int i = 0; i < 16; i++)
        EXPECT_EQ(fmt.subFormat[i], subFmt[i]);
}
