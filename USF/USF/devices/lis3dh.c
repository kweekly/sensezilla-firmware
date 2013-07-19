/*
 * lis3dh.c
 *
 * Created: 3/12/2013 5:48:08 PM
 *  Author: kweekly
 */ 

#include "devicedefs.h"
#ifdef USE_LIS3DH

#include "avrincludes.h"
#include "protocol/report.h"

#include "devices/lis3dh.h"
#include "drivers/i2cmaster.h"
#include "drivers/pcint.h"
#include "drivers/rtctimer.h"
#include "utils/scheduler.h"

#define STATUS_REG_AUX 0x07
#define OUT_ADC1_L		0x08
#define OUT_ADC1_H		0x09
#define OUT_ADC2_L		0x0A
#define OUT_ADC2_H		0x0B
#define OUT_ADC3_L		0x0C
#define OUT_ADC3_H		0x0D
#define INT_COUNTER_REG	0x0E
#define WHO_AM_I		0x0F
#define TEMP_CFG_REG	0x1F
#define CTRL_REG1		0x20
#define CTRL_REG2		0x21
#define CTRL_REG3		0x22
#define CTRL_REG4		0x23
#define CTRL_REG5		0x24
#define CTRL_REG6		0x25
#define REFERENCE		0x26
#define STATUS_REG		0x27
#define OUT_X_L			0x28
#define OUT_X_H			0x29
#define OUT_Y_L			0x2A
#define OUT_Y_H			0x2B
#define OUT_Z_L			0x2C
#define OUT_Z_H			0x2D
#define FIFO_CTRL_REG	0x2E
#define FIFO_SRC_REG	0x2F
#define INT1_CFG		0x30
#define INT1_SOURCE		0x31
#define INT1_THS		0x32
#define INT1_DURATION	0x33
#define CLICK_CFG		0x38
#define CLICK_SRC		0x39
#define CLICK_THS		0x3A
#define TIME_LIMIT		0x3B
#define TIME_LATENCY	0x3C
#define TIME_WINDOW		0x3D

#define READ_MULTIPLE 0x80

void (*click_cb)(unsigned char src);
void (*orient_cb)(unsigned char orientation);


void accel_init(void) {
	unsigned char b;
	
	accel_wake();
	
	b = 0b00000100; // HPF freq is 1Hz, enable HPF for click, but not data or AOI function
	i2c_writereg(ACCEL_ADDR, CTRL_REG2, 1, &b);
	
	b = 0b00000000; // no interrupts ( for now ?)
	i2c_writereg(ACCEL_ADDR, CTRL_REG3, 1, &b);
	
	b = 0b10000000; // no self test, block data update, +/- 2G scale
	i2c_writereg(ACCEL_ADDR, CTRL_REG4, 1, &b);
	
	b = 0b00000000; // no 4D detection, no FIFO
	i2c_writereg(ACCEL_ADDR, CTRL_REG5, 1, &b);
	
	b = 0b00000000; 
	i2c_writereg(ACCEL_ADDR, CTRL_REG6, 1, &b);
	
	click_cb = orient_cb = NULL;
}

void accel_wake(void) {
	unsigned char b = 0x4F; // low power mode, 50Hz
	if ( i2c_writereg(ACCEL_ADDR, CTRL_REG1, 1, &b ) ) {
		kputs("Error turning on accel\n");
	}	
}

void accel_sleep(void) {
	
	unsigned char b = 0b00001000;
	if ( i2c_writereg(ACCEL_ADDR, CTRL_REG1, 1, &b ) ) {
		kputs("Error turning off accel\n");
	}
	
}

uint8_t accel_read_status(void) {
	unsigned char b;
	if ( i2c_readreg(ACCEL_ADDR, STATUS_REG, 1, &b) ) {
		kputs("Error reading status reg\n");
	}
	return b;
}

accel_reading_t accel_read(void) {
	accel_reading_t retval;
	unsigned char buf[6];
	
	while ( (accel_read_status() & _BV(3)) == 0) {
		kputs("Error: Accel not ready\n");
	}
	
	if ( i2c_readreg(ACCEL_ADDR, READ_MULTIPLE | OUT_X_L, 6, buf ) ) {
		kputs("Error reading from accel sensor\n");
	}
	retval.X = *(int16_t *)(buf);
	retval.Y = *(int16_t *)(buf+2);
	retval.Z = *(int16_t *)(buf+4);
	
	return retval;	
	
}

// Uses INT1 of accel -> PORTA.0 / PCINT0
void accel_configure_click( void (*c)(unsigned char) ) {
	unsigned char b = 0b10000000; // click interrupt on INT1
	if ( c == NULL ) {
		pcint_unregister(0);
		click_cb = c;
		return;			
	}
	if ( i2c_writereg(ACCEL_ADDR, CTRL_REG3, 1, &b ) ) {
		kputs("Error configuring accel sensor\n");
	}
	
	b = 0b00010101; // tap detect on all axis
	i2c_writereg(ACCEL_ADDR, CLICK_CFG, 1, &b);
	
	b = 80; // approx 1.25g
	i2c_writereg(ACCEL_ADDR, CLICK_THS, 1, &b);
	
	//b = 1; // at most 20ms of acceleration
	b = 10; // at most 200ms of acceleration
	i2c_writereg(ACCEL_ADDR, TIME_LIMIT, 1, &b);
	
	click_cb = c;
	
	// setup PCINT0
	pcint_register(0,&_accel_int1_cb);
}

