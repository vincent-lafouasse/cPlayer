#include <cstdint>
#include <fstream>

#include "gtest/gtest.h"

extern "C" {
#include "FileReader.h"
}

const std::string prefix = "../build/test/";

void writeFile(const std::string& path, const std::vector<uint8_t>& data)
{
    std::ofstream os(path);
    for (uint8_t byte : data) {
        os << byte;
    }
}

TEST(Read, ReadBytes)
{
    const std::vector<uint8_t> data = {0, 0};
    const std::string name = "dummy";

    const std::string path = prefix + name;
    writeFile(path, data);

    FileReader r = fr_new(path.c_str());

    uint8_t byte;

    for (uint8_t expected : data) {
        ASSERT_EQ(fr_takeByte(&r, &byte), Read_Ok);
        ASSERT_EQ(byte, expected);
    }
    ASSERT_EQ(fr_takeByte(&r, &byte), Read_Done);

    fr_close(&r);
}
