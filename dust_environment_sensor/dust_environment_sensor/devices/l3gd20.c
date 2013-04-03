/*
 * l3gd20.c
 *
 * Created: 3/12/2013 5:49:25 PM
 *  Author: kweekly
 */ 

#include "all.h"

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

#define READ_MULTIPLE 0x80

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
	b = 0; // 250 dps scale
	i2c_writereg(GYRO_ADDR, CTRL_REG4, 1, &b);
	
	// configure other stuff 
	b = 0b0011010; // enable HPF
	i2c_writereg(GYRO_ADDR, CTRL_REG5, 1, &b); 
}

uint8_t gyro_read_status(void) {
	unsigned char b;
	if ( i2c_readreg(GYRO_ADDR, STATUS_REG, 1, &b) ) {
		kputs("Error reading status reg\n");
	}
	return b;
}

gyro_reading_t gyro_read()  {
	gyro_reading_t retval;
	unsigned char buf[6];
	
	if ( (gyro_read_status() & _BV(3) ) == 0) {
		kputs("Error: Gyro not ready\n");
	}
	
	if ( i2c_readreg(GYRO_ADDR, READ_MULTIPLE | OUT_X_L, 6, buf ) ) {
		kputs("Error reading from gyro sensor\n");
	}
	retval.X = *(int16_t *)(buf);
	retval.Y = *(int16_t *)(buf+2);
	retval.Z = *(int16_t *)(buf+4);
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

#define GYRO_SENSITIVITY 250.0

void gyro_fmt_reading(gyro_reading_t * reading, uint8_t maxlen, char * str) {
	float gx = reading->X / (float)(1<<15) * GYRO_SENSITIVITY;
	float gy = reading->Y / (float)(1<<15) * GYRO_SENSITIVITY;
	float gz = reading->Z / (float)(1<<15) * GYRO_SENSITIVITY;
	snprintf_P(str,maxlen,PSTR("gx=%5.2fdps gy=%5.2fdps gz=%5.2fdps"),gx,gy,gz);
}

uint8_t gyro_convert_real(gyro_reading_t * reading, float * fltptr) {
	fltptr[0] = reading->X / (float)(1<<15) * GYRO_SENSITIVITY;
	fltptr[1]  = reading->Y / (float)(1<<15) * GYRO_SENSITIVITY;
	fltptr[2]  = reading->Z / (float)(1<<15) * GYRO_SENSITIVITY;
	return 3;	
}

void gyro_setup_reporting_schedule(uint16_t starttime) {
	scheduler_add_task(GYRO_TASK_ID,starttime,&gyro_wake);
	scheduler_add_task(GYRO_TASK_ID,starttime + 15,&_gyro_reporting_finish);
}

void _gyro_reporting_finish() {
	report_current()->gyro = gyro_read();
	report_current()->fields |= REPORT_TYPE_GYRO;
	gyro_sleep();
}