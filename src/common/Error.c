#include "Error.h"
#include "FileReader.h"

const char* errorRepr(Error e)
{
    switch (e) {
        case NoError:
            return "No error";
        case E_OOM:
            return "Out of memory";

        case E_Bad_Usage:
            return "Bad usage";

        case E_FailedRead:
            return "Read failed";
        case E_UnexpectedEOF:
            return "Unexpected EOF when reading";

        case E_Codec_UnsupportedCodec:
            return "Unsupported codec";
        case E_Codec_UnsupportedChannelLayout:
            return "Unsupported channel layout";

        case E_Wav_UnknownFourCC:
            return "Unexpected RIFF 4CC";
        case E_Wav_UnsupportedSampleFormat:
            return "Unsupported sample format";
        default:
            return "Unkown error";
    }
}

Error error_fromReadStatus(ReadStatus readStatus)
{
    switch (readStatus) {
        case ReadStatus_Ok:
            return NoError;
        case ReadStatus_ReadErr:
            return E_FailedRead;
        case ReadStatus_EOF:
            return E_UnexpectedEOF;
        case ReadStatus_OOM:
            return E_OOM;
    }
}
