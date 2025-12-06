#include <cstdint>
#include <string>

#include "Error.h"
#include "gtest/gtest.h"

extern "C" {
#include "FileReader.h"
#include "Reader.h"
#include "ReaderAdapters.h"
}

struct TmpFileReader {
    FileReader reader;

    TmpFileReader(const std::string& data)
    {
        char path[] = "/tmp/fr_testXXXXXX";
        int fd = mkstemp(path);
        if (fd < 0) {
            perror("mkstemp");
            abort();
        }
        write(fd, data.c_str(), data.size());
        close(fd);

        this->reader = fr_open(path);
        assert(fr_isOpened(&this->reader));
    }

    ~TmpFileReader() { fr_close(&this->reader); }
};

static void assertSliceEq(const Slice& slice, const std::string& expected)
{
    std::string actual(slice.slice, slice.slice + slice.len);
    ASSERT_EQ(actual, expected);
}

TEST(FileReaderReader, PeekSlice_Basic)
{
    const std::string data = "1234567";
    TmpFileReader fileReader(data);
    Reader reader = reader_fromFileReader(&fileReader.reader);

    Slice slice;

    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 1, &slice)));
    assertSliceEq(slice, "1");

    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 2, &slice)));
    assertSliceEq(slice, "12");

    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 3, &slice)));
    assertSliceEq(slice, "123");

    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 4, &slice)));
    assertSliceEq(slice, "1234");

    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 5, &slice)));
    assertSliceEq(slice, "12345");

    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 6, &slice)));
    assertSliceEq(slice, "123456");

    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 7, &slice)));
    assertSliceEq(slice, "1234567");

    Error err = reader.peekSlice(&reader, 8, &slice);
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(FileReaderReader, PeekInto_Basic)
{
    const std::string data = "abcdefg";
    TmpFileReader fileReader(data);
    Reader reader = reader_fromFileReader(&fileReader.reader);

    uint8_t buf[16] = {};

    ASSERT_TRUE(err_isOk(reader.peekInto(&reader, 3, buf)));
    ASSERT_EQ(memcmp(buf, "abc", 3), 0);

    ASSERT_TRUE(err_isOk(reader.peekInto(&reader, 7, buf)));
    ASSERT_EQ(memcmp(buf, "abcdefg", 7), 0);

    Error err = reader.peekInto(&reader, 8, buf);
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(FileReaderReader, PeekInto_DoesNotAdvance)
{
    const std::string data = "hello";
    TmpFileReader fileReader(data);
    Reader reader = reader_fromFileReader(&fileReader.reader);

    uint8_t buf[8] = {};
    ASSERT_TRUE(err_isOk(reader.peekInto(&reader, 5, buf)));
    ASSERT_EQ(memcmp(buf, "hello", 5), 0);

    // peek again, expecting same
    memset(buf, 0, sizeof(buf));
    ASSERT_TRUE(err_isOk(reader.peekInto(&reader, 5, buf)));
    ASSERT_EQ(memcmp(buf, "hello", 5), 0);

    // offset must not have changed
    ASSERT_EQ(reader.offset, 0u);
}

TEST(FileReaderReader, Skip_Basic)
{
    const std::string data = "ABCDEFG";
    TmpFileReader fileReader(data);
    Reader reader = reader_fromFileReader(&fileReader.reader);

    ASSERT_TRUE(err_isOk(reader.skip(&reader, 1)));
    ASSERT_EQ(reader.offset, 1u);

    ASSERT_TRUE(err_isOk(reader.skip(&reader, 2)));
    ASSERT_EQ(reader.offset, 3u);

    ASSERT_TRUE(err_isOk(reader.skip(&reader, 3)));
    ASSERT_EQ(reader.offset, 6u);

    ASSERT_TRUE(err_isOk(reader.skip(&reader, 1)));
    ASSERT_EQ(reader.offset, 7u);

    Error err = reader.skip(&reader, 1);
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(FileReaderReader, Skip_Then_Peek)
{
    const std::string data = "0123456789";
    TmpFileReader fileReader(data);
    Reader reader = reader_fromFileReader(&fileReader.reader);

    ASSERT_TRUE(err_isOk(reader.skip(&reader, 3)));
    ASSERT_EQ(reader.offset, 3u);

    Slice slice;
    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 4, &slice)));
    assertSliceEq(slice, "3456");

    ASSERT_TRUE(err_isOk(reader.skip(&reader, 4)));
    ASSERT_EQ(reader.offset, 7u);

    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 3, &slice)));
    assertSliceEq(slice, "789");

    Error err = reader.peekSlice(&reader, 4, &slice);
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(FileReaderReader, ZeroLength_Peeks)
{
    const std::string data = "xyz";
    TmpFileReader fileReader(data);
    Reader reader = reader_fromFileReader(&fileReader.reader);

    Slice slice;
    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 0, &slice)));
    ASSERT_EQ(slice.len, 0u);

    uint8_t buf[1];
    ASSERT_TRUE(err_isOk(reader.peekInto(&reader, 0, buf)));
}

