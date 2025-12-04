#include "Error.h"
#include "audio.h"
#include "int24.h"
#include "wav_internals.h"

#include <stdlib.h>

AudioDataResult decodeWav(Reader* reader)
{
    Header wavHeader;
    const Error err = readWavHeader(reader, &wavHeader);
    if (err != NoError) {
        return AudioDataResult_Err(err);
    }
    logHeader(&wavHeader);

    return readWavData(reader, wavHeader);
}

static AudioDataResult readWavDataMono(Reader* reader, Header h);
static AudioDataResult readWavDataStereo(Reader* reader, Header h);

AudioDataResult readWavData(Reader* reader, Header h)
{
    if (h.nChannels == 1) {
        return readWavDataMono(reader, h);
    } else if (h.nChannels == 2) {
        return readWavDataStereo(reader, h);
    } else {
        return AudioDataResult_Err(E_Codec_UnsupportedChannelLayout);
    }
}

static AudioDataResult readWavDataMono(Reader* reader, Header h)
{
    Error err = NoError;

    float* left = malloc(h.size * sizeof(float));
    if (left == NULL) {
        err = E_OOM;
        goto out;
    }

    for (uint32_t i = 0; i < h.size; ++i) {
        err = readSample(reader, h.sampleFormat, left + i);
        if (err != NoError) {
            goto out;
        }
    }
    dumpFloatCsv(left, h.size, DUMP_PREFIX "float" DUMP_SUFFIX);

out:
    if (err == NoError) {
        const AudioData track = (AudioData){.left = left,
                                            .right = left,
                                            .size = h.size,
                                            .sampleRate = h.sampleRate};
        return AudioDataResult_Ok(track);
    } else {
        free(left);
        return AudioDataResult_Err(err);
    }
}

static Error readStereoFrame(Reader* reader,
                             SampleFormat format,
                             float* left,
                             float* right)
{
    TRY(readSample(reader, format, left));
    TRY(readSample(reader, format, right));
    return NoError;
}

static AudioDataResult readWavDataStereo(Reader* reader, Header h)
{
    Error err = NoError;

    float* left = malloc(h.size * sizeof(float));
    float* right = malloc(h.size * sizeof(float));
    if (left == NULL || right == NULL) {
        err = E_OOM;
        goto out;
    }

    for (uint32_t i = 0; i < h.size; ++i) {
        err = readStereoFrame(reader, h.sampleFormat, left + i, right + i);
        if (err != NoError) {
            goto out;
        }
    }

out:
    if (err != NoError) {
        free(left);
        free(right);
        return AudioDataResult_Err(err);
    } else {
        const AudioData track = (AudioData){.left = left,
                                            .right = right,
                                            .size = h.size,
                                            .sampleRate = h.sampleRate};
        return AudioDataResult_Ok(track);
    }
}
