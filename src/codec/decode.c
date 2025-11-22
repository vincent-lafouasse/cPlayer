#include "decode.h"

#include "wav/wav.h"

DecodingResult decodeAudio(FileReader* reader)
{
    const AudioData audio = decodeWav(reader);

    return (DecodingResult){.track = audio, .err = NoError};
}
