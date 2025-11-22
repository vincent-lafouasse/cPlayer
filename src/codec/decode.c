#include "decode.h"

#include "audio.h"
#include "wav/wav.h"

AudioDataResult decodeAudio(FileReader* reader)
{
    const AudioDataResult maybeTrack = decodeWav(reader);
    if (maybeTrack.err != NoError) {
        return maybeTrack;
    }

    return (AudioDataResult){.track = maybeTrack.track, .err = NoError};
}
