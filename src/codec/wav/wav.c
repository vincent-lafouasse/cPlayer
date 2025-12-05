#include "wav_internals.h"

#include "Error.h"
#include "audio.h"

AudioDataResult decodeWav(Reader* reader)
{
    WavFormatInfo format;
    const Error err = readWavFormatInfo(reader, &format);
    if (err != NoError) {
        return AudioDataResult_Err(err_fromLegacy(err));
    }
    logWavFormatInfo(&format);

    return readWavData(reader, &format);
}
