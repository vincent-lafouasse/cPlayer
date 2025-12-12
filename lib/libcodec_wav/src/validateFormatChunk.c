#include "libcodec_wav.h"
#include "wav_internals.h"

WavError validateWavFormatChunk(const WavFormatChunk* format)
{
    if (format->formatTag != WAVE_FORMAT_PCM &&
        format->formatTag != WAVE_FORMAT_ADPCM &&
        format->formatTag != WAVE_FORMAT_IEEE_FLOAT &&
        format->formatTag != WAVE_FORMAT_ALAW &&
        format->formatTag != WAVE_FORMAT_MULAW &&
        format->formatTag != WAVE_FORMAT_EXTENSIBLE) {
        return EWav_UnknownFormatTag;
    }
    // Check number of channels (we only accept mono/stereo eventually)
    if (format->nChannels == 0 || format->nChannels > 2)
        return EWav_UnsupportedChannelLayout;

    // Check sample rate sanity (arbitrary max = 192 kHz)
    if (format->sampleRate == 0 || format->sampleRate > 192000)
        return EWav_AbsurdSampleRate;

    // Check bits per sample / block align consistency
    if (format->bitDepth == 0 || format->bitDepth > 64)
        return EWav_InvalidBitDepth;

    // PCM sanity: blockAlign must match nChannels * bytesPerSample
    uint16_t bytesPerSample = (format->bitDepth + 7) / 8;
    if (format->formatTag == WAVE_FORMAT_PCM ||
        format->formatTag == WAVE_FORMAT_IEEE_FLOAT) {
        if (format->blockAlign != bytesPerSample * format->nChannels)
            return EWav_BlockAlignMismatch;
    }

    // Extension size sanity
    if (format->size < 16)
        return EWav_FormatChunkTooSmall;
    if (format->size == 18 && format->extensionSize != 0)
        return EWav_ExtensionSizeMismatch;
    if (format->size == 40 && format->extensionSize != 22)
        return EWav_ExtensionSizeMismatch;

    return EWav_Ok;
}
