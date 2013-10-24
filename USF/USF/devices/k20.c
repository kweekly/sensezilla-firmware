/*
 * k20.c
 *
 * Created: 10/23/2013 4:39:25 PM
 *  Author: kweekly
 */ 

#include "devicedefs.h"
#ifdef USE_K20
#include "avrincludes.h"
#include "utils/scheduler.h"
#include "drivers/softserial.h"
#include "devices/k20.h"
#include "protocol/report.h"

#define K20_TIMEOUT 10000 // in 10us increments

const uint8_t READCMD[] = {0xFE, 0x44, 0x00, 0x08, 0x02, 0x9F, 0x25};

void k20_init() {
	softserial_init();
}

void k20_setup_reporting_schedule(uint16_t starttime) {
	// try to find a time when not hing else is happing
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST, K20_TASK_ID, starttime += 10, &_k20_reporting_startread);
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST, K20_TASK_ID, starttime + 20, &_k20_reporting_checkread);
}

void _k20_reporting_startread() {
	softserial_write(sizeof(READCMD),READCMD);
}

void _k20_reporting_checkread() {
	uint16_t timeout = K20_TIMEOUT;
	uint16_t co2val;
	while (timeout--) {
		if ( softserial_available() >= 7) {
			if ( softserial_read() == 0xFE && softserial_read() == 0x44 && softserial_read() == 0x02 ) {
				co2val = ((uint16_t)softserial_read()<<(uint16_t)8) | (uint16_t)softserial_read();
				report_current()->co2 = co2val;
				report_current()->fields |= REPORT_TYPE_CO2;
				return;
			}
		}
		pcint_check(); // needed for software serial
		_delay_us(10);
	}
	kputs("Timeout waiting for K20 to respond\n");
}

#endif