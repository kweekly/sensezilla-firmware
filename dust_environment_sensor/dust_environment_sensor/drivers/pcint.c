/*
 * pcint.c
 *
 * Created: 3/27/2013 2:35:06 PM
 *  Author: kweekly
 */ 

#include "all.h"
#define PCINT_NUM_PINS 32

void (*pcint_callbacks[PCINT_NUM_PINS])(void);
volatile uint8_t pcint_cbflags[PCINT_NUM_PINS/8];

volatile uint8_t pcint_latA;
volatile uint8_t pcint_latB;
volatile uint8_t pcint_latC;
volatile uint8_t pcint_latD;

ISR(PCINT0_vect) {
	uint8_t rval = PINA;
	pcint_cbflags[0] |= rval ^ pcint_latA;
	pcint_latA = rval;
}

ISR(PCINT1_vect) {
	uint8_t rval = PINB;
	pcint_cbflags[1] |= rval ^ pcint_latB;
	pcint_latB = rval;
}

ISR(PCINT2_vect) {
	uint8_t rval = PINC;
	pcint_cbflags[2] |= rval ^ pcint_latC;
	pcint_latC = rval;
}

ISR(PCINT3_vect) {
	uint8_t rval = PIND;
	pcint_cbflags[3] |= rval ^ pcint_latD;
	pcint_latD = rval;
}


void pcint_init() {
	for (int c = 0; c < PCINT_NUM_PINS; c++)
		pcint_callbacks[c] = NULL;
		
	// disable all interrupts
	PCICR = 0;
	pcint_latchall();
}

void pcint_check() {
	for ( int c = 0; c < PCINT_NUM_PINS / 8; c++) {
		if ( pcint_cbflags[c] )
			for ( int d = 0; d < 8; d++ ) {
				if ( pcint_cbflags[c] & _BV(d) && pcint_callbacks[c*8+d]) {
					pcint_callbacks[c*8+d]();
					pcint_cbflags[c] &= ~_BV(d);
				}
			}
	}
}

void pcint_latchall() {
	pcint_latA = PINA;
	pcint_latB = PINB;
	pcint_latC = PINC;
	pcint_latD = PIND;
}

void pcint_register(uint8_t intnum, void (*cb)(void)) {
	uint8_t regnum = (int)(intnum/8);
	uint8_t bitnum = intnum % 8;
	
	switch(regnum) {
		case 0:
			PCMSK0 |= _BV(bitnum);
			break;
		case 1:
			PCMSK1 |= _BV(bitnum);
			break;
		case 2:
			PCMSK2 |= _BV(bitnum);
			break;
		case 3:
			PCMSK3 |= _BV(bitnum);
			break;
		default:
			printf_P(PSTR("Error registering pcint: Too high of an intnum specified: %d"),intnum);
			return;
	}		
	
	PCICR |= _BV(regnum);
	pcint_callbacks[intnum] = cb;
}

void pcint_unregister(uint8_t intnum) {
	uint8_t regnum = (int)(intnum/8);
	uint8_t bitnum = intnum % 8;
	
	switch(regnum) {
		case 0:
		PCMSK0 &= ~_BV(bitnum);
		if (PCMSK0 == 0) PCICR &= ~1;
		break;
		case 1:
		PCMSK1 &= ~_BV(bitnum);
		if (PCMSK1 == 0) PCICR &= ~2;
		break;
		case 2:
		PCMSK2 &= ~_BV(bitnum);
		if (PCMSK2 == 0) PCICR &= ~4;
		break;
		case 3:
		PCMSK3 &= ~_BV(bitnum);
		if (PCMSK3 == 0) PCICR &= ~8;
		break;
		default:
		printf_P(PSTR("Error un-registering pcint: Too high of an intnum specified: %d"),intnum);
		return;
	}
	
	pcint_callbacks[intnum] = NULL;
}