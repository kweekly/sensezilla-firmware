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

// for reporting framework
void light_setup_reporting_schedule(uint16_t starttime);
void _light_reporting_finish(void);

void light_fmt_reading(light_reading_t * reading, uint8_t maxlen, char * str);
uint8_t light_convert_real(light_reading_t * reading, float * fltptr);

#endif /* TSL2560_H_ */