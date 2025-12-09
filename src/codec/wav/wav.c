#include "wav_internals.h"

#include "audio.h"

Error decodeWav(Reader* reader, AudioData* out)
{
    WavFormatInfo format;
    TRY(readWavFormatInfo(reader, &format));
    logWavFormatInfo(&format);

    TRY(readWavData(reader, &format, out));
    return err_Ok();
}
