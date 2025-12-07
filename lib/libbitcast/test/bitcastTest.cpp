#include <cfloat>
#include <cmath>

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

TEST(bitcast, U24_LE)
{
    // Zero
    {
        constexpr Byte b[3] = {0x00, 0x00, 0x00};
        constexpr u32 expected = 0x000000;
        EXPECT_EQ(bitcastU24_LE(b), expected);
    }
    // One (LSB set)
    {
        constexpr Byte b[3] = {0x01, 0x00, 0x00};
        constexpr u32 expected = 0x000001;
        EXPECT_EQ(bitcastU24_LE(b), expected);
    }
    // Max Positive Value (0xFFFFFF)
    {
        constexpr Byte b[3] = {0xff, 0xff, 0xff};
        constexpr u32 expected = 0xffffff;
        EXPECT_EQ(bitcastU24_LE(b), expected);
    }
    // Arbitrary Value
    {
        // 0x123456
        constexpr Byte b[3] = {0x56, 0x34, 0x12};
        constexpr u32 expected = 0x123456;
        EXPECT_EQ(bitcastU24_LE(b), expected);
    }
}

TEST(bitcast, U24_BE)
{
    // Zero
    {
        constexpr Byte b[3] = {0x00, 0x00, 0x00};
        constexpr u32 expected = 0x000000;
        EXPECT_EQ(bitcastU24_BE(b), expected);
    }
    // One (LSB set)
    {
        constexpr Byte b[3] = {0x00, 0x00, 0x01};
        constexpr u32 expected = 0x000001;
        EXPECT_EQ(bitcastU24_BE(b), expected);
    }
    // Max Positive Value (0xFFFFFF)
    {
        constexpr Byte b[3] = {0xff, 0xff, 0xff};
        constexpr u32 expected = 0xffffff;
        EXPECT_EQ(bitcastU24_BE(b), expected);
    }
    // Arbitrary Value
    {
        // 0x123456
        constexpr Byte b[3] = {0x12, 0x34, 0x56};
        constexpr u32 expected = 0x123456;
        EXPECT_EQ(bitcastU24_BE(b), expected);
    }
}

constexpr i32 int24Min = -(1 << 23);
constexpr i32 int24Max = -int24Min - 1;

TEST(bitcastI24, Zero_LE)
{
    constexpr Byte b[3] = {0x00, 0x00, 0x00};
    constexpr i32 expected = 0;
    EXPECT_EQ(bitcastI24_LE(b), expected);
}

TEST(bitcastI24, One_LE)
{
    constexpr Byte b[3] = {0x01, 0x00, 0x00};
    constexpr i32 expected = 1;
    EXPECT_EQ(bitcastI24_LE(b), expected);
}

TEST(bitcastI24, MaxPositive_LE)
{
    // Corresponds to 0x007fffff
    constexpr Byte b[3] = {0xff, 0xff, 0x7f};
    constexpr i32 expected = int24Max;
    EXPECT_EQ(bitcastI24_LE(b), expected);
}

TEST(bitcastI24, PositiveNearBoundary_LE)
{
    // 0x007ffffe (One less than Max Positive)
    constexpr Byte b[3] = {0xfe, 0xff, 0x7f};
    constexpr i32 expected = int24Max - 1;
    EXPECT_EQ(bitcastI24_LE(b), expected);
}

TEST(bitcastI24, MinNegative_LE)
{
    // Corresponds to 0x00800000 (The 24-bit sign bit is set)
    constexpr Byte b[3] = {0x00, 0x00, 0x80};
    constexpr i32 expected = int24Min;
    EXPECT_EQ(bitcastI24_LE(b), expected);
}

TEST(bitcastI24, NegativeNearBoundary_LE)
{
    // Corresponds to 0x00800001
    constexpr Byte b[3] = {0x01, 0x00, 0x80};
    constexpr i32 expected = int24Min + 1;
    EXPECT_EQ(bitcastI24_LE(b), expected);
}

TEST(bitcastI24, AllOnes_NegativeOne_LE)
{
    // Corresponds to 0x00ffffff, which is -1
    constexpr Byte b[3] = {0xff, 0xff, 0xff};
    constexpr i32 expected = -1;
    EXPECT_EQ(bitcastI24_LE(b), expected);
}
TEST(bitcastI24, Zero_BE)
{
    constexpr Byte b[3] = {0x00, 0x00, 0x00};
    constexpr i32 expected = 0;
    EXPECT_EQ(bitcastI24_BE(b), expected);
}

TEST(bitcastI24, One_BE)
{
    constexpr Byte b[3] = {0x00, 0x00, 0x01};
    constexpr i32 expected = 1;
    EXPECT_EQ(bitcastI24_BE(b), expected);
}

