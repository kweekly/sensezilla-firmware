/*
 * l3gd20.h
 *
 * Created: 3/12/2013 5:49:14 PM
 *  Author: kweekly
 */ 


#ifndef L3GD20_H_
#define L3GD20_H_



typedef struct  {
	uint16_t X;
	uint16_t Y;
	uint16_t Z;
} gyro_reading_t;

void gyro_init(void);
gyro_reading_t gyro_read() ;

void gyro_wake(void);
void gyro_sleep(void);





#endif /* L3GD20_H_ */