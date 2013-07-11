/*
 * report.h
 *
 * Created: 3/19/2013 4:14:01 PM
 *  Author: kweekly
 */
#ifndef _REPORT_H
#define _REPORT_H

#include "devicedefs.h"
#include "device_headers.h"

#if defined(ENVIRONMENT_SENSOR)
	#define REPORT_NUM_TYPES		10
	#define REPORT_TYPE_TEMP		0x01
	#define REPORT_TYPE_HUMID		0x02
	#define REPORT_TYPE_OCCUPANCY	0x04
	#define REPORT_TYPE_LIGHT		0x08
	#define REPORT_TYPE_ACCEL		0x10
	#define REPORT_TYPE_GYRO		0x20
	#define REPORT_TYPE_RSSI		0x40
	
	#define REPORT_TYPE_OCCUPANCY_CHANGED	0x100
	#define REPORT_TYPE_ORIENTATION_CHANGED	0x200
	#define REPORT_TYPE_LIGHT_CHANGED		0x400

	typedef struct {
		uint32_t time;
		uint16_t fields;
		union {
			uint8_t data;
			humid_reading_t temphumid;
		};			
		float  occupancy;
		light_reading_t light;
		accel_reading_t accel;
		gyro_reading_t gyro;
		int8_t rssi;
		
		unsigned char occupancy_state;
		unsigned char orientation;
		unsigned char light_level_state;
	} report_t;
	
#elif defined(POWER_STRIP_MONITOR)
	#define REPORT_NUM_TYPES		7

	#define REPORT_TYPE_POWER_CH0		0x01
	#define REPORT_TYPE_POWER_CH1		0x02
	#define REPORT_TYPE_POWER_CH2		0x04
	#define REPORT_TYPE_POWER_CH3		0x08
	#define REPORT_TYPE_POWER_CH4		0x10
	#define REPORT_TYPE_POWER_CH5		0x20
	#define REPORT_TYPE_RSSI			0x100

	typedef struct {
		uint32_t time;
		uint16_t fields;
		union {
			uint8_t data;
			powermon_reading_t power[POWERMON_NUM_CHANNELS];
		};	
		int8_t rssi;
	} report_t;
#endif

void report_init();
void report_new(uint32_t time);
void report_print(report_t * rep);
void report_print_human(report_t * rep);
uint16_t report_populate_real_data(report_t * rep, uint8_t * buf);
report_t * report_current();
void report_poplast();

#endif