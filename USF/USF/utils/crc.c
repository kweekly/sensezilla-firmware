/*
 * crc.c
 *
 * Created: 1/14/2014 4:48:03 PM
 *  Author: kweekly
 */ 
#include <avr/io.h>
#include "utils/crc.h"

uint8_t crc8(const uint16_t CRC_POLY,const uint8_t * input, const uint8_t len)  {
	uint8_t rem = 0;
	for (uint8_t c = 0; c < len; c++) {
		rem ^= input[c];
		for (uint8_t b = 0; b < 8; b++) {
			if (rem & 0x80)
			rem = (rem<<1)^CRC_POLY;
			else
			rem	<<= 1;
		}
	}
	return rem;
}