TEST(bitcastI24, MaxPositive_BE)
{
    // Corresponds to 0x007fffff
    constexpr Byte b[3] = {0x7f, 0xff, 0xff};
    constexpr i32 expected = int24Max;
    EXPECT_EQ(bitcastI24_BE(b), expected);
}

TEST(bitcastI24, PositiveNearBoundary_BE)
{
    // 0x007ffffe (One less than Max Positive)
    constexpr Byte b[3] = {0x7f, 0xff, 0xfe};
    constexpr i32 expected = int24Max - 1;
    EXPECT_EQ(bitcastI24_BE(b), expected);
}

TEST(bitcastI24, MinNegative_BE)
{
    // Corresponds to 0x00800000 (The 24-bit sign bit is set)
    constexpr Byte b[3] = {0x80, 0x00, 0x00};
    constexpr i32 expected = int24Min;
    EXPECT_EQ(bitcastI24_BE(b), expected);
}

TEST(bitcastI24, NegativeNearBoundary_BE)
{
    // Corresponds to 0x00800001
    constexpr Byte b[3] = {0x80, 0x00, 0x01};
    constexpr i32 expected = int24Min + 1;
    EXPECT_EQ(bitcastI24_BE(b), expected);
}

TEST(bitcastI24, AllOnes_NegativeOne_BE)
{
    // Corresponds to 0x00ffffff, which is -1
    constexpr Byte b[3] = {0xff, 0xff, 0xff};
    constexpr i32 expected = -1;
    EXPECT_EQ(bitcastI24_BE(b), expected);
}

// --- bitcastF32_LE Tests (Little Endian) ---

TEST(bitcastF32, Zero_LE)
{
    // 0x00000000 -> 0.0f
    constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(bitcastF32_LE(b), 0.0f);
}

TEST(bitcastF32, One_LE)
{
    // 0x3f800000 -> 1.0f
    constexpr Byte b[4] = {0x00, 0x00, 0x80, 0x3f};
    EXPECT_EQ(bitcastF32_LE(b), 1.0f);
}

TEST(bitcastF32, NegativeOne_LE)
{
    // 0xbf800000 -> -1.0f
    constexpr Byte b[4] = {0x00, 0x00, 0x80, 0xbf};
    EXPECT_EQ(bitcastF32_LE(b), -1.0f);
}

TEST(bitcastF32, Half_LE)
{
    // 0x3f000000 -> 0.5f
    constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x3f};
    EXPECT_EQ(bitcastF32_LE(b), 0.5f);
}

TEST(bitcastF32, MaxPositive_LE)
{
    // 0x7f7fffff -> FLT_MAX
    constexpr Byte b[4] = {0xff, 0xff, 0x7f, 0x7f};
    EXPECT_EQ(bitcastF32_LE(b), FLT_MAX);
}

TEST(bitcastF32, MinPositiveNormalized_LE)
{
    // 0x00800000 -> FLT_MIN (smallest normalized positive float)
    constexpr Byte b[4] = {0x00, 0x00, 0x80, 0x00};
    EXPECT_EQ(bitcastF32_LE(b), FLT_MIN);
}

TEST(bitcastF32, PiValue_LE)
{
    // 0x40490fdb (~3.14159265)
    constexpr Byte b[4] = {0xdb, 0x0f, 0x49, 0x40};
    EXPECT_NEAR(bitcastF32_LE(b), 3.14159265f, 1e-6);
}

TEST(bitcastF32, Infinity_LE)
{
    // 0x7f800000
    constexpr Byte b[4] = {0x00, 0x00, 0x80, 0x7f};
    const float result = bitcastF32_LE(b);
    EXPECT_TRUE(isinf(result) && result > 0);
}

TEST(bitcastF32, NegativeInfinity_LE)
{
    // 0xff800000
    constexpr Byte b[4] = {0x00, 0x00, 0x80, 0xff};
    const float result = bitcastF32_LE(b);
    EXPECT_TRUE(isinf(result) && result < 0);
}

TEST(bitcastF32, NaN_LE)
{
    // 0x7fc00000 (Quiet NaN)
    constexpr Byte b[4] = {0x00, 0x00, 0xc0, 0x7f};
    EXPECT_TRUE(isnan(bitcastF32_LE(b)));
}

// --- bitcastF32_BE Tests (Big Endian) ---

TEST(bitcastF32, One_BE)
{
    // 0x3f800000 (1.0f)
    constexpr Byte b[4] = {0x3f, 0x80, 0x00, 0x00};
    EXPECT_EQ(bitcastF32_BE(b), 1.0f);
}

TEST(bitcastF32, NegativeHalf_BE)
{
    // 0xbf000000 (-0.5f)
    constexpr Byte b[4] = {0xbf, 0x00, 0x00, 0x00};
    EXPECT_EQ(bitcastF32_BE(b), -0.5f);
}

