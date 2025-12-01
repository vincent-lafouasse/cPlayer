#include "gtest/gtest.h"

extern "C" {
#include "Options.h"
}

TEST(ParseOptions, HeadlessFlagLong)
{
    const char* args[] = {"--headless", "--input", "file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_EQ(res.err, NoError);
    EXPECT_TRUE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}

TEST(ParseOptions, HeadlessFlagShort)
{
    const char* args[] = {"-h", "--input", "file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_EQ(res.err, NoError);
    EXPECT_TRUE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}

TEST(ParseOptions, InputFlagLong)
{
    const char* args[] = {"--input", "file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_EQ(res.err, NoError);
    EXPECT_FALSE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}

TEST(ParseOptions, InputFlagShort)
{
    const char* args[] = {"-i", "file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_EQ(res.err, NoError);
    EXPECT_FALSE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}

TEST(ParseOptions, PositionalInput)
{
    const char* args[] = {"file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_EQ(res.err, NoError);
    EXPECT_FALSE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}

TEST(ParseOptions, FlagBeforePositional)
{
    const char* args[] = {"--headless", "file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_EQ(res.err, NoError);
    EXPECT_TRUE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}

TEST(ParseOptions, UnknownFlag)
{
    const char* args[] = {"--foo", "file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    EXPECT_EQ(res.err, E_Unknown_Flag);
}

TEST(ParseOptions, MissingInputForFlag)
{
    const char* args[] = {"--input"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    EXPECT_EQ(res.err, E_Bad_Usage);
}

TEST(ParseOptions, ManyPositionals)
{
    const char* args[] = {"file1.wav", "file2.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    EXPECT_EQ(res.err, NoError);
    EXPECT_STREQ(res.options.input, "file2.wav");
}

TEST(ParseOptions, NoInputProvided)
{
    const char* args[] = {"--headless"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    EXPECT_EQ(res.err, E_Bad_Usage);
}

TEST(ParseOptions, MixedFlagsAndPositionals)
{
    const char* args[] = {"file.wav", "-h"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_EQ(res.err, NoError);
    EXPECT_TRUE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}
