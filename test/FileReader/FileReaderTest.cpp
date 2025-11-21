#include <cstdint>
#include <format>

#include "gtest/gtest.h"

extern "C" {
#include "FileReader.h"
#include "unistd.h"
}

namespace {
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

TEST(FileReader, OpenClose)
{
    auto file = writeTempFile({1, 2, 3});
    FileReader fr = fr_open(file.c_str());
    ASSERT_TRUE(fr_isOpened(&fr));
    fr_close(&fr);
    ASSERT_FALSE(fr_isOpened(&fr));
}

// peek doesn't advance, take does
TEST(FileReader, PeekAndTakeByte)
{
    auto file = writeTempFile({10, 20, 30});
    FileReader fr = fr_open(file.c_str());

    uint8_t b;

    // Peek first byte
    EXPECT_EQ(fr_peekByte(&fr, &b), Read_Ok);
    EXPECT_EQ(b, 10);

    // Peek again: should still be 10
    b = 0;
    EXPECT_EQ(fr_peekByte(&fr, &b), Read_Ok);
    EXPECT_EQ(b, 10);

    // Take now
    b = 0;
    EXPECT_EQ(fr_takeByte(&fr, &b), Read_Ok);
    EXPECT_EQ(b, 10);

    // Next byte is 20
    b = 0;
    EXPECT_EQ(fr_takeByte(&fr, &b), Read_Ok);
    EXPECT_EQ(b, 20);

    // Next is 30
    b = 0;
    EXPECT_EQ(fr_takeByte(&fr, &b), Read_Ok);
    EXPECT_EQ(b, 30);

    // Now EOF
    EXPECT_EQ(fr_peekByte(&fr, &b), Read_Done);
    EXPECT_EQ(fr_takeByte(&fr, &b), Read_Done);

    fr_close(&fr);
}

TEST(FileReader, PeekAndTakeSlice)
{
    auto file = writeTempFile({1, 2, 3, 4});
    FileReader fr = fr_open(file.c_str());

    uint8_t out[4];

    // Peek first 3 bytes
    EXPECT_EQ(fr_peekSlice(&fr, out, 3), Read_Ok);
    EXPECT_EQ(out[0], 1);
    EXPECT_EQ(out[1], 2);
    EXPECT_EQ(out[2], 3);

    // Peek again — still same
    memset(out, 0, 4);
    EXPECT_EQ(fr_peekSlice(&fr, out, 3), Read_Ok);
    EXPECT_EQ(out[0], 1);
    EXPECT_EQ(out[1], 2);
    EXPECT_EQ(out[2], 3);

    // Take 3 bytes
    memset(out, 0, 4);
    EXPECT_EQ(fr_takeSlice(&fr, out, 3), Read_Ok);
    EXPECT_EQ(out[0], 1);
    EXPECT_EQ(out[1], 2);
    EXPECT_EQ(out[2], 3);

    // 1 byte left
    uint8_t b = 0;
    EXPECT_EQ(fr_takeByte(&fr, &b), Read_Ok);
    EXPECT_EQ(b, 4);

    // EOF
    EXPECT_EQ(fr_peekSlice(&fr, out, 1), Read_Done);
    EXPECT_EQ(fr_takeSlice(&fr, out, 1), Read_Done);

    fr_close(&fr);
}

// partial slice does not advance
TEST(FileReader, SlicePartialReadFailsAndDoesNotAdvance)
{
    auto file = writeTempFile({9, 8});
    FileReader fr = fr_open(file.c_str());

    uint8_t out[4];

    // Try reading 3 bytes but file has only 2
    memset(out, 0, 4);
    EXPECT_EQ(fr_peekSlice(&fr, out, 3), Read_Err);

    // Ensure buffer hasn't advanced: peek a byte -> should be 9
    uint8_t b = 0;
    EXPECT_EQ(fr_peekByte(&fr, &b), Read_Ok);
    EXPECT_EQ(b, 9);

    // take a byte, ensure it's still 9
    b = 0;
    EXPECT_EQ(fr_takeByte(&fr, &b), Read_Ok);
    EXPECT_EQ(b, 9);

    // now only 1 byte remains
    b = 0;
    EXPECT_EQ(fr_takeByte(&fr, &b), Read_Ok);
    EXPECT_EQ(b, 8);

    // EOF
    EXPECT_EQ(fr_peekSlice(&fr, out, 1), Read_Done);

    fr_close(&fr);
}

// across buffer
TEST(FileReader, CrossesBufferBoundary)
{
    std::vector<uint8_t> data(FILE_READER_BUFFER_SIZE + 5);
    for (size_t i = 0; i < data.size(); ++i)
        data[i] = uint8_t(i & 0xFF);

    auto file = writeTempFile(data);
    FileReader fr = fr_open(file.c_str());

    uint8_t out[16];

    // take a slice that ends exactly at interior boundary
    EXPECT_EQ(fr_takeSlice(&fr, out, FILE_READER_BUFFER_SIZE - 2), Read_Ok);

    // next slice crosses buffer boundary
    memset(out, 0, 16);
    EXPECT_EQ(fr_takeSlice(&fr, out, 6), Read_Ok);
    EXPECT_EQ(out[0], data[FILE_READER_BUFFER_SIZE - 2]);
    EXPECT_EQ(out[5], data[FILE_READER_BUFFER_SIZE + 3]);

    fr_close(&fr);
}
