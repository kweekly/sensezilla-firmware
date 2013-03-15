/*
 * lis3dh.h
 *
 * Created: 3/12/2013 5:48:19 PM
 *  Author: kweekly
 */ 


#ifndef LIS3DH_H_
#define LIS3DH_H_


typedef struct  {
	uint16_t X;
	uint16_t Y;
	uint16_t Z;
} accel_reading_t;

void accel_init(void);
void accel_wake(void);
void accel_sleep(void);
accel_reading_t accel_read(void);

void accel_configure_click( void (*click_cb)(void) );

#define ACCEL_ORIENTATION_ZUP	0x20
#define ACCEL_ORIENTATION_ZDOWN 0x10
#define ACCEL_ORIENTATION_YUP	0x08
#define ACCEL_ORIENTATION_YDOWN 0x04
#define ACCEL_ORIENTATION_XUP	0x02
#define ACCEL_ORIENTATION_XDOWN	0x01

void accel_configure_orientation_detection( unsigned char detection_mask, void (*orient_cb)(unsigned char orientation) );

#endif /* LIS3DH_H_ */