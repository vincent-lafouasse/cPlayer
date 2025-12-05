#include "Error.h"
#include "audio.h"
#include "int24.h"
#include "wav_internals.h"

#include <stdlib.h>

AudioDataResult decodeWav(Reader* reader)
{
    WavFormatInfo format;
    const Error err = readWavFormatInfo(reader, &format);
    if (err != NoError) {
        return AudioDataResult_Err(err);
    }
    logWavFormatInfo(&format);

    return readWavData(reader, &format);
}
