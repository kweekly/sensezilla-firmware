/*
 * si7005.h
 *
 * Created: 3/12/2013 5:48:38 PM
 *  Author: kweekly
 */ 


#ifndef SI7005_H_
#define SI7005_H_

typedef struct {
	uint16_t humidity;
	uint16_t temperature;
} humid_reading_t;

void humid_init(void);
void humid_sleep(void);
void humid_wake(void);

humid_reading_t humid_read(void);


#endif /* SI7005_H_ */