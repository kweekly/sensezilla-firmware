#ifndef INCL_HDLC_H
#define INCL_HDLC_H

#include <stdint.h>

#define HDLC_ERR_CRC     -1
#define HDLC_ERR_FRAME   -2
#define HDLC_ERR_LEN     -3

int32_t hdlc_encode(uint8_t* pOut, uint8_t* pIn, uint32_t inLen, uint32_t maxOutLen);
int32_t hdlc_decode(uint8_t* pOut, uint8_t* pIn, uint32_t inLen, uint32_t maxOutLen);

#endif

