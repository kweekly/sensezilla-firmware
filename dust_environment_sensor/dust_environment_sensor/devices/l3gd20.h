/*
 * l3gd20.h
 *
 * Created: 3/12/2013 5:49:14 PM
 *  Author: kweekly
 */ 


#ifndef L3GD20_H_
#define L3GD20_H_



typedef struct  {
	int16_t X;
	int16_t Y;
	int16_t Z;
} gyro_reading_t;

void gyro_init(void);
gyro_reading_t gyro_read() ;

void gyro_wake(void);
void gyro_sleep(void);
uint8_t gyro_read_status(void);

void gyro_fmt_reading(gyro_reading_t * reading, uint8_t maxlen, char * str);

void gyro_setup_reporting_schedule(uint16_t starttime);
void _gyro_reporting_finish();



#endif /* L3GD20_H_ */