void _accel_int1_cb() {
	unsigned char b;
	if ( ACCEL_INT1_PIN && click_cb ) {
		// read to clear status
		if ( i2c_readreg(ACCEL_ADDR, CLICK_SRC, 1, &b) ) {
			kputs("Error reading accel click source\n");
			return;
		}		
		click_cb(b);
	}
}


// uses INT2 -> PORTA.1 / PCINT1
void accel_configure_orientation_detection( unsigned char detection_mask, void (*o)(unsigned char orientation) ) {
	unsigned char b = 0b01000000; // active high, AOI interrupt enable
	if ( o == NULL ) {
		pcint_unregister(1);
		orient_cb = NULL;
		return;
	}
	if ( i2c_writereg(ACCEL_ADDR, CTRL_REG6, 1, &b ) ) {
		kputs("Error configuring accel sensor\n");
	}
	
	b = 0b11000000 | detection_mask; // enable 6D motion detection
	i2c_writereg(ACCEL_ADDR, INT1_CFG, 1, &b );
	
	b = 0x21; // Approx 0.528g
	i2c_writereg(ACCEL_ADDR, INT1_THS, 1, &b );
	
	b = 25; // must be active for half-second
	i2c_writereg(ACCEL_ADDR, INT1_DURATION, 1, &b);
	
	orient_cb = o;
	
	// setup PCINT1
	pcint_register(1,&_accel_int2_cb);
}

void _accel_int2_cb() {
	unsigned char b;
	if ( ACCEL_INT2_PIN && orient_cb ) {
		if ( i2c_readreg(ACCEL_ADDR, INT1_SOURCE, 1, &b) ) {
			kputs("Error reading accel interrupt source\n");
			return;
		}
		orient_cb(b&0x3F);
	}
}

void accel_setup_reporting_schedule(uint16_t starttime) {
	accel_wake(); // accelerometer will be always-on
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,ACCEL_TASK_ID,starttime,&_accel_reporting_doread);
}

volatile unsigned char int_orientation_latch;
volatile uint32_t int_start_latch;
volatile uint8_t int_setup_time;

void accel_setup_interrupt_schedule(uint16_t starttime) {
	accel_configure_interrupt(5);
	accel_wake();
	accel_configure_orientation_detection(ACCEL_ORIENTATION_ALL, &_accel_interrupt_orient_cb);
	int_orientation_latch = 0;
	int_start_latch = 0;
	scheduler_add_task(SCHEDULER_MONITOR_LIST, ACCEL_TASK_ID, starttime, &_accel_interrupt_check );
}

void accel_configure_interrupt(uint8_t setup_time) {
	int_setup_time = setup_time;
}

void _accel_interrupt_check() {
	volatile uint32_t curtime = rtctimer_read();
	//printf_P(PSTR("OL=%02X curtime=%ld startlatch=%ld\n"),int_orientation_latch,curtime,int_start_latch);
	if (int_orientation_latch && (curtime - int_start_latch > int_setup_time) ) {
		// in one place for long enough
		report_current()->fields |= REPORT_TYPE_ORIENTATION_CHANGED;
		report_current()->orientation = int_orientation_latch;
		int_orientation_latch = 0;
	}
}

void _accel_interrupt_orient_cb(unsigned char orientation) {
	//printf_P(PSTR("Orient=%02X\n"),orientation);
	if ( orientation != int_orientation_latch ) {
		int_orientation_latch = orientation;
		int_start_latch = rtctimer_read();
	}		
}

void _accel_reporting_doread() {
	report_current()->accel = accel_read();
	report_current()->fields |= REPORT_TYPE_ACCEL;
}

#define ACCEL_SENSITIVITY 2.0

void accel_fmt_reading(accel_reading_t * reading, uint8_t maxlen, char * str) {
	float ax = reading->X / (float)(1<<15) * ACCEL_SENSITIVITY;
	float ay = reading->Y / (float)(1<<15) * ACCEL_SENSITIVITY;
	float az = reading->Z / (float)(1<<15) * ACCEL_SENSITIVITY;
	snprintf_P(str,maxlen,PSTR("ax=%5.2fg ay=%5.2fg az=%5.2fg"),ax,ay,az);
}

uint8_t accel_convert_real(accel_reading_t * reading, float * fltptr) {
	fltptr[0] = reading->X / (float)(1<<15) * ACCEL_SENSITIVITY;
	fltptr[1] = reading->Y / (float)(1<<15) * ACCEL_SENSITIVITY;
	fltptr[2] = reading->Z / (float)(1<<15) * ACCEL_SENSITIVITY;	
	return 3;
}


#endif