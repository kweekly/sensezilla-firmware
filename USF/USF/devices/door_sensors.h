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

void _door_sensors_indoor_interrupt();
void _door_sensors_outdoor_interrupt();
void _door_sensors_open_interrupt();

void door_sensors_fmt_reading(uint8_t door_sensor_state, uint8_t bufsize, char * buf);

#endif /* DOOR_SENSORS_H_ */