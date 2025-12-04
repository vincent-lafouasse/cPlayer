#include <cstdint>
#include <string>

#include "gtest/gtest.h"

#include "MemoryReader.hpp"

extern "C" {
#include "Reader.h"
}

void assertSliceEq(const Slice& slice, const std::string& expected)
{
    std::string actual(slice.slice, slice.slice + slice.len);
    ASSERT_EQ(actual, expected);
}

TEST(Reader, PeekSlice_Basic)
{
    MemoryReader memoryReader("1234567");
    Reader reader = memoryReaderInterface(&memoryReader);

    Slice slice;

    ASSERT_EQ(reader.peekSlice(&reader, 1, &slice), NoError);
    assertSliceEq(slice, "1");

    ASSERT_EQ(reader.peekSlice(&reader, 2, &slice), NoError);
    assertSliceEq(slice, "12");

    ASSERT_EQ(reader.peekSlice(&reader, 3, &slice), NoError);
    assertSliceEq(slice, "123");

    ASSERT_EQ(reader.peekSlice(&reader, 4, &slice), NoError);
    assertSliceEq(slice, "1234");

    ASSERT_EQ(reader.peekSlice(&reader, 5, &slice), NoError);
    assertSliceEq(slice, "12345");

    ASSERT_EQ(reader.peekSlice(&reader, 6, &slice), NoError);
    assertSliceEq(slice, "123456");

    ASSERT_EQ(reader.peekSlice(&reader, 7, &slice), NoError);
    assertSliceEq(slice, "1234567");

    ASSERT_EQ(reader.peekSlice(&reader, 8, &slice), E_UnexpectedEOF);
}

TEST(Reader, PeekInto_Basic)
{
    MemoryReader memoryReader("abcdefg");
    Reader reader = memoryReaderInterface(&memoryReader);

    uint8_t buf[16] = {};

    ASSERT_EQ(reader.peekInto(&reader, 3, buf), NoError);
    ASSERT_EQ(memcmp(buf, "abc", 3), 0);

    ASSERT_EQ(reader.peekInto(&reader, 7, buf), NoError);
    ASSERT_EQ(memcmp(buf, "abcdefg", 7), 0);

    ASSERT_EQ(reader.peekInto(&reader, 8, buf), E_UnexpectedEOF);
}

TEST(Reader, PeekInto_DoesNotAdvance)
{
    MemoryReader memoryReader("hello");
    Reader reader = memoryReaderInterface(&memoryReader);

    uint8_t buf[8] = {};
    ASSERT_EQ(reader.peekInto(&reader, 5, buf), NoError);
    ASSERT_EQ(memcmp(buf, "hello", 5), 0);

    // peek again, expecting same
    memset(buf, 0, sizeof(buf));
    ASSERT_EQ(reader.peekInto(&reader, 5, buf), NoError);
    ASSERT_EQ(memcmp(buf, "hello", 5), 0);

    // offset must not have changed
    ASSERT_EQ(reader.offset, 0u);
}

TEST(Reader, Skip_Basic)
{
    MemoryReader memoryReader("ABCDEFG");
    Reader reader = memoryReaderInterface(&memoryReader);

    ASSERT_EQ(reader.skip(&reader, 1), NoError);
    ASSERT_EQ(reader.offset, 1u);

    ASSERT_EQ(reader.skip(&reader, 2), NoError);
    ASSERT_EQ(reader.offset, 3u);

    ASSERT_EQ(reader.skip(&reader, 3), NoError);
    ASSERT_EQ(reader.offset, 6u);

    ASSERT_EQ(reader.skip(&reader, 1), NoError);  // exactly consumes
    ASSERT_EQ(reader.offset, 7u);

    ASSERT_EQ(reader.skip(&reader, 1), E_UnexpectedEOF);
}

TEST(Reader, Skip_Then_Peek)
{
    MemoryReader memoryReader("0123456789");
    Reader reader = memoryReaderInterface(&memoryReader);

    ASSERT_EQ(reader.skip(&reader, 3), NoError);
    ASSERT_EQ(reader.offset, 3u);

    Slice slice;
    ASSERT_EQ(reader.peekSlice(&reader, 4, &slice), NoError);
    assertSliceEq(slice, "3456");

    ASSERT_EQ(reader.skip(&reader, 4), NoError);
    ASSERT_EQ(reader.offset, 7u);

    ASSERT_EQ(reader.peekSlice(&reader, 3, &slice), NoError);
    assertSliceEq(slice, "789");

    ASSERT_EQ(reader.peekSlice(&reader, 4, &slice), E_UnexpectedEOF);
}

TEST(Reader, ZeroLength_Peeks)
{
    MemoryReader memoryReader("xyz");
    Reader reader = memoryReaderInterface(&memoryReader);

    Slice slice;
    ASSERT_EQ(reader.peekSlice(&reader, 0, &slice), NoError);
    ASSERT_EQ(slice.len, 0u);

    uint8_t buf[1];
    ASSERT_EQ(reader.peekInto(&reader, 0, buf), NoError);
}

TEST(Reader, EOF_PeekSlice_Exact)
{
    MemoryReader memoryReader("hi");
    Reader reader = memoryReaderInterface(&memoryReader);

    Slice slice;
    ASSERT_EQ(reader.peekSlice(&reader, 2, &slice), NoError);
    assertSliceEq(slice, "hi");

    ASSERT_EQ(reader.peekSlice(&reader, 3, &slice), E_UnexpectedEOF);
}

TEST(Reader, EOF_PeekInto_Exact)
{
    MemoryReader memoryReader("xy");
    Reader reader = memoryReaderInterface(&memoryReader);

    uint8_t buf[4];

    ASSERT_EQ(reader.peekInto(&reader, 2, buf), NoError);
    ASSERT_EQ(memcmp(buf, "xy", 2), 0);

    ASSERT_EQ(reader.peekInto(&reader, 3, buf), E_UnexpectedEOF);
}

TEST(Reader, PeekSlice_AfterEOFOffset)
{
    MemoryReader memoryReader("aaa");
    Reader reader = memoryReaderInterface(&memoryReader);

    ASSERT_EQ(reader.skip(&reader, 3), NoError);
    ASSERT_EQ(reader.offset, 3u);

    Slice slice;
    ASSERT_EQ(reader.peekSlice(&reader, 1, &slice), E_UnexpectedEOF);
}

TEST(Reader, MixedOperations_Consistency)
{
    MemoryReader memoryReader("abcdefghij");
    Reader reader = memoryReaderInterface(&memoryReader);

    Slice slice;

    // Peek first 5
    ASSERT_EQ(reader.peekSlice(&reader, 5, &slice), NoError);
    assertSliceEq(slice, "abcde");

    // Skip 2
    ASSERT_EQ(reader.skip(&reader, 2), NoError);
    ASSERT_EQ(reader.offset, 2u);

    // Peek 4 starting at offset 2
    ASSERT_EQ(reader.peekSlice(&reader, 4, &slice), NoError);
    assertSliceEq(slice, "cdef");

    // Skip to near end
    ASSERT_EQ(reader.skip(&reader, 6), NoError);
    ASSERT_EQ(reader.offset, 8u);

    ASSERT_EQ(reader.peekSlice(&reader, 2, &slice), NoError);
    assertSliceEq(slice, "ij");

    ASSERT_EQ(reader.peekSlice(&reader, 3, &slice), E_UnexpectedEOF);
}
