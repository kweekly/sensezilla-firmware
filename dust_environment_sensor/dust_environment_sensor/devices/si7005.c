/*
 * si7005.c
 *
 * Created: 3/12/2013 5:48:30 PM
 *  Author: kweekly
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "drivers/i2cmaster.h"
#include "devicedefs.h"
#include "devices/si7005.h"


#define STATUS 0
#define DATAh 1
#define DATAl 2
#define CONFIG 3
#define ID 17

void humid_init(void) { // nothing here since we cut power to the device normally	
}

void humid_sleep(void) {
	HUMID_VCC = 0;
}

void humid_wake(void) {
	HUMID_VCC = 1; 
	// should now wait for 10ms
}

humid_reading_t humid_read(void) {
	humid_reading_t retval;
	unsigned char b;
	unsigned char buf[2];
	
	b = 0b00000001; // start a RH conversion, no fast mode
	if ( i2c_writereg(HUMID_ADDR, CONFIG, 1, &b) ) {
		kputs("Error Starting humidity conversion");
		return;
	}
	
	// wait for 35ms (typical)
	do {
		// wait for a couple ms
		i2c_readreg(HUMID_ADDR, STATUS, 1, &b);
	} while(b & 0x01);

	i2c_readreg(HUMID_ADDR, DATAh, 2, &buf );
	retval.humidity = ((uint16_t)buf[0] << 8) | buf[1];
	
	
	b = 0b00010001; // start a Temp conversion, no fast mode
	if ( i2c_writereg(HUMID_ADDR, CONFIG, 1, &b) ) {
		kputs("Error Starting humidity conversion");
		return;
	}
		
	// wait for 35ms (typical)
	do {
		// wait for a couple ms
		i2c_readreg(HUMID_ADDR, STATUS, 1, &b);
	} while(b & 0x01);

	i2c_readreg(HUMID_ADDR, DATAh, 2, &buf );
	retval.temperature = ((uint16_t)buf[0] << 8) | buf[1];
}
