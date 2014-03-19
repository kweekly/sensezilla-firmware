/*
 * tsl2560.h
 *
 * Created: 3/12/2013 5:49:02 PM
 *  Author: kweekly
 */ 


#ifndef TSL2560_H_
#define TSL2560_H_

#define LIGHT_LEVEL_DRAWER_CLOSED_MAX 0x05
#define LIGHT_LEVEL_DRAWER_OPEN_MIN	  0x10
#define LIGHT_LEVEL_OFFICE_SPACE_MIN  0x50

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

void light_setup_interrupt_schedule(uint16_t starttime);
void light_set_interrupt_params(uint16_t level, uint16_t hysteresis);
void _light_interrupt_finish();

void light_fmt_reading(light_reading_t * reading, uint8_t maxlen, char * str);
uint8_t light_convert_real(light_reading_t * reading, float * fltptr);

void light_fmt_reading_raw(light_reading_t * reading, uint8_t maxlen, char * str);
uint8_t light_convert_real_raw(light_reading_t * reading, float * fltptr);

#endif /* TSL2560_H_ */