TEST(FileReaderReader, EOF_PeekSlice_Exact)
{
    const std::string data = "hi";
    TmpFileReader fileReader(data);
    Reader reader = reader_fromFileReader(&fileReader.reader);

    Slice slice;
    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 2, &slice)));
    assertSliceEq(slice, "hi");

    Error err = reader.peekSlice(&reader, 3, &slice);
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(FileReaderReader, EOF_PeekInto_Exact)
{
    const std::string data = "xy";
    TmpFileReader fileReader(data);
    Reader reader = reader_fromFileReader(&fileReader.reader);

    uint8_t buf[4];

    ASSERT_TRUE(err_isOk(reader.peekInto(&reader, 2, buf)));
    ASSERT_EQ(memcmp(buf, "xy", 2), 0);

    Error err = reader.peekInto(&reader, 3, buf);
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(FileReaderReader, PeekSlice_AfterEOFOffset)
{
    const std::string data = "aaa";
    TmpFileReader fileReader(data);
    Reader reader = reader_fromFileReader(&fileReader.reader);

    ASSERT_TRUE(err_isOk(reader.skip(&reader, 3)));
    ASSERT_EQ(reader.offset, 3u);

    Slice slice;
    Error err = reader.peekSlice(&reader, 1, &slice);
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(FileReaderReader, MixedOperations_Consistency)
{
    const std::string data = "abcdefghij";
    TmpFileReader fileReader(data);
    Reader reader = reader_fromFileReader(&fileReader.reader);

    Slice slice;

    // Peek first 5
    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 5, &slice)));
    assertSliceEq(slice, "abcde");

    // Skip 2
    ASSERT_TRUE(err_isOk(reader.skip(&reader, 2)));
    ASSERT_EQ(reader.offset, 2u);

    // Peek 4 starting at offset 2
    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 4, &slice)));
    assertSliceEq(slice, "cdef");

    // Skip to near end
    ASSERT_TRUE(err_isOk(reader.skip(&reader, 6)));
    ASSERT_EQ(reader.offset, 8u);

    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 2, &slice)));
    assertSliceEq(slice, "ij");

    Error err = reader.peekSlice(&reader, 3, &slice);
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(FileReaderReader, PeekSlice_SingleByteAdvances)
{
    const std::string data = "ABCDEFGH";
    TmpFileReader fileReader(data);
    Reader r = reader_fromFileReader(&fileReader.reader);

    Slice s;
    for (size_t i = 0; i < 8; i++) {
        ASSERT_TRUE(err_isOk(r.peekSlice(&r, 1, &s)));
        ASSERT_EQ(s.len, 1u);
        ASSERT_EQ((char)s.slice[0], char('A' + i));

        ASSERT_TRUE(err_isOk(r.skip(&r, 1)));
        ASSERT_EQ(r.offset, i + 1);
    }

    // now at EOF
    Error err = r.peekSlice(&r, 1, &s);
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(FileReaderReader, OffsetMonotonicity_PeekDoesNotAdvance)
{
    const std::string data = "0123456789";
    TmpFileReader fileReader(data);
    Reader r = reader_fromFileReader(&fileReader.reader);

    for (int i = 0; i < 20; i++) {
        Slice s;
        size_t before = r.offset;

        // varying peek sizes
        size_t n = (i % 11);
        Error err = r.peekSlice(&r, n, &s);

        if (n <= 10) {
            if (n <= 10 - before) {
                ASSERT_TRUE(err_isOk(err));
            } else {
                ASSERT_EQ(err_category(err), E_Read);
                ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
            }
        }

        ASSERT_EQ(r.offset, before) << "peek must never advance offset";
    }
}

TEST(FileReaderReader, RepeatedEOFPeek)
{
    const std::string data = "xyz";
    TmpFileReader fileReader(data);
    Reader r = reader_fromFileReader(&fileReader.reader);

    ASSERT_TRUE(err_isOk(r.skip(&r, 3)));
    ASSERT_EQ(r.offset, 3u);

    for (int i = 0; i < 10; i++) {
        Slice s;
        Error err = r.peekSlice(&r, 1, &s);
        ASSERT_EQ(err_category(err), E_Read);
        ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
        ASSERT_EQ(r.offset, 3u);
    }
}

TEST(FileReaderReader, ExhaustiveByteWalk)
{
    const std::string data = "abcdefghijklmnop";
    TmpFileReader fileReader(data);
    Reader r = reader_fromFileReader(&fileReader.reader);

    uint8_t buf[32];

    for (size_t i = 0; i < data.size(); i++) {
        // peek next byte
        ASSERT_TRUE(err_isOk(r.peekInto(&r, 1, buf)));
        ASSERT_EQ(buf[0], (uint8_t)data[i]);

        // now skip it
        ASSERT_TRUE(err_isOk(r.skip(&r, 1)));
        ASSERT_EQ(r.offset, i + 1);
    }

    // exhausted
    Error err = r.peekInto(&r, 1, buf);
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(FileReaderReader, SlidingWindows)
{
    const std::string data = "0123456789";
    TmpFileReader fileReader(data);
    Reader r = reader_fromFileReader(&fileReader.reader);

    Slice s;

    // at each offset j, try peeking windows of size k
    for (size_t j = 0; j < 10; j++) {
        ASSERT_TRUE(err_isOk(r.skip(&r, j - r.offset)));

        for (size_t k = 0; k <= 10; k++) {
            size_t remaining = 10 - j;

            Error err = r.peekSlice(&r, k, &s);

            if (k <= remaining) {
                ASSERT_TRUE(err_isOk(err));
                ASSERT_EQ(s.len, k);
                ASSERT_EQ(memcmp(s.slice, data.c_str() + j, k), 0);
            } else {
                ASSERT_EQ(err_category(err), E_Read);
                ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
            }
        }
    }
}

TEST(FileReaderReader, SliceStabilityAcrossPeeks)
{
    const std::string data = "ABCDE";
    TmpFileReader fileReader(data);
    Reader r = reader_fromFileReader(&fileReader.reader);

    Slice s1, s2;

    ASSERT_TRUE(err_isOk(r.peekSlice(&r, 3, &s1)));
    ASSERT_EQ(memcmp(s1.slice, "ABC", 3), 0);

    ASSERT_TRUE(err_isOk(r.peekSlice(&r, 5, &s2)));
    ASSERT_EQ(memcmp(s2.slice, "ABCDE", 5), 0);

    // original slice must still match "ABC"
    ASSERT_EQ(memcmp(s1.slice, "ABC", 3), 0);

    ASSERT_EQ(r.offset, 0u);
}

TEST(FileReaderReader, SkipToLastByte)
{
    const std::string data = "Z123";
    TmpFileReader fileReader(data);
    Reader r = reader_fromFileReader(&fileReader.reader);

    ASSERT_TRUE(err_isOk(r.skip(&r, 3)));
    ASSERT_EQ(r.offset, 3u);

    Slice s;
    ASSERT_TRUE(err_isOk(r.peekSlice(&r, 1, &s)));
    ASSERT_EQ((char)s.slice[0], '3');

    Error err = r.peekSlice(&r, 2, &s);
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(FileReaderReader, SkipExhaustively)
{
    const std::string data = "aaa";
    TmpFileReader fileReader(data);
    Reader r = reader_fromFileReader(&fileReader.reader);

    ASSERT_TRUE(err_isOk(r.skip(&r, 3)));
    ASSERT_EQ(r.offset, 3u);

    for (int i = 0; i < 5; i++) {
        Error err = r.skip(&r, 1);
        ASSERT_EQ(err_category(err), E_Read);
        ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
    }
}

TEST(FileReaderReader, PeekInto_Substrings)
{
    const std::string data = "HelloWorld";
    TmpFileReader fileReader(data);
    Reader r = reader_fromFileReader(&fileReader.reader);

    uint8_t buf[64];

    for (size_t n = 0; n <= data.size(); n++) {
        Error e = r.peekInto(&r, n, buf);
        ASSERT_TRUE(err_isOk(e));
        ASSERT_EQ(memcmp(buf, data.c_str(), n), 0);
    }

    Error err = r.peekInto(&r, data.size() + 1, buf);
    ASSERT_EQ(err_category(err), E_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(FileReaderReader, ComplexSkipPattern)
{
    const std::string data = "abcdefghijklmnopqrstuv";
    TmpFileReader fileReader(data);
    Reader r = reader_fromFileReader(&fileReader.reader);

    size_t total = data.size();

    size_t steps[] = {1, 2, 3, 5, 8, 3, 2, 1};

    size_t expectedOffset = 0;
    for (size_t s : steps) {
        if (expectedOffset + s <= total) {
            ASSERT_TRUE(err_isOk(r.skip(&r, s)));
            expectedOffset += s;
        } else {
            Error err = r.skip(&r, s);
            ASSERT_EQ(err_category(err), E_Read);
            ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
        }

        ASSERT_EQ(r.offset, expectedOffset);
    }
}
