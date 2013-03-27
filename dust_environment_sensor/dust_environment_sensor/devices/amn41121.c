/*
 * amn41121.c
 *
 * Created: 3/12/2013 5:49:45 PM
 *  Author: kweekly
 */ 

#include "all.h"

// upper byte is seconds, lower byte is 1/256 of seconds
uint16_t pir_accH;
uint16_t pir_accL;
uint16_t pir_latch;

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
		//kputs("DOOP H\n");
	} else { // went low from being high
		pir_accH += tdiff;
		//kputs("DOOP L\n");
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
	pir_sleep();
	pir_accH = pir_accL = 0;
}

void pir_setup_reporting_schedule(uint16_t starttime) {
	scheduler_add_task(PIR_TASK_ID,starttime,&_pir_reporting_write_report);	
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

void pir_fmt_reading(float * reading, uint8_t maxlen, char * str) {
	snprintf_P(str,maxlen,PSTR("Occupancy=%-4.1f%%"),(100.0* *reading));
}