TEST(bitcastF32, PiValue_BE)
{
    // 0x40490fdb (~3.14159265)
    constexpr Byte b[4] = {0x40, 0x49, 0x0f, 0xdb};
    EXPECT_NEAR(bitcastF32_BE(b), 3.14159265f, 1e-6);
}

TEST(bitcastF32, MaxPositive_BE)
{
    // 0x7f7fffff (FLT_MAX)
    constexpr Byte b[4] = {0x7f, 0x7f, 0xff, 0xff};
    EXPECT_EQ(bitcastF32_BE(b), FLT_MAX);
}

// --- bitcastF64_LE Tests (Little Endian) ---

TEST(bitcastF64, One_LE)
{
    // 0x3ff0000000000000 (1.0)
    // LE: 00 00 00 00 00 00 F0 3F
    constexpr Byte b[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x3f};
    EXPECT_EQ(bitcastF64_LE(b), 1.0);
}

TEST(bitcastF64, NegativeHalf_LE)
{
    // 0xbfe0000000000000 (-0.5)
    // LE: 00 00 00 00 00 00 E0 BF
    constexpr Byte b[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xbf};
    EXPECT_EQ(bitcastF64_LE(b), -0.5);
}

TEST(bitcastF64, MaxPositive_LE)
{
    // 0x7fefffffffffffff (DBL_MAX)
    // LE: FF FF FF FF FF FF EF 7F
    constexpr Byte b[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xef, 0x7f};
    EXPECT_EQ(bitcastF64_LE(b), DBL_MAX);
}

TEST(bitcastF64, MinPositiveNormalized_LE)
{
    // 0x0010000000000000 (DBL_MIN)
    // LE: 00 00 00 00 00 00 10 00
    constexpr Byte b[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00};
    EXPECT_EQ(bitcastF64_LE(b), DBL_MIN);
}

TEST(bitcastF64, PiValue_LE)
{
    // 0x400921fb54442d18 (~3.14159265359)
    // LE: 18 2D 44 54 FB 21 09 40
    constexpr Byte b[8] = {0x18, 0x2d, 0x44, 0x54, 0xfb, 0x21, 0x09, 0x40};
    EXPECT_NEAR(bitcastF64_LE(b), 3.141592653589793, 1e-12);
}

TEST(bitcastF64, PositiveInfinity_LE)
{
    // 0x7ff0000000000000
    // LE: 00 00 00 00 00 00 F0 7F
    constexpr Byte b[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x7f};
    const double result = bitcastF64_LE(b);
    EXPECT_TRUE(isinf(result) && result > 0);
}

TEST(bitcastF64, NegativeInfinity_LE)
{
    // 0xfff0000000000000
    // LE: 00 00 00 00 00 00 F0 FF
    constexpr Byte b[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff};
    const double result = bitcastF64_LE(b);
    EXPECT_TRUE(isinf(result) && result < 0);
}

TEST(bitcastF64, QuietNaN_LE)
{
    // 0x7ff8000000000000 (Exponent all ones, MSB of mantissa set)
    // LE: 00 00 00 00 00 00 F8 7F
    constexpr Byte b[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x7f};
    EXPECT_TRUE(isnan(bitcastF64_LE(b)));
}

// --- bitcastF64_BE Tests (Big Endian) ---

TEST(bitcastF64, One_BE)
{
    // 0x3ff0000000000000 (1.0)
    // BE: 3F F0 00 00 00 00 00 00
    constexpr Byte b[8] = {0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(bitcastF64_BE(b), 1.0);
}

TEST(bitcastF64, NegativeOne_BE)
{
    // 0xbff0000000000000 (-1.0)
    // BE: BF F0 00 00 00 00 00 00
    constexpr Byte b[8] = {0xbf, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(bitcastF64_BE(b), -1.0);
}

TEST(bitcastF64, MaxPositive_BE)
{
    // 0x7fefffffffffffff (DBL_MAX)
    // BE: 7F EF FF FF FF FF FF FF
    constexpr Byte b[8] = {0x7f, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    EXPECT_EQ(bitcastF64_BE(b), DBL_MAX);
}

TEST(bitcastF64, PiValue_BE)
{
    // 0x400921fb54442d18 (~3.14159265359)
    // BE: 40 09 21 FB 54 44 2D 18
    constexpr Byte b[8] = {0x40, 0x09, 0x21, 0xfb, 0x54, 0x44, 0x2d, 0x18};
    EXPECT_NEAR(bitcastF64_BE(b), 3.141592653589793, 1e-12);
}

TEST(bitcastF64, QuietNaN_BE)
{
    // 0x7ff8000000000000
    // BE: 7F F8 00 00 00 00 00 00
    constexpr Byte b[8] = {0x7f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    EXPECT_TRUE(isnan(bitcastF64_BE(b)));
}
