#include "wav_internals.h"

Error validateWavFormatChunk(const WavFormatChunk* format, SampleFormat* out)
{
    // Check number of channels (we only accept mono/stereo eventually)
    if (format->nChannels == 0 || format->nChannels > 2)
        return err_withCtx(E_Codec, ECdc_UnsupportedChannelLayout,
                           format->nChannels);

    // Check sample rate sanity (arbitrary max = 192 kHz)
    if (format->sampleRate == 0 || format->sampleRate > 192000)
        return err_withCtx(E_Codec, ECdc_AbsurdSampleRate, format->sampleRate);

    // Check bits per sample / block align consistency
    if (format->bitDepth == 0 || format->bitDepth > 64)
        return err_withCtx(E_Wav, EWav_InvalidBitDepth, format->bitDepth);

    // PCM sanity: blockAlign must match nChannels * bytesPerSample
    uint16_t bytesPerSample = (format->bitDepth + 7) / 8;
    if (format->formatTag == WAVE_FORMAT_PCM ||
        format->formatTag == WAVE_FORMAT_IEEE_FLOAT) {
        if (format->blockAlign != bytesPerSample * format->nChannels)
            return err_Err(E_Wav, EWav_BlockAlignMismatch);
    }

    // Extension size sanity
    if (format->size < 16)
        return err_withCtx(E_Wav, EWav_FormatChunkTooSmall, format->size);
    if (format->size == 18 && format->extensionSize != 0)
        return err_Err(E_Wav, EWav_ExtensionSizeMismatch);
    if (format->size == 40 && format->extensionSize != 22)
        return err_Err(E_Wav, EWav_ExtensionSizeMismatch);

    // Sample format mapping
    switch (format->formatTag) {
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
                    return err_withCtx(E_Wav, EWav_UnsupportedBitDepth,
                                       format->bitDepth);
            }
            break;
        case WAVE_FORMAT_IEEE_FLOAT:
            if (format->bitDepth == 32)
                *out = SampleFormat_Float32;
            else if (format->bitDepth == 64)
                *out = SampleFormat_Float64;
            else
                return err_withCtx(E_Wav, EWav_UnsupportedBitDepth,
                                   format->bitDepth);
            break;
        case WAVE_FORMAT_ADPCM:
            *out = SampleFormat_ADPCM;  // no validation yet
            break;
        case WAVE_FORMAT_MULAW:
            *out = SampleFormat_MULAW;  // no validation yet
            break;
        case WAVE_FORMAT_ALAW:
            *out = SampleFormat_ALAW;  // no validation yet
            break;
        default:
            return err_withCtx(E_Wav, EWav_UnknownSampleFormat,
                               format->formatTag);
    }

    return err_Ok();
}
