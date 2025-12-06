#include "wav_internals.h"

#include "audio.h"

AudioDataResult decodeWav(Reader* reader)
{
    WavFormatInfo format;
    const Error64 err = readWavFormatInfo(reader, &format);
    if (!err_isOk(err)) {
        return AudioDataResult_Err(err);
    }
    logWavFormatInfo(&format);

    return readWavData(reader, &format);
}
