#include "Error.h"
#include "gtest/gtest.h"

extern "C" {
#include "options/Options.h"
}

TEST(ParseOptions, HeadlessFlag)
{
    const char* args[] = {"--headless", "--input", "file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_TRUE(err_isOk(res.err));
    EXPECT_TRUE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}

TEST(ParseOptions, InputFlagLong)
{
    const char* args[] = {"--input", "file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_TRUE(err_isOk(res.err));
    EXPECT_FALSE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}

TEST(ParseOptions, InputFlagShort)
{
    const char* args[] = {"-i", "file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_TRUE(err_isOk(res.err));
    EXPECT_FALSE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}

TEST(ParseOptions, PositionalInput)
{
    const char* args[] = {"file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_TRUE(err_isOk(res.err));
    EXPECT_FALSE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}

TEST(ParseOptions, FlagBeforePositional)
{
    const char* args[] = {"--headless", "file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_TRUE(err_isOk(res.err));
    EXPECT_TRUE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}

TEST(ParseOptions, UnknownFlag)
{
    const char* args[] = {"--foo", "file.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    EXPECT_EQ(err_category(res.err), E_Option);
    EXPECT_EQ(err_subCategory(res.err), EOpt_UnknownFlag);
    EXPECT_EQ(err_context(res.err), 0);  // args[0]
}

TEST(ParseOptions, MissingInputForFlag)
{
    const char* args[] = {"--input"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    EXPECT_EQ(err_category(res.err), E_Option);
    EXPECT_EQ(err_subCategory(res.err), EOpt_BadUsage);
    // EXPECT_EQ(err_context(res.err), 0); no context
}

TEST(ParseOptions, ManyPositionals)
{
    const char* args[] = {"file1.wav", "file2.wav"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_TRUE(err_isOk(res.err));
    EXPECT_STREQ(res.options.input, "file2.wav");
}

TEST(ParseOptions, NoInputProvided)
{
    const char* args[] = {"--headless"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    EXPECT_EQ(err_category(res.err), E_Option);
    EXPECT_EQ(err_subCategory(res.err), EOpt_BadUsage);
}

TEST(ParseOptions, MixedFlagsAndPositionals)
{
    const char* args[] = {"file.wav", "--headless"};
    const size_t sz = sizeof(args) / sizeof(*args);
    OptionsResult res = parseOptions(args, sz);

    ASSERT_TRUE(err_isOk(res.err));
    EXPECT_TRUE(res.options.headless);
    EXPECT_STREQ(res.options.input, "file.wav");
}
