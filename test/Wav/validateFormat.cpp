
#include "gtest/gtest.h"

#include "Error.h"
#include "wav_internals.h"

TEST(WavValidator, ValidPCM16Mono)
{
    WavFormatChunk fmt = {};
    fmt.formatTag = WAVE_FORMAT_PCM;
    fmt.nChannels = 1;
    fmt.sampleRate = 44100;
    fmt.bitDepth = 16;
    fmt.blockAlign = 2;  // mono i16
    fmt.size = 16;
    fmt.extensionSize = 0;

    EXPECT_TRUE(err_isOk(validateWavFormatChunk(&fmt)));
}

TEST(WavValidator, ValidPCM16Stereo)
{
    WavFormatChunk fmt = {};
    fmt.formatTag = WAVE_FORMAT_PCM;
    fmt.nChannels = 2;
    fmt.sampleRate = 48000;
    fmt.bitDepth = 16;
    fmt.blockAlign = 4;  // stereo i16
    fmt.size = 16;
    fmt.extensionSize = 0;

    EXPECT_TRUE(err_isOk(validateWavFormatChunk(&fmt)));
}

TEST(WavValidator, ValidFloat32Stereo)
{
    WavFormatChunk fmt = {};
    fmt.formatTag = WAVE_FORMAT_IEEE_FLOAT;
    fmt.nChannels = 2;
    fmt.sampleRate = 44100;
    fmt.bitDepth = 32;
    fmt.blockAlign = 8;  // stereo f32
    fmt.size = 16;
    fmt.extensionSize = 0;

    EXPECT_TRUE(err_isOk(validateWavFormatChunk(&fmt)));
}

TEST(WavValidator, AbsurdSampleRate)
{
    WavFormatChunk fmt = {};
    fmt.formatTag = WAVE_FORMAT_PCM;
    fmt.nChannels = 2;
    fmt.sampleRate = 1000000;  // too high
    fmt.bitDepth = 16;
    fmt.blockAlign = 4;
    fmt.size = 16;
    fmt.extensionSize = 0;

    Error err = validateWavFormatChunk(&fmt);
    ASSERT_EQ(err_category(err), E_Codec);
    ASSERT_EQ(err_subCategory(err), ECdc_AbsurdSampleRate);
}

TEST(WavValidator, ZeroChannels)
{
    WavFormatChunk fmt = {};
    fmt.formatTag = WAVE_FORMAT_PCM;
    fmt.nChannels = 0;
    fmt.sampleRate = 44100;
    fmt.bitDepth = 16;
    fmt.blockAlign = 0;
    fmt.size = 16;
    fmt.extensionSize = 0;

    Error err = validateWavFormatChunk(&fmt);
    ASSERT_EQ(err_category(err), E_Codec);
    ASSERT_EQ(err_subCategory(err), ECdc_UnsupportedChannelLayout);
}

TEST(WavValidator, BlockAlignMismatch)
{
    WavFormatChunk fmt = {};
    fmt.formatTag = WAVE_FORMAT_PCM;
    fmt.nChannels = 2;
    fmt.sampleRate = 44100;
    fmt.bitDepth = 16;
    fmt.blockAlign = 3;  // should be 4
    fmt.size = 16;
    fmt.extensionSize = 0;

    Error err = validateWavFormatChunk(&fmt);
    ASSERT_EQ(err_category(err), E_Wav);
    ASSERT_EQ(err_subCategory(err), EWav_BlockAlignMismatch);
}

TEST(WavValidator, FormatChunkTooSmall)
{
    WavFormatChunk fmt = {};
    fmt.formatTag = WAVE_FORMAT_PCM;
    fmt.nChannels = 2;
    fmt.sampleRate = 44100;
    fmt.bitDepth = 16;
    fmt.blockAlign = 4;
    fmt.size = 12;  // < 16
    fmt.extensionSize = 0;

    Error err = validateWavFormatChunk(&fmt);
    ASSERT_EQ(err_category(err), E_Wav);
    ASSERT_EQ(err_subCategory(err), EWav_FormatChunkTooSmall);
}

TEST(WavValidator, ExtensionSizeMismatch_18Bytes)
{
    WavFormatChunk fmt = {};
    fmt.formatTag = WAVE_FORMAT_PCM;
    fmt.nChannels = 2;
    fmt.sampleRate = 44100;
    fmt.bitDepth = 16;
    fmt.blockAlign = 4;
    fmt.size = 18;
    fmt.extensionSize = 2;  // should be 0

    Error err = validateWavFormatChunk(&fmt);
    ASSERT_EQ(err_category(err), E_Wav);
    ASSERT_EQ(err_subCategory(err), EWav_ExtensionSizeMismatch);
}

TEST(WavValidator, ExtensionSizeMismatch_40Bytes)
{
    WavFormatChunk fmt = {};
    fmt.formatTag = WAVE_FORMAT_PCM;
    fmt.nChannels = 2;
    fmt.sampleRate = 44100;
    fmt.bitDepth = 16;
    fmt.blockAlign = 4;
    fmt.size = 40;
    fmt.extensionSize = 10;  // should be 22

    Error err = validateWavFormatChunk(&fmt);
    ASSERT_EQ(err_category(err), E_Wav);
    ASSERT_EQ(err_subCategory(err), EWav_ExtensionSizeMismatch);
}

TEST(WavValidator, UnknownFormatTag)
{
    WavFormatChunk fmt = {};
    fmt.formatTag = 0x1234;  // unknown
    fmt.nChannels = 1;
    fmt.sampleRate = 44100;
    fmt.bitDepth = 16;
    fmt.blockAlign = 2;
    fmt.size = 16;
    fmt.extensionSize = 0;

    Error err = validateWavFormatChunk(&fmt);
    ASSERT_EQ(err_category(err), E_Wav);
    ASSERT_EQ(err_subCategory(err), EWav_UnknownFormatTag);
}
