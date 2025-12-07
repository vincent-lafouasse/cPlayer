#include "gtest/gtest.h"

extern "C" {
#include "bitcast.h"
}

using u16 = uint16_t;
using i16 = int16_t;
using u32 = uint32_t;
using i32 = int32_t;
using u64 = uint64_t;
using i64 = int64_t;

TEST(bitcast, U16_LE)
{
    {
        constexpr Byte b[2] = {0x00, 0x00};
        constexpr u16 expected = 0x0000;
        EXPECT_EQ(bitcastU16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x01, 0x00};
        constexpr u16 expected = 0x0001;
        EXPECT_EQ(bitcastU16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0x00};
        constexpr u16 expected = 0x00ff;
        EXPECT_EQ(bitcastU16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x34, 0x12};
        constexpr u16 expected = 0x1234;
        EXPECT_EQ(bitcastU16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0xff};
        constexpr u16 expected = 0xffff;
        EXPECT_EQ(bitcastU16_LE(b), expected);
    }
}

TEST(bitcast, U16_BE)
{
    {
        constexpr Byte b[2] = {0x00, 0x00};
        constexpr u16 expected = 0x0000;
        EXPECT_EQ(bitcastU16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x00, 0x01};
        constexpr u16 expected = 0x0001;
        EXPECT_EQ(bitcastU16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x00, 0xff};
        constexpr u16 expected = 0x00ff;
        EXPECT_EQ(bitcastU16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x12, 0x34};
        constexpr u16 expected = 0x1234;
        EXPECT_EQ(bitcastU16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0xff};
        constexpr u16 expected = 0xffff;
        EXPECT_EQ(bitcastU16_BE(b), expected);
    }
}

TEST(bitcast, U32_LE)
{
    {
        constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x00};
        constexpr u32 expected = 0x00000000;
        EXPECT_EQ(bitcastU32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x01, 0x00, 0x00, 0x00};
        constexpr u32 expected = 0x00000001;
        EXPECT_EQ(bitcastU32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xfe, 0xca, 0xbe, 0xba};
        constexpr u32 expected = 0xbabecafe;
        EXPECT_EQ(bitcastU32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xff, 0xff, 0xff, 0xff};
        constexpr u32 expected = 0xffffffff;
        EXPECT_EQ(bitcastU32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x78, 0x56, 0x34, 0x12};
        constexpr u32 expected = 0x12345678;
        EXPECT_EQ(bitcastU32_LE(b), expected);
    }
}

TEST(bitcast, U32_BE)
{
    {
        constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x00};
        constexpr u32 expected = 0x00000000;
        EXPECT_EQ(bitcastU32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x01};
        constexpr u32 expected = 0x00000001;
        EXPECT_EQ(bitcastU32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xba, 0xbe, 0xca, 0xfe};
        constexpr u32 expected = 0xbabecafe;
        EXPECT_EQ(bitcastU32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xff, 0xff, 0xff, 0xff};
        constexpr u32 expected = 0xffffffff;
        EXPECT_EQ(bitcastU32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x12, 0x34, 0x56, 0x78};
        constexpr u32 expected = 0x12345678;
        EXPECT_EQ(bitcastU32_BE(b), expected);
    }
}

TEST(bitcast, I16_LE)
{
    {
        constexpr Byte b[2] = {0x00, 0x00};
        constexpr i16 expected = 0x0000;
        EXPECT_EQ(bitcastI16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x01, 0x00};
        constexpr i16 expected = 0x0001;
        EXPECT_EQ(bitcastI16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0x7f};
        constexpr i16 expected = INT16_MAX;
        EXPECT_EQ(bitcastI16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x00, 0x80};
        constexpr i16 expected = INT16_MIN;
        EXPECT_EQ(bitcastI16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xfe, 0xca};
        constexpr i16 expected = static_cast<i16>(0xcafe);
        EXPECT_EQ(bitcastI16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0xff};
        constexpr i16 expected = -1;
        EXPECT_EQ(bitcastI16_LE(b), expected);
    }
}

TEST(bitcast, I16_BE)
{
    {
        constexpr Byte b[2] = {0x00, 0x00};
        constexpr i16 expected = 0x0000;
        EXPECT_EQ(bitcastI16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x00, 0x01};
        constexpr i16 expected = 0x0001;
        EXPECT_EQ(bitcastI16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x7f, 0xff};
        constexpr i16 expected = INT16_MAX;
        EXPECT_EQ(bitcastI16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x80, 0x00};
        constexpr i16 expected = INT16_MIN;
        EXPECT_EQ(bitcastI16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xca, 0xfe};
        constexpr i16 expected = static_cast<i16>(0xcafe);
        EXPECT_EQ(bitcastI16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0xff};
        constexpr i16 expected = -1;
        EXPECT_EQ(bitcastI16_BE(b), expected);
    }
}

TEST(bitcast, I32_LE)
{
    {
        constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x00};
        constexpr i32 expected = 0x00000000;
        EXPECT_EQ(bitcastI32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x01, 0x00, 0x00, 0x00};
        constexpr i32 expected = 0x00000001;
        EXPECT_EQ(bitcastI32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xff, 0xff, 0xff, 0x7f};
        constexpr i32 expected = INT32_MAX;
        EXPECT_EQ(bitcastI32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x80};
        constexpr i32 expected = INT32_MIN;
        EXPECT_EQ(bitcastI32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xfe, 0xca, 0xbe, 0xba};
        constexpr i32 expected = static_cast<i32>(0xbabecafe);
        EXPECT_EQ(bitcastI32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xff, 0xff, 0xff, 0xff};
        constexpr i32 expected = -1;
        EXPECT_EQ(bitcastI32_LE(b), expected);
    }
}

