#include "hdlc.h"
#include "fcs.h"
#include <string.h> // for memset

#define HDLC_FRAME_BYTE     0x7E
#define HDLC_ESCAPE_BYTE    0x7D


/*
 * Decode an HDLC packet into destination (strip framing and CRC)
 * Arguments:
 *    pOut:      Destination to write decoded data
 *    pIn:       HDLC data to decode
 *    inLen:     Length of input HDLC data
 *    maxOutLen: Max output length  
 * Returns:
 *    > 0: length of decoded data
 *    < 0: error code
 */
int32_t hdlc_decode(uint8_t* pOut, uint8_t* pIn, uint32_t inLen, uint32_t maxOutLen)
{
   uint32_t i;
   uint32_t wIdx = 0;
   uint16_t calcCrc = FCS_INITIAL_FCS16;
   uint8_t fOpenFlag = 0;
   uint8_t fDataStart = 0;
   uint8_t curByte;
   uint8_t fEscape = 0;
   
   for (i = 0; i < inLen; i++) {
      curByte = pIn[i];
      if (curByte == HDLC_FRAME_BYTE) {
         if (!fOpenFlag) {
            fOpenFlag = 1;
         }
         else if (fDataStart) {
            if (calcCrc == FCS_GOOD_FCS16) {
               // return length excluding CRC bytes
               return (wIdx - 2);
            }
            else {
               return HDLC_ERR_CRC;
            }
         }
      }
      else if (curByte == HDLC_ESCAPE_BYTE) {
         fEscape = 1;
      }
      else {
         if (fOpenFlag) {
            fDataStart = 1;
            if (fEscape) {
               fEscape = 0;
               pOut[wIdx] = (curByte ^ 0x20);
            }
            else {
               pOut[wIdx] = curByte;
            }
            if (wIdx > maxOutLen) {
               return HDLC_ERR_LEN;
            }
            calcCrc = fcs_fcs16(calcCrc, pOut[wIdx]);
            wIdx++;
         }
      }
   }
   return HDLC_ERR_FRAME;
}

/*
 * HDLC stuff byte into destination buffer,
 * inserts escape bytes where appropriate
 * Arguments:
 *    pDest:  destination pointer for encoded bytes
 *    byte:   byte to encode
 * Returns:
 *    Number of bytes written to destination (1 or 2 if escaped) 
 */
uint8_t hdlc_stuffByte(uint8_t* pDest, uint8_t byte)
{
   if (byte == HDLC_FRAME_BYTE || byte == HDLC_ESCAPE_BYTE) {
      *pDest = HDLC_ESCAPE_BYTE;
      *(pDest+1) = byte ^ 0x20;
      return 2;
   }
   else {
      *pDest = byte;
      return 1;
   }
}

/*
 * Encode an input buffer into an HDLC frame, 
 * performs byte stuffing and adds CRC
 * Arguments:
 *    pOut:  destination pointer for encoded bytes
 *    pIn:   pointer to unencoded source data
 *    inLen:     Length of input data
 *    maxOutLen: Max output length  
 * Returns:
 *    Number of bytes written to destination 
 */
int32_t hdlc_encode(uint8_t* pOut, uint8_t* pIn, uint32_t inLen, uint32_t maxOutLen)
{
   uint16_t i;
   uint16_t wIdx = 0;
   uint16_t txFcs = FCS_INITIAL_FCS16;
   uint8_t fcsHi, fcsLo;
   
   pOut[wIdx++] = HDLC_FRAME_BYTE;
   for (i = 0; i < inLen; i++) {
      wIdx += hdlc_stuffByte(&pOut[wIdx], pIn[i]);
      txFcs = fcs_fcs16(txFcs, pIn[i]);
      if (wIdx > (maxOutLen - 4)) {
         return HDLC_ERR_LEN; 
      }
   }
   // append CRC
   txFcs = ~txFcs;
   fcsHi = (txFcs & 0xFF);
   fcsLo = ((txFcs >> 8) & 0xFF);
   wIdx += hdlc_stuffByte(&pOut[wIdx], fcsHi);
   wIdx += hdlc_stuffByte(&pOut[wIdx], fcsLo);
   pOut[wIdx++] = HDLC_FRAME_BYTE;
   return wIdx;
}

