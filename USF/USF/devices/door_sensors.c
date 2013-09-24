/*
 * door_sensors.c
 *
 * Created: 9/20/2013 11:41:29 AM
 *  Author: kweekly
 */ 

#include "devicedefs.h"
#ifdef USE_DOOR_SENSORS

#include <avr/io.h>
#include "utils/scheduler.h"
#include "protocol/report.h"
#include "devices/door_sensors.h"

void door_sensors_init() {
	EXP_CSN = 1;
	
}

void door_sensors_setup_interrupt_schedule(uint16_t starttime) {
	scheduler_add_task(SCHEDULER_MONITOR_LIST, DOOR_SENSOR_TASK_ID, starttime, &_door_sensors_setup_write_report );	
}

void _door_sensors_setup_write_report() {
	
	
	
}

#endif