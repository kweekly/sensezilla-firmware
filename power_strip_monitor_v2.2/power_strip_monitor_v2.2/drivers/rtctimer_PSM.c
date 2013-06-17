/*
 * rtctimer.c
 *
 * Created: 3/15/2013 4:21:43 PM
 *  Author: kweekly
 */ 
#include "all.h"

volatile uint32_t time_val;

volatile uint16_t alarm_reload_val;
volatile uint16_t alarm_val;

void (* alarm_cb)(void);

ISR(TIMER3_COMPA_vect) {
	time_val++;
	if ( alarm_val )
		alarm_val--;
}


void rtctimer_init() {
	OCR3A  = 16000; // 1 sec period
	TCCR3A = 0;
	TCCR3B = 0b00001101; // CTC mode 1024 prescaler ( tick period = 62.5uS )
	TIFR3 = 0xFF; //clear interrupts
	TIMSK3 = _BV(OCIE3A); // enable compare interrupt
	
	time_val = 0;
	alarm_cb = NULL;
}

char rtctimer_check_alarm() {
	if (alarm_cb != NULL && alarm_val == 0) {
		alarm_val = alarm_reload_val;
		alarm_cb();
		return 1;		
	}
	return 0;
}

void rtctimer_write(uint32_t t) {
	time_val = t;
}

uint32_t rtctimer_read() {
	return time_val;
}

uint8_t rtctimer_read_low() {
	return TCNT2;
}

void rtctimer_set_periodic_alarm(uint16_t period_s, void (* cb)(void)) {
	alarm_cb = cb;
	alarm_reload_val = alarm_val = period_s;
}

uint16_t rtctimer_get_alarm_period() {
	return alarm_reload_val;
}