#include <cstdint>
#include <fstream>

#include "gtest/gtest.h"

extern "C" {
#include "FileReader.h"
}

const std::string prefix = "../build/test/";

void writeFile(const std::string& path, const std::vector<uint8_t>& data)
{
    std::ofstream os(path);
    for (uint8_t byte : data) {
        os << byte;
    }
}

TEST(Read, ReadBytes)
{
    const std::vector<uint8_t> data = {1, 4, 9, 240, 42, 67};
    const std::string name = "takeByte";

    const std::string path = prefix + name;
    writeFile(path, data);

    FileReader r = fr_new(path.c_str());

    uint8_t byte;

    for (uint8_t expected : data) {
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

    const std::string path = prefix + name;
    writeFile(path, data);

    FileReader r = fr_new(path.c_str());

    uint8_t byte;
    int nPeeks = 67;

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

TEST(Read, PeekU16Basic)
{
    const std::vector<uint8_t> data = {42, 0};
    const std::string name = "peekU16";

    const std::string path = prefix + name;
    writeFile(path, data);

    FileReader r = fr_new(path.c_str());

    uint16_t nibble;
    uint16_t expected = 42;
    ASSERT_EQ(fr_peekU16LE(&r, &nibble), Read_Ok);
    ASSERT_EQ(nibble, expected);
    ASSERT_EQ(r.head, static_cast<std::size_t>(0));

    fr_close(&r);
}

TEST(Read, PeekU16_DoesntAdvanceOnReadError)
{
    const std::vector<uint8_t> data = {67};
    const std::string name = "peekU16_NotEnough";

    const std::string path = prefix + name;
    writeFile(path, data);

    FileReader r = fr_new(path.c_str());

    uint16_t nibble;
    ASSERT_EQ(fr_peekU16LE(&r, &nibble), Read_Err);
    ASSERT_EQ(r.head, static_cast<std::size_t>(0));

    fr_close(&r);
}
