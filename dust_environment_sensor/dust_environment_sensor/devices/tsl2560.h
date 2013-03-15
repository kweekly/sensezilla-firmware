/*
 * tsl2560.h
 *
 * Created: 3/12/2013 5:49:02 PM
 *  Author: kweekly
 */ 


#ifndef TSL2560_H_
#define TSL2560_H_

typedef struct  {
	uint16_t ch0;
	uint16_t ch1;
} light_reading_t;

void light_init(void);
light_reading_t light_read();

void light_wake();
void light_sleep();



#endif /* TSL2560_H_ */