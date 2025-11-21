#include <cstdint>
#include <format>
#include <fstream>

#include "gtest/gtest.h"

extern "C" {
#include "FileReader.h"
}

const std::string prefix = "../build/test/";
const std::string suffix = ".dat";

void writeFile(const std::string& path, const std::vector<uint8_t>& data)
{
    std::ofstream os(path);
    for (const uint8_t byte : data) {
        os << byte;
    }
}

TEST(Read, ReadBytes)
{
    const std::vector<uint8_t> data = {1, 4, 9, 240, 42, 67};
    const std::string name = "takeByte";

    const std::string path = prefix + name + suffix;
    writeFile(path, data);

    FileReader r = fr_open(path.c_str());
    if (r.fd == -1) {
        FAIL() << std::format("Failed to open file {}", path);
    }

    uint8_t byte;
    for (const uint8_t expected : data) {
        ASSERT_EQ(fr_takeByte(&r, &byte), Read_Ok);
        ASSERT_EQ(byte, expected);
    }
    ASSERT_EQ(fr_takeByte(&r, &byte), Read_Done);

    fr_close(&r);
}

TEST(Read, PeekByteDoesntAdvance)
{
    const std::vector<uint8_t> data = {0, 1};
    const std::string name = "peekByte";

    const std::string path = prefix + name + suffix;
    writeFile(path, data);

    FileReader r = fr_open(path.c_str());

    constexpr int nPeeks = 67;
    uint8_t byte;
    for (int _ = 0; _ < nPeeks; ++_) {
        ASSERT_EQ(fr_peekByte(&r, &byte), Read_Ok);
        ASSERT_EQ(byte, 0);
    }

    ASSERT_EQ(fr_takeByte(&r, &byte), Read_Ok);
    ASSERT_EQ(byte, 0);

    for (int _ = 0; _ < nPeeks; ++_) {
        ASSERT_EQ(fr_peekByte(&r, &byte), Read_Ok);
        ASSERT_EQ(byte, 1);
    }

    ASSERT_EQ(fr_takeByte(&r, &byte), Read_Ok);
    ASSERT_EQ(byte, 1);

    ASSERT_EQ(fr_takeByte(&r, &byte), Read_Done);

    fr_close(&r);
}

TEST(Read, TakeU16Basic)
{
    const std::vector<uint8_t> data = {42, 0};
    const std::string name = "takeU16_basic";

    const std::string path = prefix + name + suffix;
    writeFile(path, data);

    FileReader r = fr_open(path.c_str());

    uint16_t actual;
    constexpr uint16_t expected = 42;
    ASSERT_EQ(fr_takeU16LE(&r, &actual), Read_Ok);
    ASSERT_EQ(actual, expected);
    ASSERT_EQ(r.head, r.len);

    ASSERT_NE(fr_takeU16LE(&r, &actual), Read_Ok);

    fr_close(&r);
}

TEST(Read, TakeU16)
{
    const std::vector<uint8_t> data = {0, 1};
    const std::vector<uint16_t> expectedValues = {16};
    const std::string name = "takeU16";

    const std::string path = prefix + name + suffix;
    writeFile(path, data);

    FileReader r = fr_open(path.c_str());

    uint16_t actual;
    for (uint16_t expected : expectedValues) {
        ASSERT_EQ(fr_takeU16LE(&r, &actual), Read_Ok);
        ASSERT_EQ(actual, expected);
    }

    ASSERT_NE(fr_takeU16LE(&r, &actual), Read_Ok);

    fr_close(&r);
}

TEST(Read, TakeU16_DoesntAdvanceOnReadError)
{
    const std::vector<uint8_t> data = {67};
    const std::string name = "takeU16_NotEnough";

    const std::string path = prefix + name + suffix;
    writeFile(path, data);

    FileReader r = fr_open(path.c_str());

    uint16_t actual;
    ASSERT_EQ(fr_takeU16LE(&r, &actual), Read_Err);
    ASSERT_EQ(r.head, static_cast<std::size_t>(0));
    ASSERT_EQ(fr_takeU16LE(&r, &actual), Read_Err);
    ASSERT_EQ(fr_takeU16LE(&r, &actual), Read_Err);
    ASSERT_EQ(fr_takeU16LE(&r, &actual), Read_Err);
    ASSERT_EQ(fr_takeU16LE(&r, &actual), Read_Err);
    ASSERT_EQ(fr_takeU16LE(&r, &actual), Read_Err);
    ASSERT_EQ(r.head, static_cast<std::size_t>(0));

    fr_close(&r);
}
