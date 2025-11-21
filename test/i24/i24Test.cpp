#include "gtest/gtest.h"

extern "C" {
#include "int24.h"
}

TEST(i24, Dummy)
{
    const Int24 i{0, 0, 0};
    EXPECT_EQ(i24_asI32(i), 0);
}
