#include "libcodec_wav.h"
#include "wav_internals.h"

#include <stdlib.h>

#include "log.h"

static WavError readMonoPCM(Reader* reader,
                            const WavFormatInfo* format,
                            AudioBuffer* out);
static WavError readStereoPCM(Reader* reader,
                              const WavFormatInfo* format,
                              AudioBuffer* out);

WavError readWavData(Reader* reader,
                     const WavFormatInfo* format,
                     AudioBuffer* out)
{
    if (format->formatTag != WAVE_FORMAT_PCM) {
        return EWav_UnsupportedSampleFormat;
    }

    logFn(LogLevel_Debug,
          "start of the data _segment_, (data data not data chunk): %u\n",
          reader->offset);

    if (format->nChannels == 1) {
        return readMonoPCM(reader, format, out);
    } else if (format->nChannels == 2) {
        return readStereoPCM(reader, format, out);
    } else {
        return EWav_UnsupportedChannelLayout;
    }
}

static WavError readMonoPCM(Reader* reader,
                            const WavFormatInfo* format,
                            AudioBuffer* out)
{
    WavError err = EWav_Ok;

    AudioBuffer track = audiobuffer_new(format->nFrames, 1, format->sampleRate);
    if (track.data == NULL) {
        err = EWav_OOM;
        goto out;
    }

    for (uint32_t i = 0; i < format->nFrames; ++i) {
        err = readSample(reader, format, track.data[0] + i);
        if (err != EWav_Ok) {
            goto out;
        }
    }
    dumpFloatCsv(track.data[0], format->nFrames,
                 DUMP_PREFIX "float" DUMP_SUFFIX);

out:
    if (err != EWav_Ok) {
        *out = track;
        return EWav_Ok;
    } else {
        audiobuffer_destroy(&track);
        return err;
    }
}

static WavError readStereoFrame(Reader* reader,
                                const WavFormatInfo* format,
                                float* left,
                                float* right)
{
    TRY(readSample(reader, format, left));
    TRY(readSample(reader, format, right));
    return EWav_Ok;
}

static WavError readStereoPCM(Reader* reader,
                              const WavFormatInfo* format,
                              AudioBuffer* out)
{
    WavError err = EWav_Ok;

    AudioBuffer track = audiobuffer_new(format->nFrames, 2, format->sampleRate);
    if (track.data == NULL) {
        err = EWav_OOM;
        goto out;
    }

    float* left = track.data[0];
    float* right = track.data[1];

    for (uint32_t i = 0; i < format->nFrames; ++i) {
        err = readStereoFrame(reader, format, left + i, right + i);
        if (err != EWav_Ok) {
            goto out;
        }
    }

out:
    if (err != EWav_Ok) {
        audiobuffer_destroy(&track);
        return err;
    } else {
        *out = track;
        return EWav_Ok;
    }
}
