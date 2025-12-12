#include "libcodec_wav.h"
#include "wav_internals.h"

WavError decodeWav(Reader* reader, AudioBuffer* out)
{
    WavFormatInfo format;
    TRY(readWavFormatInfo(reader, &format));
    logWavFormatInfo(&format);

    TRY(readWavData(reader, &format, out));
    return EWav_Ok;
}
