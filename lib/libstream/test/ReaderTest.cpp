#include <cstdint>
#include <string>

#include "gtest/gtest.h"

#include "MockReader.hpp"
#include "Reader.h"

static void assertSliceEq(const Slice& slice, const std::string& expected)
{
    std::string actual(slice.slice, slice.slice + slice.len);
    ASSERT_EQ(actual, expected);
}

#define ASSERT_IS_OK(status) ASSERT_EQ(status, ReadStatus_Ok)
#define ASSERT_IS_EOF(status) ASSERT_EQ(status, ReadStatus_UnexpectedEOF)

TEST(MockReaderReader, PeekSlice_Basic)
{
    MockReader mockReader("1234567");
    Reader reader = mockReaderInterface(&mockReader);

    Slice slice;

    ASSERT_IS_OK(reader.peekSlice(&reader, 1, &slice));
    assertSliceEq(slice, "1");

    ASSERT_IS_OK(reader.peekSlice(&reader, 2, &slice));
    assertSliceEq(slice, "12");

    ASSERT_IS_OK(reader.peekSlice(&reader, 3, &slice));
    assertSliceEq(slice, "123");

    ASSERT_IS_OK(reader.peekSlice(&reader, 4, &slice));
    assertSliceEq(slice, "1234");

    ASSERT_IS_OK(reader.peekSlice(&reader, 5, &slice));
    assertSliceEq(slice, "12345");

    ASSERT_IS_OK(reader.peekSlice(&reader, 6, &slice));
    assertSliceEq(slice, "123456");

    ASSERT_IS_OK(reader.peekSlice(&reader, 7, &slice));
    assertSliceEq(slice, "1234567");

    ASSERT_IS_EOF(reader.peekSlice(&reader, 8, &slice));
}

TEST(MockReaderReader, Skip_Basic)
{
    MockReader mockReader("ABCDEFG");
    Reader reader = mockReaderInterface(&mockReader);

    ASSERT_IS_OK(reader.skip(&reader, 1));
    ASSERT_EQ(reader.offset, 1u);

    ASSERT_IS_OK(reader.skip(&reader, 2));
    ASSERT_EQ(reader.offset, 3u);

    ASSERT_IS_OK(reader.skip(&reader, 3));
    ASSERT_EQ(reader.offset, 6u);

    ASSERT_IS_OK(reader.skip(&reader, 1));
    ASSERT_EQ(reader.offset, 7u);

    ASSERT_IS_EOF(reader.skip(&reader, 1));
}

TEST(MockReaderReader, Skip_Then_Peek)
{
    MockReader mockReader("0123456789");
    Reader reader = mockReaderInterface(&mockReader);

    ASSERT_IS_OK(reader.skip(&reader, 3));
    ASSERT_EQ(reader.offset, 3u);

    Slice slice;
    ASSERT_IS_OK(reader.peekSlice(&reader, 4, &slice));
    assertSliceEq(slice, "3456");

    ASSERT_IS_OK(reader.skip(&reader, 4));
    ASSERT_EQ(reader.offset, 7u);

    ASSERT_IS_OK(reader.peekSlice(&reader, 3, &slice));
    assertSliceEq(slice, "789");

    ReadStatus err = reader.peekSlice(&reader, 4, &slice);
    ASSERT_IS_EOF(err);
}

TEST(MockReaderReader, ZeroLength_Peeks)
{
    MockReader mockReader("xyz");
    Reader reader = mockReaderInterface(&mockReader);

    Slice slice;
    ASSERT_IS_OK(reader.peekSlice(&reader, 0, &slice));
    ASSERT_EQ(slice.len, 0u);
}

TEST(MockReaderReader, EOF_PeekSlice_Exact)
{
    MockReader mockReader("hi");
    Reader reader = mockReaderInterface(&mockReader);

    Slice slice;
    ASSERT_IS_OK(reader.peekSlice(&reader, 2, &slice));
    assertSliceEq(slice, "hi");

    ReadStatus err = reader.peekSlice(&reader, 3, &slice);
    ASSERT_IS_EOF(err);
}

TEST(MockReaderReader, PeekSlice_AfterEOFOffset)
{
    MockReader mockReader("aaa");
    Reader reader = mockReaderInterface(&mockReader);

    ASSERT_IS_OK(reader.skip(&reader, 3));
    ASSERT_EQ(reader.offset, 3u);

    Slice slice;
    ReadStatus err = reader.peekSlice(&reader, 1, &slice);
    ASSERT_IS_EOF(err);
}

TEST(MockReaderReader, MixedOperations_Consistency)
{
    MockReader mockReader("abcdefghij");
    Reader reader = mockReaderInterface(&mockReader);

    Slice slice;

    // Peek first 5
    ASSERT_IS_OK(reader.peekSlice(&reader, 5, &slice));
    assertSliceEq(slice, "abcde");

    // Skip 2
    ASSERT_IS_OK(reader.skip(&reader, 2));
    ASSERT_EQ(reader.offset, 2u);

    // Peek 4 starting at offset 2
    ASSERT_IS_OK(reader.peekSlice(&reader, 4, &slice));
    assertSliceEq(slice, "cdef");

    // Skip to near end
    ASSERT_IS_OK(reader.skip(&reader, 6));
    ASSERT_EQ(reader.offset, 8u);

    ASSERT_IS_OK(reader.peekSlice(&reader, 2, &slice));
    assertSliceEq(slice, "ij");

    ReadStatus err = reader.peekSlice(&reader, 3, &slice);
    ASSERT_IS_EOF(err);
}

