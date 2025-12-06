#include <cstdint>
#include <string>

#include "gtest/gtest.h"

#include "MemoryReader.hpp"

extern "C" {
#include "Reader.h"
}

static void assertSliceEq(const Slice& slice, const std::string& expected)
{
    std::string actual(slice.slice, slice.slice + slice.len);
    ASSERT_EQ(actual, expected);
}

TEST(MemoryReaderReader, PeekSlice_Basic)
{
    MemoryReader memoryReader("1234567");
    Reader reader = memoryReaderInterface(&memoryReader);

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

    Error64 err = reader.peekSlice(&reader, 8, &slice);
    ASSERT_EQ(err_category(err), E64_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(MemoryReaderReader, PeekInto_Basic)
{
    MemoryReader memoryReader("abcdefg");
    Reader reader = memoryReaderInterface(&memoryReader);

    uint8_t buf[16] = {};

    ASSERT_TRUE(err_isOk(reader.peekInto(&reader, 3, buf)));
    ASSERT_EQ(memcmp(buf, "abc", 3), 0);

    ASSERT_TRUE(err_isOk(reader.peekInto(&reader, 7, buf)));
    ASSERT_EQ(memcmp(buf, "abcdefg", 7), 0);

    Error64 err = reader.peekInto(&reader, 8, buf);
    ASSERT_EQ(err_category(err), E64_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(MemoryReaderReader, PeekInto_DoesNotAdvance)
{
    MemoryReader memoryReader("hello");
    Reader reader = memoryReaderInterface(&memoryReader);

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

TEST(MemoryReaderReader, Skip_Basic)
{
    MemoryReader memoryReader("ABCDEFG");
    Reader reader = memoryReaderInterface(&memoryReader);

    ASSERT_TRUE(err_isOk(reader.skip(&reader, 1)));
    ASSERT_EQ(reader.offset, 1u);

    ASSERT_TRUE(err_isOk(reader.skip(&reader, 2)));
    ASSERT_EQ(reader.offset, 3u);

    ASSERT_TRUE(err_isOk(reader.skip(&reader, 3)));
    ASSERT_EQ(reader.offset, 6u);

    ASSERT_TRUE(err_isOk(reader.skip(&reader, 1)));
    ASSERT_EQ(reader.offset, 7u);

    Error64 err = reader.skip(&reader, 1);
    ASSERT_EQ(err_category(err), E64_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(MemoryReaderReader, Skip_Then_Peek)
{
    MemoryReader memoryReader("0123456789");
    Reader reader = memoryReaderInterface(&memoryReader);

    ASSERT_TRUE(err_isOk(reader.skip(&reader, 3)));
    ASSERT_EQ(reader.offset, 3u);

    Slice slice;
    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 4, &slice)));
    assertSliceEq(slice, "3456");

    ASSERT_TRUE(err_isOk(reader.skip(&reader, 4)));
    ASSERT_EQ(reader.offset, 7u);

    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 3, &slice)));
    assertSliceEq(slice, "789");

    Error64 err = reader.peekSlice(&reader, 4, &slice);
    ASSERT_EQ(err_category(err), E64_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(MemoryReaderReader, ZeroLength_Peeks)
{
    MemoryReader memoryReader("xyz");
    Reader reader = memoryReaderInterface(&memoryReader);

    Slice slice;
    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 0, &slice)));
    ASSERT_EQ(slice.len, 0u);

    uint8_t buf[1];
    ASSERT_TRUE(err_isOk(reader.peekInto(&reader, 0, buf)));
}

