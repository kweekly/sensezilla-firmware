/*
 * l3gd20.c
 *
 * Created: 3/12/2013 5:49:25 PM
 *  Author: kweekly
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "drivers/i2cmaster.h"
#include "devicedefs.h"
#include "devices/l3gd20.h"

#define WHO_AM_I			0x0F
#define CTRL_REG1			0x20
#define CTRL_REG2			0x21
#define CTRL_REG3			0x22
#define CTRL_REG4			0x23
#define CTRL_REG5			0x24

#define REFERENCE			0x25
#define OUT_TEMP			0x26
#define STATUS_REG			0x27
#define OUT_X_L				0x28
#define OUT_X_H				0x29
#define OUT_Y_L				0x2A
#define OUT_Y_H				0x2B
#define OUT_Z_L				0x2C
#define OUT_Z_H				0x2D

#define FIFO_CTRL_REG		0x2E
#define FIFO_SRC_REG		0x2F
#define INT1_CFG			0x30
#define INT1_SRC			0x31
#define INT1_TSH_XH			0x32
#define INT1_TSH_XL			0x33
#define INT1_TSH_YH			0x34
#define INT1_TSH_YL			0x35
#define INT1_TSH_ZH			0x36
#define INT1_TSH_ZL			0x37
#define INT1_DURATION		0x38

#define DEFAULT_CTRL_REG1   0x00 // 95Hz Data Rate, 12.5 Hz Cut-Off freq


void gyro_init(void) {
	unsigned char b;
	
	gyro_sleep();
	
	// configure high-pass filter
	b = 0b00100110; // 0.9Hz Cutoff frequency
	i2c_writereg(GYRO_ADDR, CTRL_REG2, 1, &b );
	
	// configure interrupts
	b = 0b00001000; // Data Ready interrupt on
	i2c_writereg(GYRO_ADDR, CTRL_REG3, 1, &b);
	
	// configure data stuff
	b = 0;
	i2c_writereg(GYRO_ADDR, CTRL_REG4, 1, &b);
	
	// configure other stuff 
	b = 0b0011010; // enable HPF
	i2c_writereg(GYRO_ADDR, CTRL_REG5, 1, &b); 
}

gyro_reading_t gyro_read()  {
	gyro_reading_t retval;
	unsigned char buf[6];
	if ( i2c_readreg(GYRO_ADDR, OUT_X_L, 6, buf ) ) {
		kputs("Error reading from gyro sensor\n");
	}
	retval.X = buf[0] | ((uint16_t)buf[1] << 8);
	retval.Y = buf[2] | ((uint16_t)buf[3] << 8);
	retval.Z = buf[4] | ((uint16_t)buf[5] << 8);
	return retval;
}

void gyro_wake(void) {
	unsigned char b = DEFAULT_CTRL_REG1 | 0xF;
	if ( i2c_writereg(GYRO_ADDR, CTRL_REG1, 1, &b ) ) {
		kputs("Error turning on gyro\n");
	}
}

void gyro_sleep(void) {
	unsigned char b = DEFAULT_CTRL_REG1;
	if ( i2c_writereg(GYRO_ADDR, CTRL_REG1, 1, &b ) ) {
		kputs("Error turning off gyro\n");
	}	
}

