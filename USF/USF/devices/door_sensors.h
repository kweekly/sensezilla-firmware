/*
 * door_sensors.h
 *
 * Created: 9/20/2013 11:41:17 AM
 *  Author: kweekly
 */ 


#ifndef DOOR_SENSORS_H_
#define DOOR_SENSORS_H_

void door_sensors_init();

void door_sensors_setup_interrupt_schedule();
void _door_sensors_setup_write_report();


#endif /* DOOR_SENSORS_H_ */