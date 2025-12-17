#include "decode.h"

#include "libcodec_wav.h"

Error decodeAudio(Reader* reader, AudioBuffer* out)
{
    WavError err = decodeWav(reader, out);
    if (err != EWav_Ok) {
        return err_Err(E_Wav, err);
    }

    return err_Ok();
}