TEST(MemoryReaderReader, EOF_PeekSlice_Exact)
{
    MemoryReader memoryReader("hi");
    Reader reader = memoryReaderInterface(&memoryReader);

    Slice slice;
    ASSERT_TRUE(err_isOk(reader.peekSlice(&reader, 2, &slice)));
    assertSliceEq(slice, "hi");

    Error64 err = reader.peekSlice(&reader, 3, &slice);
    ASSERT_EQ(err_category(err), E64_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(MemoryReaderReader, EOF_PeekInto_Exact)
{
    MemoryReader memoryReader("xy");
    Reader reader = memoryReaderInterface(&memoryReader);

    uint8_t buf[4];

    ASSERT_TRUE(err_isOk(reader.peekInto(&reader, 2, buf)));
    ASSERT_EQ(memcmp(buf, "xy", 2), 0);

    Error64 err = reader.peekInto(&reader, 3, buf);
    ASSERT_EQ(err_category(err), E64_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(MemoryReaderReader, PeekSlice_AfterEOFOffset)
{
    MemoryReader memoryReader("aaa");
    Reader reader = memoryReaderInterface(&memoryReader);

    ASSERT_TRUE(err_isOk(reader.skip(&reader, 3)));
    ASSERT_EQ(reader.offset, 3u);

    Slice slice;
    Error64 err = reader.peekSlice(&reader, 1, &slice);
    ASSERT_EQ(err_category(err), E64_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(MemoryReaderReader, MixedOperations_Consistency)
{
    MemoryReader memoryReader("abcdefghij");
    Reader reader = memoryReaderInterface(&memoryReader);

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

    Error64 err = reader.peekSlice(&reader, 3, &slice);
    ASSERT_EQ(err_category(err), E64_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(MemoryReaderReader, PeekSlice_SingleByteAdvances)
{
    MemoryReader mem("ABCDEFGH");
    Reader r = memoryReaderInterface(&mem);

    Slice s;
    for (size_t i = 0; i < 8; i++) {
        ASSERT_TRUE(err_isOk(r.peekSlice(&r, 1, &s)));
        ASSERT_EQ(s.len, 1u);
        ASSERT_EQ((char)s.slice[0], char('A' + i));

        ASSERT_TRUE(err_isOk(r.skip(&r, 1)));
        ASSERT_EQ(r.offset, i + 1);
    }

    // now at EOF
    Error64 err = r.peekSlice(&r, 1, &s);
    ASSERT_EQ(err_category(err), E64_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(MemoryReaderReader, OffsetMonotonicity_PeekDoesNotAdvance)
{
    MemoryReader mem("0123456789");
    Reader r = memoryReaderInterface(&mem);

    for (int i = 0; i < 20; i++) {
        Slice s;
        size_t before = r.offset;

        // varying peek sizes
        size_t n = (i % 11);
        Error64 err = r.peekSlice(&r, n, &s);

        if (n <= 10) {
            if (n <= 10 - before)
                ASSERT_TRUE(err_isOk(err));
            else {
                ASSERT_EQ(err_category(err), E64_Read);
                ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
            }
        }

        ASSERT_EQ(r.offset, before) << "peek must never advance offset";
    }
}

TEST(MemoryReaderReader, RepeatedEOFPeek)
{
    MemoryReader mem("xyz");
    Reader r = memoryReaderInterface(&mem);

    ASSERT_TRUE(err_isOk(r.skip(&r, 3)));
    ASSERT_EQ(r.offset, 3u);

    for (int i = 0; i < 10; i++) {
        Slice s;
        Error64 err = r.peekSlice(&r, 1, &s);
        ASSERT_EQ(err_category(err), E64_Read);
        ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
        ASSERT_EQ(r.offset, 3u);
    }
}

TEST(MemoryReaderReader, ExhaustiveByteWalk)
{
    const char* data = "abcdefghijklmnop";
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    uint8_t buf[32];

    for (size_t i = 0; i < strlen(data); i++) {
        // peek next byte
        ASSERT_TRUE(err_isOk(r.peekInto(&r, 1, buf)));
        ASSERT_EQ(buf[0], (uint8_t)data[i]);

        // now skip it
        ASSERT_TRUE(err_isOk(r.skip(&r, 1)));
        ASSERT_EQ(r.offset, i + 1);
    }

    // exhausted
    Error64 err = r.peekInto(&r, 1, buf);
    ASSERT_EQ(err_category(err), E64_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(MemoryReaderReader, SlidingWindows)
{
    const char* data = "0123456789";
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    Slice s;

    // at each offset j, try peeking windows of size k
    for (size_t j = 0; j < 10; j++) {
        ASSERT_TRUE(err_isOk(r.skip(&r, j - r.offset)));

        for (size_t k = 0; k <= 10; k++) {
            size_t remaining = 10 - j;

            Error64 err = r.peekSlice(&r, k, &s);

            if (k <= remaining) {
                ASSERT_TRUE(err_isOk(err));
                ASSERT_EQ(s.len, k);
                ASSERT_EQ(memcmp(s.slice, data + j, k), 0);
            } else {
                ASSERT_EQ(err_category(err), E64_Read);
                ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
            }
        }
    }
}

TEST(MemoryReaderReader, SliceStabilityAcrossPeeks)
{
    MemoryReader mem("ABCDE");
    Reader r = memoryReaderInterface(&mem);

    Slice s1, s2;

    ASSERT_TRUE(err_isOk(r.peekSlice(&r, 3, &s1)));
    ASSERT_EQ(memcmp(s1.slice, "ABC", 3), 0);

    ASSERT_TRUE(err_isOk(r.peekSlice(&r, 5, &s2)));
    ASSERT_EQ(memcmp(s2.slice, "ABCDE", 5), 0);

    // original slice must still match "ABC"
    ASSERT_EQ(memcmp(s1.slice, "ABC", 3), 0);

    ASSERT_EQ(r.offset, 0u);
}

TEST(MemoryReaderReader, SkipToLastByte)
{
    MemoryReader mem("Z123");
    Reader r = memoryReaderInterface(&mem);

    ASSERT_TRUE(err_isOk(r.skip(&r, 3)));
    ASSERT_EQ(r.offset, 3u);

    Slice s;
    ASSERT_TRUE(err_isOk(r.peekSlice(&r, 1, &s)));
    ASSERT_EQ((char)s.slice[0], '3');

    Error64 err = r.peekSlice(&r, 2, &s);
    ASSERT_EQ(err_category(err), E64_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(MemoryReaderReader, LargePeekRequest)
{
    MemoryReader mem("short");
    Reader r = memoryReaderInterface(&mem);

    Slice s;
    Error64 err = r.peekSlice(&r, 1000, &s);
    ASSERT_EQ(err_category(err), E64_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
    ASSERT_EQ(r.offset, 0u);
}

TEST(MemoryReaderReader, SkipExhaustively)
{
    MemoryReader mem("aaa");
    Reader r = memoryReaderInterface(&mem);

    ASSERT_TRUE(err_isOk(r.skip(&r, 3)));
    ASSERT_EQ(r.offset, 3u);

    for (int i = 0; i < 5; i++) {
        Error64 err = r.skip(&r, 1);
        ASSERT_EQ(err_category(err), E64_Read);
        ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
    }
}

TEST(MemoryReaderReader, PeekInto_Substrings)
{
    const char* data = "HelloWorld";
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    uint8_t buf[64];

    for (size_t n = 0; n <= strlen(data); n++) {
        ASSERT_TRUE(err_isOk(r.peekInto(&r, n, buf)));
        ASSERT_EQ(memcmp(buf, data, n), 0);
    }

    Error64 err = r.peekInto(&r, strlen(data) + 1, buf);
    ASSERT_EQ(err_category(err), E64_Read);
    ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
}

TEST(MemoryReaderReader, ComplexSkipPattern)
{
    const char* data = "abcdefghijklmnopqrstuv";
    MemoryReader mem(data);
    Reader r = memoryReaderInterface(&mem);

    size_t total = strlen(data);

    size_t steps[] = {1, 2, 3, 5, 8, 3, 2, 1};

    size_t expectedOffset = 0;
    for (size_t s : steps) {
        if (expectedOffset + s <= total) {
            ASSERT_TRUE(err_isOk(r.skip(&r, s)));
            expectedOffset += s;
        } else {
            Error64 err = r.skip(&r, s);
            ASSERT_EQ(err_category(err), E64_Read);
            ASSERT_EQ(err_subCategory(err), ERd_UnexpectedEOF);
        }

        ASSERT_EQ(r.offset, expectedOffset);
    }
}
