/*
 * rtctimer.c
 *
 * Created: 3/15/2013 4:21:43 PM
 *  Author: kweekly
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint32_t time_val;

ISR(TIMER2_OVF_vect) {
	time_val++;
}


void rtctimer_init() {
	TCCR2A = 0;
	TCCR2B = 0x05; // 128 prescaler, (1 second period)
	ASSR = 0x20; // set external crystal
	TIFR2 = 0x01; //overflow interrupt enabled
	time_val = 0;
}

uint32_t rtctimer_read() {
	
}

void rtctimer_set_periodic_alarm(uint16_t period_s, void (* cb)(void)) {
	
}