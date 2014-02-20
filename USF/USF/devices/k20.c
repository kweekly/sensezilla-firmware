/*
 * k20.c
 *
 * Created: 10/23/2013 4:39:25 PM
 *  Author: kweekly
 */ 

#include "devicedefs.h"
#ifdef USE_SOFTSERIAL
#include "avrincludes.h"
#include "utils/scheduler.h"
#include "drivers/softserial.h"
#include "devices/k20.h"
#include "protocol/report.h"
#include "drivers/pcint.h"

#define K20_TIMEOUT 100000 // in 10us increments

const uint8_t READCMD[] = {0xFE, 0x44, 0x00, 0x08, 0x02, 0x9F, 0x25};

void k20_init() {
	softserial_init();
}

void k20_setup_reporting_schedule(uint16_t starttime) {
	// try to find a time when not hing else is happing
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST, K20_TASK_ID, starttime, &_k20_reporting_startread);
	//scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST, K20_TASK_ID, starttime + 5, &_k20_reporting_checkread);
}

void _k20_reporting_startread() {
	softserial_write(sizeof(READCMD),READCMD);
	#ifdef LED1
		LED1 = 1;
	#endif
	_k20_reporting_checkread();
}

void _k20_reporting_checkread() {
	uint16_t timeout = K20_TIMEOUT;
	uint16_t co2val;
	
	//EXP_CSN = 1;
	while (timeout--) {
		if ( softserial_available() >= 7) {
			if ( softserial_read() == 0xFE && softserial_read() == 0x44 && softserial_read() == 0x02 ) {
				co2val = ((uint16_t)softserial_read()<<(uint16_t)8) | (uint16_t)softserial_read();
				report_current()->co2 = co2val;
				report_current()->fields |= REPORT_TYPE_CO2;
				#ifdef LED1
					LED1 = 0;
				#endif
				//EXP_CSN = 0;
				return;
			} else {
				kputs("Enough bytes but bad\n");
			}
		}
		pcint_check(); // needed for software serial
		_delay_us(1);
	}
	kputs("Timeout waiting for K20 to respond\n");
	//EXP_CSN = 0;
}

#else // for I2C
#include "avrincludes.h"
#include "utils/scheduler.h"
#include "drivers/i2cmaster.h"
#include "devices/k20.h"
#include "protocol/report.h"
void k20_init() {
}

void k20_setup_reporting_schedule(uint16_t starttime) {
	// try to find a time when not hing else is happing
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST, K20_TASK_ID, starttime, &_k20_reporting_startread);
	//scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST, K20_TASK_ID, starttime + 20, &_k20_reporting_checkread);
}

void _k20_reporting_startread() {
	char b[] = {0x00,0x08,0x2A};
	if ( i2c_writereg(K20_ADDR,0x22,3,b) ) {
		kputs("Error starting K20 I2C Conversion\n");
	}
	#ifdef LED1
		LED1 = 1;
	#endif
	_delay_ms(20);
	_k20_reporting_checkread();
	_delay_ms(10);
}

void _k20_reporting_checkread() {
	char numTries = 5;
	char rbuf[4];
	while(numTries--) {
		if (i2c_readbytes(K20_ADDR,4,rbuf)) {
			kputs("Error reading from K20 I2C\n");
			return;
		}
		// check checksum
		char sum = rbuf[0] + rbuf[1] + rbuf[2];
		if ( sum != rbuf[3] ) {
			kputs("K20 I2C Checksum error\n");
		}		
		else if ( rbuf[0] & 0x01 ) { // completed
			uint16_t co2val = 0;
			co2val |= rbuf[1] & 0xFF;
			co2val = co2val << 8;
			co2val |= rbuf[2] & 0xFF;
			report_current()->co2 = co2val;
			report_current()->fields |= REPORT_TYPE_CO2;
			#ifdef LED1	
				LED1 = 0;		
			#endif
			return;
		}
		_delay_ms(10);
	}
	kputs("Timeout reading from K20 I2C\n");
}



#endif