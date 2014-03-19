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

// fields requested to be reported.. not necessarily what is actually reported
extern uint16_t report_fields_requested;

#if defined(ENVIRONMENT_SENSOR)
	#define REPORT_TYPE_OCCUPANCY	 0x04
	#define REPORT_TYPE_LIGHT		 0x08
	
	#if HW_VERSION == 1 || HW_VERSION == 2		
		#define REPORT_TYPE_GYRO		 0x20
	#endif
	#define REPORT_TYPE_ACCEL		 0x10
	#define REPORT_TYPE_ORIENTATION_CHANGED	0x200
	#define REPORT_TYPE_TEMP		 0x01
	#define REPORT_TYPE_HUMID		 0x02
	#define REPORT_TYPE_RSSI		 0x40
	#define REPORT_TYPE_LIGHTRAW	 0x80
	
	#define REPORT_TYPE_OCCUPANCY_CHANGED	0x100
	#define REPORT_TYPE_LIGHT_CHANGED		0x400
	#ifdef USE_DOOR_SENSORS
		#define REPORT_TYPE_DOOR_SENSORS		0x800
	#endif
	#ifdef USE_PN532
		#define REPORT_TYPE_RFID_COUNT	0x1000
	#endif	
	
	#define REPORT_TYPE_CO2 0x2000
#elif defined(POWER_STRIP_MONITOR)
	#define REPORT_TYPE_POWER_CH0		0x01
	#define REPORT_TYPE_POWER_CH1		0x02
	#define REPORT_TYPE_POWER_CH2		0x04
	#define REPORT_TYPE_POWER_CH3		0x08
	#define REPORT_TYPE_POWER_CH4		0x10
	#define REPORT_TYPE_POWER_CH5		0x20
	#define REPORT_TYPE_RSSI			0x100
#endif

#if defined(USE_MACHXO2_PMCO2)
	#define REPORT_TYPE_CO2_CH0		    0x01
	#define REPORT_TYPE_CO2_CH1		    0x02
	#define REPORT_TYPE_CO2_CH3		    0x04
	#define REPORT_TYPE_PM_CH0			0x08
	#define REPORT_TYPE_PM_CH1			0x10
	#define REPORT_TYPE_PM_CH2			0x20
	#define REPORT_TYPE_PM_CH3			0x40
	#define REPORT_TYPE_PM_CH4			0x80
	#define REPORT_TYPE_PM_CH5			0x100
	#define REPORT_TYPE_PM_CH6			0x200
#endif




typedef struct {
	uint32_t time;
	uint16_t fields;
	
#if defined ENVIRONMENT_SENSOR
	humid_reading_t temphumid;

	float  occupancy;
	light_reading_t light;
	accel_reading_t accel;
	gyro_reading_t gyro;
	int8_t rssi;
	
	unsigned char occupancy_state;
	unsigned char orientation;
	unsigned char light_level_state;
	
	#ifdef USE_DOOR_SENSORS
	unsigned char door_sensor_state;
	#endif
	
	#ifdef USE_PN532
	unsigned char rfid_count;
	#endif
	
#elif defined POWER_STRIP_MONITOR
	powermon_reading_t power[POWERMON_NUM_CHANNELS];
	int8_t rssi;
#endif

#ifdef USE_MACHXO2_PMCO2
	uint16_t co2[MACHXO2_NUM_CO2INPUTS];
	uint32_t pm[MACHXO2_NUM_PMINPUTS];
	uint32_t pm_timer;
#endif

uint16_t co2;


} report_t;

void report_init();
void report_new(uint32_t time);
void report_print_human(report_t * rep);
uint16_t report_populate_real_data(report_t * rep, uint8_t * buf);
report_t * report_current();
void report_poplast();

#endif