TEST(bitcast, I32_BE)
{
    {
        constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x00};
        constexpr i32 expected = 0x00000000;
        EXPECT_EQ(bitcastI32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x01};
        constexpr i32 expected = 0x00000001;
        EXPECT_EQ(bitcastI32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x7f, 0xff, 0xff, 0xff};
        constexpr i32 expected = INT32_MAX;
        EXPECT_EQ(bitcastI32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x80, 0x00, 0x00, 0x00};
        constexpr i32 expected = INT32_MIN;
        EXPECT_EQ(bitcastI32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xba, 0xbe, 0xca, 0xfe};
        constexpr i32 expected = static_cast<i32>(0xbabecafe);
        EXPECT_EQ(bitcastI32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xff, 0xff, 0xff, 0xff};
        constexpr i32 expected = -1;
        EXPECT_EQ(bitcastI32_BE(b), expected);
    }
}

TEST(bitcast, U64_LE)
{
    // Zero
    {
        constexpr Byte b[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        constexpr u64 expected = 0x0000000000000000ull;
        EXPECT_EQ(bitcastU64_LE(b), expected);
    }
    // One (LSB set)
    {
        constexpr Byte b[8] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        constexpr u64 expected = 0x0000000000000001ull;
        EXPECT_EQ(bitcastU64_LE(b), expected);
    }
    // Max Value (all 0xFF)
    {
        constexpr Byte b[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
        constexpr u64 expected = UINT64_MAX;
        EXPECT_EQ(bitcastU64_LE(b), expected);
    }
    // Arbitrary Value
    {
        // 0x1122334455667788ull
        constexpr Byte b[8] = {0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11};
        constexpr u64 expected = 0x1122334455667788ull;
        EXPECT_EQ(bitcastU64_LE(b), expected);
    }
}

TEST(bitcast, U64_BE)
{
    // Zero
    {
        constexpr Byte b[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        constexpr u64 expected = 0x0000000000000000ull;
        EXPECT_EQ(bitcastU64_BE(b), expected);
    }
    // One (LSB set)
    {
        constexpr Byte b[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
        constexpr u64 expected = 0x0000000000000001ull;
        EXPECT_EQ(bitcastU64_BE(b), expected);
    }
    // Max Value (all 0xFF)
    {
        constexpr Byte b[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
        constexpr u64 expected = UINT64_MAX;
        EXPECT_EQ(bitcastU64_BE(b), expected);
    }
    // Arbitrary Value
    {
        // 0x1122334455667788ull
        constexpr Byte b[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
        constexpr u64 expected = 0x1122334455667788ull;
        EXPECT_EQ(bitcastU64_BE(b), expected);
    }
}

// --- Signed 64-bit Tests ---

TEST(bitcast, I64_LE)
{
    // Zero
    {
        constexpr Byte b[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        constexpr i64 expected = 0;
        EXPECT_EQ(bitcastI64_LE(b), expected);
    }
    // Max Value
    {
        // 0xFF in LSB, 0x7F in MSB
        constexpr Byte b[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f};
        constexpr i64 expected = INT64_MAX;
        EXPECT_EQ(bitcastI64_LE(b), expected);
    }
    // Min Value
    {
        // 0x00 in LSB, 0x80 in MSB
        constexpr Byte b[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80};
        constexpr i64 expected = INT64_MIN;
        EXPECT_EQ(bitcastI64_LE(b), expected);
    }
    // Minus One (all 0xFF)
    {
        constexpr Byte b[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
        constexpr i64 expected = -1;
        EXPECT_EQ(bitcastI64_LE(b), expected);
    }
    // Arbitrary Negative Value (e.g., -100)
    {
        // -100 is 0xFFFFFFFFFFFFFF9C in two's complement.
        constexpr Byte b[8] = {0x9c, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
        constexpr i64 expected = -100;
        EXPECT_EQ(bitcastI64_LE(b), expected);
    }
}

TEST(bitcast, I64_BE)
{
    // Zero
    {
        constexpr Byte b[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        constexpr i64 expected = 0;
        EXPECT_EQ(bitcastI64_BE(b), expected);
    }
    // Max Value
    {
        // 0x7F in MSB, 0xFF in LSB
        constexpr Byte b[8] = {0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
        constexpr i64 expected = INT64_MAX;
        EXPECT_EQ(bitcastI64_BE(b), expected);
    }
    // Min Value
    {
        // 0x80 in MSB, 0x00 in LSB
        constexpr Byte b[8] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        constexpr i64 expected = INT64_MIN;
        EXPECT_EQ(bitcastI64_BE(b), expected);
    }
    // Minus One (all 0xFF)
    {
        constexpr Byte b[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
        constexpr i64 expected = -1;
        EXPECT_EQ(bitcastI64_BE(b), expected);
    }
    // Arbitrary Negative Value (e.g., -100)
    {
        // -100 is 0xFFFFFFFFFFFFFF9C in two's complement.
        constexpr Byte b[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x9c};
        constexpr i64 expected = -100;
        EXPECT_EQ(bitcastI64_BE(b), expected);
    }
}
