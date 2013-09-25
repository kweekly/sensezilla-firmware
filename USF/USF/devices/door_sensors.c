/*
 * door_sensors.c
 *
 * Created: 9/20/2013 11:41:29 AM
 *  Author: kweekly
 */ 

#include "devicedefs.h"
#ifdef USE_DOOR_SENSORS


#include "avrincludes.h"
#include "utils/scheduler.h"
#include "protocol/report.h"
#include "devices/door_sensors.h"

#define SENSOR_INDOOR	1
#define SENSOR_OUTDOOR	2
#define SENSOR_OPEN		4
#define SENSOR_CLOSED	8
volatile unsigned char door_sensor_state;
volatile unsigned char last_open_state;

void door_sensors_init() {
	EXP_CSN = 1;
	pcint_register(DOOR_SENSOR_OUTDOOR_TOUCH_PCINT,&_door_sensors_outdoor_interrupt);
	pcint_register(DOOR_SENSOR_INDOOR_TOUCH_PCINT,&_door_sensors_indoor_interrupt);
	door_sensor_state = 0;
	last_open_state = 0x80;
}

void door_sensors_setup_interrupt_schedule(uint16_t starttime) {
	scheduler_add_task(SCHEDULER_MONITOR_LIST, DOOR_SENSOR_TASK_ID, starttime, &_door_sensors_setup_write_report );	
}

void _door_sensors_setup_write_report() {
	char open_state = (DOOR_SENSOR_SWITCH_PIN)?SENSOR_OPEN:SENSOR_CLOSED;
	if (last_open_state != open_state ){
		door_sensor_state = (door_sensor_state & ~SENSOR_OPEN & ~SENSOR_CLOSED) | open_state;
		last_open_state = open_state;
	}
		
	if ( door_sensor_state ) {
		report_current()->fields |= REPORT_TYPE_DOOR_SENSORS;
		report_current()->door_sensor_state = door_sensor_state;
		door_sensor_state = 0;
	}
}

void _door_sensors_indoor_interrupt() {
	door_sensor_state |= SENSOR_INDOOR;
}

void _door_sensors_outdoor_interrupt() {
	door_sensor_state |= SENSOR_OUTDOOR;
}

void door_sensors_fmt_reading(uint8_t door_sensor_state, uint8_t bufsize, char * buf) {
	uint8_t n;
	n = snprintf_P(buf,bufsize,PSTR("Door=["));
	buf += n; bufsize -= n;
	if ( door_sensor_state & SENSOR_OPEN ) {
		n = snprintf_P(buf,bufsize, PSTR("OPEN "));
		buf += n; bufsize -= n;	
	}
	if ( door_sensor_state & SENSOR_CLOSED ) {
		n = snprintf_P(buf,bufsize, PSTR("CLOSED "));
		buf += n; bufsize -= n;
	}
	if ( door_sensor_state & SENSOR_INDOOR ) {
		n = snprintf_P(buf,bufsize, PSTR("INDOOR "));
		buf += n; bufsize -= n;
	}
	if ( door_sensor_state & SENSOR_OUTDOOR ) {
		n = snprintf_P(buf,bufsize, PSTR("OUTDOOR "));
		buf += n; bufsize -= n;
	}
	snprintf_P(buf,bufsize, PSTR("]"));
}

#endif