#include <cstdint>
#include <format>
#include <fstream>

#include "gtest/gtest.h"

extern "C" {
#include "FileReader.h"
#include "unistd.h"
}

namespace {
static constexpr std::size_t buffer_size = FILE_READER_BUFFER_SIZE;

void writeData(int fd, const std::vector<uint8_t>& data)
{
    const uint8_t* bytes = data.data();
    const size_t size = data.size();
    write(fd, bytes, size);
}
std::string writeTempFile(const std::vector<uint8_t>& data)
{
    char path[] = "/tmp/fr_testXXXXXX";
    int fd = mkstemp(path);
    if (fd < 0) {
        perror("mkstemp");
        abort();
    }
    writeData(fd, data);
    close(fd);
    return path;
}

[[maybe_unused]] void logReader(const FileReader& r)
{
    std::cerr << std::format("FileReader {{\n");
    std::cerr << std::format("\tfd:\t{}\n", r.fd);
    std::cerr << std::format("\thead:\t{}\n", r.head);
    std::cerr << std::format("\tlen:\t{}\n", r.len);

    std::cerr << "\t\"";
    for (size_t i = 0; i < r.len; ++i) {
        std::cerr << std::format("{:02x}", r.buffer[i]);
    }
    std::cerr << std::format("\"\n");
    std::cerr << std::format("}}\n");
}
}  // namespace

TEST(Read, ReadBytes)
{
    const std::vector<uint8_t> data = {1, 4, 9, 240, 42, 67};
    const std::string path = writeTempFile(data);

    FileReader r = fr_open(path.c_str());
    if (!fr_isOpened(&r)) {
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
    const std::string path = writeTempFile(data);

    FileReader r = fr_open(path.c_str());
    if (!fr_isOpened(&r)) {
        FAIL() << std::format("Failed to open file {}", path);
    }

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
    const std::string path = writeTempFile(data);

    FileReader r = fr_open(path.c_str());
    if (!fr_isOpened(&r)) {
        FAIL() << std::format("Failed to open file {}", path);
    }

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
    const std::vector<uint16_t> expectedValues = {0x100};

    const std::string path = writeTempFile(data);

    FileReader r = fr_open(path.c_str());
    if (!fr_isOpened(&r)) {
        FAIL() << std::format("Failed to open file {}", path);
    }

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
    const std::string path = writeTempFile(data);

    FileReader r = fr_open(path.c_str());
    if (!fr_isOpened(&r)) {
        FAIL() << std::format("Failed to open file {}", path);
    }

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

TEST(Read, TakeU16_PastBuffer)
{
    std::vector<uint8_t> data;
    for (std::size_t i = 0; i < buffer_size - 1; ++i) {
        data.push_back(67);
    }
    data.push_back(0x34);
    data.push_back(0x12);

    const std::string path = writeTempFile(data);

    FileReader r = fr_open(path.c_str());
    if (!fr_isOpened(&r)) {
        FAIL() << std::format("Failed to open file {}", path);
    }

    for (std::size_t i = 0; i < buffer_size - 1; ++i) {
        uint8_t byte;
        const auto res = fr_takeByte(&r, &byte);
        ASSERT_EQ(res, Read_Ok);
        ASSERT_EQ(byte, 67);
    }
    ASSERT_EQ(r.head, buffer_size - 1);

    const uint16_t expected = 0x1234;
    uint16_t actual;

    ASSERT_EQ(fr_peekU16LE(&r, &actual), Read_Ok);
    ASSERT_EQ(actual, expected);

    ASSERT_EQ(fr_takeU16LE(&r, &actual), Read_Ok);
    ASSERT_EQ(actual, expected);

    uint8_t byte;
    ASSERT_NE(fr_peekByte(&r, &byte), Read_Ok);

    fr_close(&r);
}
