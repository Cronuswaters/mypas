/**@<errcodes.h>**/
#pragma once
enum{
	ERR_INVALID_FILE = -2,
    ERR_FILE_ERROR,
    ERR_TOKEN_MISMATCH = 0xF0000,
    ERR_UNKNOWN_TYPE,
    ERR_PREMATURE_EOF,
	ERR_CAST_ERROR,
	ERR_TYPE_MISMATCH,
};
