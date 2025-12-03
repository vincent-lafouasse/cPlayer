#include "codec_internals.h"

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
