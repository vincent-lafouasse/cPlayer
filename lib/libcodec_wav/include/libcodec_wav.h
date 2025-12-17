#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "libaudiospecs.h"
#include "libstream/Reader.h"

typedef enum {
    EWav_Ok,
    EWav_UnknownFourCC,
    EWav_UnsupportedSampleFormat,
    EWav_InvalidBitDepth,
    EWav_BlockAlignMismatch,
    EWav_FormatChunkTooSmall,
    EWav_ExtensionSizeMismatch,
    EWav_UnsupportedBitDepth,
    EWav_UnknownSampleFormat,
    EWav_UnknownGuidSubformat,
    EWav_UnknownFormatTag,
    EWav_ReadFailed,
    EWav_UnexpectedEOF,
    EWav_UnsupportedChannelLayout,
    EWav_OOM,
    EWav_AbsurdSampleRate,
} WavError;

WavError decodeWav(Reader* reader, AudioBuffer* out);

#ifdef __cplusplus
}
#endif
