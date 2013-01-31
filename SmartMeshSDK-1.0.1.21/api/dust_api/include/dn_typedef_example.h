#ifndef __DN_TYPEDEF_H
#define __DN_TYPEDEF_H
typedef unsigned char  uint8_t;
typedef          char  INT8S;
typedef unsigned short uint16_t;
typedef          short INT16S;
typedef unsigned int   uint32_t;
typedef          int   int32_t;
typedef long     long  INT64S;
typedef unsigned char  BOOLEAN;

#define  PACKED_START  __pragma(pack(1))
#define  PACKED_STOP   __pragma(pack())
#if !defined(L_ENDIAN) && !defined(B_ENDIAN)
    #define L_ENDIAN
#endif 

#endif
