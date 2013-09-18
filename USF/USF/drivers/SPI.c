/*
 * SPI.c
 *
 * Created: 5/15/2013 5:17:01 PM
 *  Author: kweekly
 */ 
#include "devicedefs.h"
#include "avrincludes.h"

uint8_t old_spcr;


void spi_init(uint8_t mode) {
	if ( mode & 0x80 ) {
		SPSR = 1;
	} else {
		SPSR = 0;
	}
	SPCR = 0b01010000 | (0x2F & mode);
	old_spcr = SPCR;
}

uint8_t spi_transfer(uint8_t data) {
	SPDR = data;
	while ( (SPSR & _BV(SPIF)) == 0 );
	return SPDR;
}

void spi_pause() {
	old_spcr = SPCR;
	SPCR &= ~_BV(SPE);
}
void spi_resume() {
	SPCR = old_spcr;
}