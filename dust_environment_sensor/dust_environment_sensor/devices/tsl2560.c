/*
 * tsl2560.c
 *
 * Created: 3/12/2013 5:48:52 PM
 *  Author: kweekly
 */ 
#include "all.h"


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
	unsigned char b;
	light_sleep();	
	
	// configure integration rate and interrupt
	b = 0b00000001; // 01 integ field, scale 0.252, time=101ms
	i2c_writereg(LIGHT_ADDR, COMMAND_DEFAULT | TIMING, 1, &b);
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

void light_setup_reporting_schedule(uint16_t starttime) {
	scheduler_add_task(LIGHT_TASK_ID, starttime, &light_wake);
	scheduler_add_task(LIGHT_TASK_ID, starttime += 105, &_light_reporting_finish); // change based on integration constant
}

void _light_reporting_finish(void) {
	report_current()->light = light_read();
	report_current()->fields |= REPORT_TYPE_LIGHT;
	light_sleep();
}

#define CHANNEL_SCALE 37177

void light_fmt_reading(light_reading_t * reading, uint8_t maxlen, char * str) {
	float flt;
	light_convert_real(reading, &flt);
	snprintf(str,maxlen,"mLux=%-6.3f",1e3*flt);
}


uint8_t light_convert_real(light_reading_t * reading, float * fltptr) {
	double lux = 0;
	double ch0 = (double)reading->ch0 / (double)CHANNEL_SCALE, ch1 = (double)reading->ch1 / (double)CHANNEL_SCALE;
	double ratio = ch1/ch0;
	if ( ratio > 0 && ratio <= 0.52) {
		lux = 0.0304*ch0 - 0.062*ch0*pow(ch1/ch0,1.4);
	} else if (ratio > 0.52 && ratio <= 0.61 ) {
		lux = 0.0224*ch0 - 0.031*ch1;
	} else if (ratio > 0.61 && ratio <= 0.8 ) {
		lux = 0.0128*ch0 - 0.0153*ch1;
	} else if ( ratio > 0.8 && ratio <= 1.3 ) {
		lux = 0.00146*ch0 - 0.00112*ch1;
	} else {
		lux = 0;
	}
	fltptr[0] = lux;
	return 1;
}