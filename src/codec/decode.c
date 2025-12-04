#include "decode.h"

#include "audio.h"
#include "wav/wav.h"

AudioDataResult decodeAudio(Reader* reader)
{
    const AudioDataResult maybeTrack = decodeWav(reader);
    if (maybeTrack.err != NoError) {
        return maybeTrack;
    }

    return AudioDataResult_Ok(maybeTrack.track);
}
