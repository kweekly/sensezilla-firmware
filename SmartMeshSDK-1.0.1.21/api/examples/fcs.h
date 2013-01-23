/* Copyright (c) 2003, Dust, Inc.  All rights reserved. */

#ifndef FCS_H
#define FCS_H

#include <stdint.h>

#define FCS_INITIAL_FCS16 ((uint16_t) 0xffff)
#define FCS_GOOD_FCS16    ((uint16_t) 0xf0b8)

uint16_t fcs_calcFcs(int8_t* buf, int32_t len);
uint16_t fcs_fcs16(uint16_t fcs, uint8_t data);

#endif
