#include "wav_internals.h"

#include "Error.h"

Error validateWavFormatChunk(const WavFormatChunk* format)
{
    if (format->formatTag != WAVE_FORMAT_PCM &&
        format->formatTag != WAVE_FORMAT_ADPCM &&
        format->formatTag != WAVE_FORMAT_IEEE_FLOAT &&
        format->formatTag != WAVE_FORMAT_ALAW &&
        format->formatTag != WAVE_FORMAT_MULAW &&
        format->formatTag != WAVE_FORMAT_EXTENSIBLE) {
        return err_withCtx(E_Wav, EWav_UnknownFormatTag, format->formatTag);
    }
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

    return err_Ok();
}
