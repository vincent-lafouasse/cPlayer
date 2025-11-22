#include "gtest/gtest.h"

extern "C" {
#include "DeserializeInts.h"
}

using u16 = uint16_t;
using i16 = int16_t;
using u32 = uint32_t;

TEST(Deserialize, U16_LE)
{
    {
        constexpr Byte b[2] = {0x00, 0x00};
        constexpr u16 expected = 0x0000;
        EXPECT_EQ(deserializeU16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x01, 0x00};
        constexpr u16 expected = 0x0001;
        EXPECT_EQ(deserializeU16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0x00};
        constexpr u16 expected = 0x00ff;
        EXPECT_EQ(deserializeU16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x34, 0x12};
        constexpr u16 expected = 0x1234;
        EXPECT_EQ(deserializeU16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0xff};
        constexpr u16 expected = 0xffff;
        EXPECT_EQ(deserializeU16_LE(b), expected);
    }
}

TEST(Deserialize, U16_BE)
{
    {
        constexpr Byte b[2] = {0x00, 0x00};
        constexpr u16 expected = 0x0000;
        EXPECT_EQ(deserializeU16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x00, 0x01};
        constexpr u16 expected = 0x0001;
        EXPECT_EQ(deserializeU16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x00, 0xff};
        constexpr u16 expected = 0x00ff;
        EXPECT_EQ(deserializeU16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x12, 0x34};
        constexpr u16 expected = 0x1234;
        EXPECT_EQ(deserializeU16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0xff};
        constexpr u16 expected = 0xffff;
        EXPECT_EQ(deserializeU16_BE(b), expected);
    }
}

TEST(Deserialize, U32_LE)
{
    {
        constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x00};
        constexpr u32 expected = 0x00000000;
        EXPECT_EQ(deserializeU32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x01, 0x00, 0x00, 0x00};
        constexpr u32 expected = 0x00000001;
        EXPECT_EQ(deserializeU32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xfe, 0xca, 0xbe, 0xba};
        constexpr u32 expected = 0xbabecafe;
        EXPECT_EQ(deserializeU32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xff, 0xff, 0xff, 0xff};
        constexpr u32 expected = 0xffffffff;
        EXPECT_EQ(deserializeU32_LE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x78, 0x56, 0x34, 0x12};
        constexpr u32 expected = 0x12345678;
        EXPECT_EQ(deserializeU32_LE(b), expected);
    }
}

TEST(Deserialize, U32_BE)
{
    {
        constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x00};
        constexpr u32 expected = 0x00000000;
        EXPECT_EQ(deserializeU32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x00, 0x00, 0x00, 0x01};
        constexpr u32 expected = 0x00000001;
        EXPECT_EQ(deserializeU32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xba, 0xbe, 0xca, 0xfe};
        constexpr u32 expected = 0xbabecafe;
        EXPECT_EQ(deserializeU32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0xff, 0xff, 0xff, 0xff};
        constexpr u32 expected = 0xffffffff;
        EXPECT_EQ(deserializeU32_BE(b), expected);
    }
    {
        constexpr Byte b[4] = {0x12, 0x34, 0x56, 0x78};
        constexpr u32 expected = 0x12345678;
        EXPECT_EQ(deserializeU32_BE(b), expected);
    }
}

TEST(Deserialize, I16_LE)
{
    {
        constexpr Byte b[2] = {0x00, 0x00};
        constexpr i16 expected = 0x0000;
        EXPECT_EQ(deserializeI16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x01, 0x00};
        constexpr i16 expected = 0x0001;
        EXPECT_EQ(deserializeI16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0x7f};
        constexpr i16 expected = INT16_MAX;
        EXPECT_EQ(deserializeI16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x00, 0x80};
        constexpr i16 expected = INT16_MIN;
        EXPECT_EQ(deserializeI16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xfe, 0xca};
        constexpr i16 expected = -13570;  // 0xcafe
        EXPECT_EQ(deserializeI16_LE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0xff};
        constexpr i16 expected = -1;
        EXPECT_EQ(deserializeI16_LE(b), expected);
    }
}

TEST(Deserialize, I16_BE)
{
    {
        constexpr Byte b[2] = {0x00, 0x00};
        constexpr i16 expected = 0x0000;
        EXPECT_EQ(deserializeI16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x00, 0x01};
        constexpr i16 expected = 0x0001;
        EXPECT_EQ(deserializeI16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x7f, 0xff};
        constexpr i16 expected = INT16_MAX;
        EXPECT_EQ(deserializeI16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0x80, 0x00};
        constexpr i16 expected = INT16_MIN;
        EXPECT_EQ(deserializeI16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xca, 0xfe};
        constexpr i16 expected = -13570;  // 0xcafe
        EXPECT_EQ(deserializeI16_BE(b), expected);
    }
    {
        constexpr Byte b[2] = {0xff, 0xff};
        constexpr i16 expected = -1;
        EXPECT_EQ(deserializeI16_BE(b), expected);
    }
}
