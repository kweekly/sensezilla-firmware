/*
 * triacs.c
 *
 * Created: 7/5/2013 1:20:42 PM
 *  Author: kweekly
 */ 

#include "devicedefs.h"
#ifdef POWER_STRIP_MONITOR
#include "avrincludes.h"

void turn_on_gate(char gateno) {
	if ( gateno == 6 ) {
		GATE6 = 1;
	} else if ( gateno >= 1 && gateno <= 5 ) {
		PORTB |= _BV(gateno-1);
	} else {
		printf_P(PSTR("Error: Invalid gate number %d\n"),gateno);
	}
}

void turn_off_gate(char gateno) {
	if ( gateno == 6 ) {
		GATE6 = 0;
	} else if ( gateno >= 1 && gateno <= 5 ) {
		PORTB &= ~_BV(gateno-1);
	} else {
		printf_P(PSTR("Error: Invalid gate number %d\n"),gateno);
	}
}

void toggle_gate(char gateno) {
	if ( gateno == 6 ) {
		GATE6 = !GATE6;
	} else if ( gateno >= 1 && gateno <= 5 ) {
		PINB = _BV(gateno-1);
	} else {
		printf_P(PSTR("Error: Invalid gate number %d\n"),gateno);
	}
}
#endif