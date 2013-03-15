/*
 * tsl2560.c
 *
 * Created: 3/12/2013 5:48:52 PM
 *  Author: kweekly
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "drivers/i2cmaster.h"
#include "devices/tsl2560.h"
#include "devicedefs.h"


#define COMMAND_DEFAULT	0x80
#define COMMAND_SMB_WORD  0xA0
#define COMMAND_BLOCK		0x90
#define COMMAND_CLEAR_INTERRUPT 0xC0


#define CONTROL	0x00
#define TIMING	0x01
#define THRESHLL	0x02
#define THRESHLH	0x03
#define THRESHHL	0x04
#define THRESHHH	0x05
#define INTERRUPT	0x07
#define ID	0x0A
#define D0L	0x0C
#define D0H	0x0D
#define D1L	0x0E
#define D1H	0x0F


void light_init(void) {
	light_sleep();	
	
	// configure integration rate and interrupt
}

light_reading_t light_read() {
	light_reading_t retval;
	unsigned char buf[4];
	if ( i2c_readreg(LIGHT_ADDR, COMMAND_BLOCK | D0L, 4, buf ) ) {
		kputs("Error reading from light sensor\n");
	}
	retval.ch0 = buf[0] | ((uint16_t)buf[1] << 8);
	retval.ch1 = buf[2] | ((uint16_t)buf[3] << 8);
	return retval;
}

void light_wake() {
	unsigned char b = 0x03;
	if ( i2c_writereg(LIGHT_ADDR, COMMAND_DEFAULT | CONTROL, 1, &b ) ) {
		kputs("Error turning on light sensor\n");
	}
}

void light_sleep() {
	unsigned char b = 0x00;
	if ( i2c_writereg(LIGHT_ADDR, COMMAND_DEFAULT | CONTROL, 1, &b) )   {
		kputs("Error turning off light sensor\n");
	}
}