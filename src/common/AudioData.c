#include "audio.h"

AudioDataResult AudioDataResult_Ok(AudioData track)
{
    return (AudioDataResult){.track = track, .err = err_Ok()};
}

AudioDataResult AudioDataResult_Err(Error err)
{
    return (AudioDataResult){.err = err};
}
