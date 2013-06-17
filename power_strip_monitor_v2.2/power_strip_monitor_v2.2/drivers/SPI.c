/*
 * SPI.c
 *
 * Created: 5/15/2013 5:17:01 PM
 *  Author: kweekly
 */ 

#include "all.h"


void spi_init(uint8_t mode) {
	if ( mode & 0x80 ) {
		SPSR = 1;
	} else {
		SPSR = 0;
	}
	SPCR = 0b01010000 | (0x2F & mode);
}

uint8_t spi_transfer(uint8_t data) {
	SPDR = data;
	while ( (SPSR & _BV(SPIF)) == 0 );
	return SPDR;
}

