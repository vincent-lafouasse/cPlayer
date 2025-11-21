#include "gtest/gtest.h"

extern "C" {
#include "int24.h"
}

TEST(i24, PositiveNumbers)
{
    EXPECT_EQ(i24_asI32(Int24{0, 0, 0}), 0x0);
    EXPECT_EQ(i24_asI32(Int24{0, 0, 1}), 0x1);
    EXPECT_EQ(i24_asI32(Int24{0, 1, 0}), 0x100);
    EXPECT_EQ(i24_asI32(Int24{1, 0, 0}), 0x10000);
    EXPECT_EQ(i24_asI32(Int24{0x11, 0x11, 0x11}), 0x111111);
    EXPECT_EQ(i24_asI32(Int24{0x7f, 0xff, 0xff}), 0x007fffff);
}

TEST(i24, NegativeNumbers)
{
    EXPECT_EQ(i24_asI32(Int24{0xff, 0xff, 0xff}), -1);
}