TEST(MockReaderReader, PeekSlice_SingleByteAdvances)
{
    MockReader mem("ABCDEFGH");
    Reader r = mockReaderInterface(&mem);

    Slice s;
    for (size_t i = 0; i < 8; i++) {
        ASSERT_IS_OK(r.peekSlice(&r, 1, &s));
        ASSERT_EQ(s.len, 1u);
        ASSERT_EQ((char)s.slice[0], char('A' + i));

        ASSERT_IS_OK(r.skip(&r, 1));
        ASSERT_EQ(r.offset, i + 1);
    }

    // now at EOF
    ReadStatus err = r.peekSlice(&r, 1, &s);
    ASSERT_IS_EOF(err);
}

TEST(MockReaderReader, OffsetMonotonicity_PeekDoesNotAdvance)
{
    MockReader mem("0123456789");
    Reader r = mockReaderInterface(&mem);

    for (int i = 0; i < 20; i++) {
        Slice s;
        size_t before = r.offset;

        // varying peek sizes
        size_t n = (i % 11);
        ReadStatus err = r.peekSlice(&r, n, &s);

        if (n <= 10) {
            if (n <= 10 - before)
                ASSERT_IS_OK(err);
            else {
                ASSERT_IS_EOF(err);
            }
        }

        ASSERT_EQ(r.offset, before) << "peek must never advance offset";
    }
}

TEST(MockReaderReader, RepeatedEOFPeek)
{
    MockReader mem("xyz");
    Reader r = mockReaderInterface(&mem);

    ASSERT_IS_OK(r.skip(&r, 3));
    ASSERT_EQ(r.offset, 3u);

    for (int i = 0; i < 10; i++) {
        Slice s;
        ASSERT_IS_EOF(r.peekSlice(&r, 1, &s));
        ASSERT_EQ(r.offset, 3u);
    }
}

TEST(MockReaderReader, ExhaustiveByteWalk)
{
    const char* data = "abcdefghijklmnop";
    MockReader mem(data);
    Reader r = mockReaderInterface(&mem);

    Slice slice;

    for (size_t i = 0; i < strlen(data); i++) {
        // peek next byte
        ASSERT_IS_OK(r.peekSlice(&r, 1, &slice));
        ASSERT_EQ(slice.slice[0], (uint8_t)data[i]);

        // now skip it
        ASSERT_IS_OK(r.skip(&r, 1));
        ASSERT_EQ(r.offset, i + 1);
    }

    // exhausted
    ASSERT_IS_EOF(r.peekSlice(&r, 1, &slice));
}

TEST(MockReaderReader, SlidingWindows)
{
    const char* data = "0123456789";
    MockReader mem(data);
    Reader r = mockReaderInterface(&mem);

    Slice s;

    // at each offset j, try peeking windows of size k
    for (size_t j = 0; j < 10; j++) {
        ASSERT_IS_OK(r.skip(&r, j - r.offset));

        for (size_t k = 0; k <= 10; k++) {
            size_t remaining = 10 - j;

            ReadStatus err = r.peekSlice(&r, k, &s);

            if (k <= remaining) {
                ASSERT_IS_OK(err);
                ASSERT_EQ(s.len, k);
                ASSERT_EQ(memcmp(s.slice, data + j, k), 0);
            } else {
                ASSERT_IS_EOF(err);
            }
        }
    }
}

TEST(MockReaderReader, SliceStabilityAcrossPeeks)
{
    MockReader mem("ABCDE");
    Reader r = mockReaderInterface(&mem);

    Slice s1, s2;

    ASSERT_IS_OK(r.peekSlice(&r, 3, &s1));
    ASSERT_EQ(memcmp(s1.slice, "ABC", 3), 0);

    ASSERT_IS_OK(r.peekSlice(&r, 5, &s2));
    ASSERT_EQ(memcmp(s2.slice, "ABCDE", 5), 0);

    // original slice must still match "ABC"
    ASSERT_EQ(memcmp(s1.slice, "ABC", 3), 0);

    ASSERT_EQ(r.offset, 0u);
}

TEST(MockReaderReader, SkipToLastByte)
{
    MockReader mem("Z123");
    Reader r = mockReaderInterface(&mem);

    ASSERT_IS_OK(r.skip(&r, 3));
    ASSERT_EQ(r.offset, 3u);

    Slice s;
    ASSERT_IS_OK(r.peekSlice(&r, 1, &s));
    ASSERT_EQ((char)s.slice[0], '3');

    ReadStatus err = r.peekSlice(&r, 2, &s);
    ASSERT_IS_EOF(err);
}

TEST(MockReaderReader, LargePeekRequest)
{
    MockReader mem("short");
    Reader r = mockReaderInterface(&mem);

    Slice s;
    ReadStatus err = r.peekSlice(&r, 1000, &s);
    ASSERT_IS_EOF(err);
}

TEST(MockReaderReader, SkipExhaustively)
{
    MockReader mem("aaa");
    Reader r = mockReaderInterface(&mem);

    ASSERT_IS_OK(r.skip(&r, 3));
    ASSERT_EQ(r.offset, 3u);

    for (int i = 0; i < 5; i++) {
        ReadStatus err = r.skip(&r, 1);
        ASSERT_IS_EOF(err);
    }
}

TEST(MockReaderReader, ComplexSkipPattern)
{
    const char* data = "abcdefghijklmnopqrstuv";
    MockReader mem(data);
    Reader r = mockReaderInterface(&mem);

    size_t total = strlen(data);

    size_t steps[] = {1, 2, 3, 5, 8, 3, 2, 1};

    size_t expectedOffset = 0;
    for (size_t s : steps) {
        if (expectedOffset + s <= total) {
            ASSERT_IS_OK(r.skip(&r, s));
            expectedOffset += s;
        } else {
            ReadStatus err = r.skip(&r, s);
            ASSERT_IS_EOF(err);
        }

        ASSERT_EQ(r.offset, expectedOffset);
    }
}
