/*
 * amn41121.c
 *
 * Created: 3/12/2013 5:49:45 PM
 *  Author: kweekly
 */ 
#include  "devicedefs.h"

#ifdef USE_AMN41121
#include "avrincludes.h"
#include "devices/amn41121.h"
#include "protocol/report.h"

#include "utils/scheduler.h"
#include "drivers/rtctimer.h"
#include "drivers/pcint.h"


// upper byte is seconds, lower byte is 1/256 of seconds
uint16_t pir_accH;
uint16_t pir_accL;
uint16_t pir_latch;

#define INT_LATCHH 0x01
#define INT_LATCHL 0x02
uint8_t int_bitmask;
uint8_t int_time_before_release;
uint32_t int_interrupt_started;

void _pir_pcint_cb() {
	uint16_t tval = (uint16_t)(rtctimer_read() << 8) | rtctimer_read_low();
	uint16_t tdiff;
	if ( tval > pir_latch ) {
		tdiff = tval - pir_latch;
	} else { // rollover occurred
		tdiff = (uint32_t)(tval) + 0x10000 - pir_latch;
	}
	if ( PIR_OUT_PIN ) { // went high from being low
		pir_accL += tdiff;
		int_bitmask |= INT_LATCHH;
	} else { // went low from being high
		int_bitmask |= INT_LATCHL;
		pir_accH += tdiff;
	}	
	pir_latch = tval;
}

void pir_wake(void) {
	PIR_VCC = 1;
	// should wait for at least a while
	// datasheet says 7s (could be up to 30s)
	
	// register the interrupt
	pir_accH = pir_accL = 0;
	pir_latch = (uint16_t)(rtctimer_read() << 8) | rtctimer_read_low();
	pcint_register(PIR_OUT_PCINT,&_pir_pcint_cb);
}

void pir_sleep(void) { 
	// unregister the interrupt
	pcint_unregister(PIR_OUT_PCINT);
	PIR_VCC = 0;
}

void pir_init() {
	pir_wake();
	pir_accH = pir_accL = 0;
}

void pir_setup_reporting_schedule(uint16_t starttime) {
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,PIR_TASK_ID,starttime,&_pir_reporting_write_report);	
}

void _pir_reporting_write_report() {
	if ( pir_accH + pir_accL ) 
		report_current()->occupancy = (float)pir_accH/(pir_accH + pir_accL);
	else 
		report_current()->occupancy = 0;
		
	report_current()->fields |= REPORT_TYPE_OCCUPANCY;
	pir_accH = pir_accL = 0;
	pir_latch = (uint16_t)(rtctimer_read() << 8) | rtctimer_read_low();
}


void pir_setup_interrupt_schedule(uint16_t starttime) {
	pir_set_interrupt_params(10);
	scheduler_add_task(SCHEDULER_MONITOR_LIST, PIR_TASK_ID, starttime, &_pir_interrupt_write_report);	
}

void pir_set_interrupt_params(uint8_t time_before_release) {
	int_time_before_release = time_before_release;
	int_bitmask = 0;
}

void _pir_interrupt_write_report() {
	uint32_t curtime = rtctimer_read();
	if ( int_bitmask & INT_LATCHH ) {
		if ( int_interrupt_started == 0 ) {
			report_current()->fields |= REPORT_TYPE_OCCUPANCY_CHANGED;
			report_current()->occupancy_state = 1;
		}
		int_interrupt_started = curtime;
		int_bitmask = 0;
	}
	if ( int_interrupt_started && (curtime - int_interrupt_started > int_time_before_release) ) {
		report_current()->fields |= REPORT_TYPE_OCCUPANCY_CHANGED;
		report_current()->occupancy_state = 0;
		int_interrupt_started = 0;
	}
	
}

void pir_fmt_reading(float * reading, uint8_t maxlen, char * str) {
	snprintf_P(str,maxlen,PSTR("Occupancy=%-5.1f%%"),(100.0* *reading));
}

#endif