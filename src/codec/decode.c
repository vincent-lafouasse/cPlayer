#include "decode.h"

#include "wav/wav.h"

Error decodeAudio(Reader* reader, AudioData* out)
{
    TRY(decodeWav(reader, out));
    return err_Ok();
}
