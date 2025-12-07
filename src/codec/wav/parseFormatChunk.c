#include "Error.h"
#include "wav_internals.h"

#include <string.h>

const uint8_t pcmGuid[16] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
                             0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71};

const uint8_t floatGuid[16] = {0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
                               0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71};

static bool guidEq(const uint8_t a[16], const uint8_t b[16])
{
    return memcmp(a, b, 16) == 0;
}

static Error assignSampleFormat(const WavFormatInfo* format, SampleFormat* out)
{
    // WAVE_FORMAT_EXTENSIBLE should have been folded into PCM or IEEE by this
    // point or entirely rejected
    switch (format->formatTag) {
        case WAVE_FORMAT_ADPCM:
            *out = SampleFormat_ADPCM;
            break;
        case WAVE_FORMAT_ALAW:
            *out = SampleFormat_ALAW;
            break;
        case WAVE_FORMAT_MULAW:
            *out = SampleFormat_MULAW;
            break;
        case WAVE_FORMAT_IEEE_FLOAT:
            switch (format->bitDepth) {
                case 32:
                    *out = SampleFormat_Float32;
                    break;
                case 64:
                    *out = SampleFormat_Float64;
                    break;
                default:
                    return err_Err(E_Wav, EWav_InvalidBitDepth);
            }
            break;
        case WAVE_FORMAT_PCM:
            switch (format->bitDepth) {
                case 8:
                    *out = SampleFormat_Unsigned8;
                    break;
                case 16:
                    *out = SampleFormat_Signed16;
                    break;
                case 24:
                    *out = SampleFormat_Signed24;
                    break;
                case 32:
                    *out = SampleFormat_Signed32;
                    break;
                default:
                    return err_Err(E_Wav, EWav_InvalidBitDepth);
            }
            break;
        default:
            // should be unreachable
            break;
    }
    return err_Ok();
}

Error parseFormatChunk(const WavFormatChunk* chunk, WavFormatInfo* out)
{
    memset(out, 0, sizeof(*out));
    *out = (WavFormatInfo){
        .formatTag = chunk->formatTag,
        .nChannels = chunk->nChannels,
        .sampleRate = chunk->sampleRate,
        // container size
        .blockAlign = chunk->blockAlign,
        // sample size, may be downsized if extended format
        .bitDepth = (chunk->blockAlign / chunk->nChannels) * 8,
    };

    if (chunk->formatTag == WAVE_FORMAT_EXTENSIBLE) {
        if (guidEq(chunk->subFormat, pcmGuid)) {
            out->formatTag = WAVE_FORMAT_PCM;
            out->bitDepth = chunk->validBitsPerSample;
        } else if (guidEq(chunk->subFormat, floatGuid)) {
            out->formatTag = WAVE_FORMAT_IEEE_FLOAT;
            out->bitDepth = chunk->validBitsPerSample;
        } else {
            return err_Err(E_Wav, EWav_UnknownGuidSubformat);
        }
    }
    TRY(assignSampleFormat(out, &out->sampleFormat));
    return err_Ok();
}

Error checkFormatSupport(const WavFormatInfo* format)
{
    (void)format;
    return err_Ok();
}
