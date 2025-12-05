#include "decode.h"

#include "audio.h"
#include "wav/wav.h"

AudioDataResult decodeAudio(Reader* reader)
{
    const AudioDataResult maybeTrack = decodeWav(reader);
    if (!err_isOk(maybeTrack.err)) {
        return maybeTrack;
    }

    return AudioDataResult_Ok(maybeTrack.track);
}
