/*
 * si7005.c
 *
 * Created: 3/12/2013 5:48:30 PM
 *  Author: kweekly
 */ 

#include "devicedefs.h"
#ifdef USE_SI7005

#include "avrincludes.h"
#include "protocol/report.h"

#include "devices/si7005.h"
#include "drivers/i2cmaster.h"
#include "utils/scheduler.h"

#define STATUS 0
#define DATAh 1
#define DATAl 2
#define CONFIG 3
#define ID 17

void humid_init(void) { // nothing here since we cut power to the device normally	
}

void humid_sleep(void) {
	HUMID_VCC = 0;
	HUMID_CSN = 1;
}

void humid_wake(void) {
	HUMID_VCC = 1; 
	HUMID_CSN = 0;
	// should now wait for 10ms
}

humid_reading_t humid_read(void) {
	humid_reading_t retval;
	unsigned char b;
	unsigned char buf[2];
	
	b = 0b00000001; // start a RH conversion, no fast mode
	if ( i2c_writereg(HUMID_ADDR, CONFIG, 1, &b) ) {
		kputs("Error Starting humidity conversion");
		return retval;
	}
	
	// wait for 35ms (typical)
	do {
		// wait for a couple ms
		i2c_readreg(HUMID_ADDR, STATUS, 1, &b);
	} while(b & 0x01);

	i2c_readreg(HUMID_ADDR, DATAh, 2, buf );
	retval.humidity = ((uint16_t)buf[0] << 8) | buf[1];
	
	
	b = 0b00010001; // start a Temp conversion, no fast mode
	if ( i2c_writereg(HUMID_ADDR, CONFIG, 1, &b) ) {
		kputs("Error Starting humidity conversion");
		return retval;
	}
		
	// wait for 35ms (typical)
	do {
		// wait for a couple ms
		i2c_readreg(HUMID_ADDR, STATUS, 1, &b);
	} while(b & 0x01);

	i2c_readreg(HUMID_ADDR, DATAh, 2, buf );
	retval.temperature = ((uint16_t)buf[0] << 8) | buf[1];
	
	return retval;
}

uint8_t humid_reporting_error_occurred;

void humid_setup_reporting_schedule(uint16_t curtime) {
	scheduler_add_task(HUMID_TASK_ID, curtime, &humid_wake);
	scheduler_add_task(HUMID_TASK_ID, curtime += 10, &_humid_reporting_readh);
	scheduler_add_task(HUMID_TASK_ID, curtime += 35, &_humid_reporting_readt);
	scheduler_add_task(HUMID_TASK_ID, curtime += 35, &_humid_reporting_finish);
	humid_reporting_error_occurred = 0;
}

void _humid_reporting_readh(void) {
	unsigned char b;
	report_t * r = report_current();
	if ( ! r ) return;
	
	b = 0b00000001; // start a RH conversion, no fast mode
	if ( i2c_writereg(HUMID_ADDR, CONFIG, 1, &b) ) {
		kputs("Error Starting humidity conversion");
		humid_reporting_error_occurred = 1;
		return;
	}
}

void _humid_reporting_readt(void){ // this saves the humidity as well
	unsigned char b;
	unsigned char buf[2];
	
	report_t * r = report_current();
	
	if ( !r || humid_reporting_error_occurred ) return;
	
	do {
		i2c_readreg(HUMID_ADDR, STATUS, 1, &b);
	} while(b & 0x01);
	
	i2c_readreg(HUMID_ADDR, DATAh, 2, buf );
	r->temphumid.humidity = ((uint16_t)buf[0] << 8) | buf[1];	
	r->fields |= REPORT_TYPE_HUMID;
	
	b = 0b00010001; // start a Temp conversion, no fast mode
	if ( i2c_writereg(HUMID_ADDR, CONFIG, 1, &b) ) {
		kputs("Error Starting temperature conversion");
		humid_reporting_error_occurred = 1;
		return;
	}	
}
	
void _humid_reporting_finish(void) {
	unsigned char b;
	unsigned char buf[2];
	
	report_t * r = report_current();
	
	if ( !r || humid_reporting_error_occurred ) return;
	
	do {
		i2c_readreg(HUMID_ADDR, STATUS, 1, &b);
	} while(b & 0x01);
	
	i2c_readreg(HUMID_ADDR, DATAh, 2, buf );
	r->temphumid.temperature = ((uint16_t)buf[0] << 8) | buf[1];
	r->fields |= REPORT_TYPE_TEMP;
	
	humid_sleep();
}

void humid_fmt_reading(humid_reading_t * reading, uint8_t maxlen, char * str) {
	double rh = (reading->humidity >> 4) / 16.0 - 24.0;
	rh = rh - (rh*rh*-0.00393 + rh*0.4008 - 4.7844);
	
	double temp = (reading->temperature >> 2) / 32.0 - 50.0;
	snprintf_P(str,maxlen,PSTR("RH=%5.2f%% T=%5.2fC"), rh,temp);
}

uint8_t humid_convert_real(humid_reading_t * reading, float * flt) {
	float rh = (reading->humidity >> 4) / 16.0 - 24.0;
	rh = rh - (rh*rh*-0.00393 + rh*0.4008 - 4.7844);
	
	float temp = (reading->temperature >> 2) / 32.0 - 50.0;
	flt[0] = rh;
	flt[1] = temp;
	return 2;	
}

#endif