/*
 * lis3dh.h
 *
 * Created: 3/12/2013 5:48:19 PM
 *  Author: kweekly
 */ 


#ifndef LIS3DH_H_
#define LIS3DH_H_


typedef struct  {
	int16_t X;
	int16_t Y;
	int16_t Z;
} accel_reading_t;

void accel_init(void);
void accel_wake(void);
void accel_sleep(void);
uint8_t accel_read_status(void);
accel_reading_t accel_read(void);

void accel_configure_click( void (*click_cb)(unsigned char) );

#define ACCEL_ORIENTATION_ZUP	0x20
#define ACCEL_ORIENTATION_ZDOWN 0x10
#define ACCEL_ORIENTATION_YUP	0x08
#define ACCEL_ORIENTATION_YDOWN 0x04
#define ACCEL_ORIENTATION_XUP	0x02
#define ACCEL_ORIENTATION_XDOWN	0x01
#define ACCEL_ORIENTATION_ALL 0x3F

void accel_configure_orientation_detection( unsigned char detection_mask, void (*orient_cb)(unsigned char orientation) );

void _accel_int1_cb();
void _accel_int2_cb();

void accel_fmt_reading(accel_reading_t * reading, uint8_t maxlen, char * str);
uint8_t accel_convert_real(accel_reading_t * reading, float * fltptr);

void accel_setup_reporting_schedule(uint16_t starttime);
void _accel_reporting_doread();

void accel_setup_interrupt_schedule(uint16_t starttime);
void accel_configure_interrupt(uint8_t setup_time);
void _accel_interrupt_check();
void _accel_interrupt_orient_cb(unsigned char orientation);
#endif /* LIS3DH_H_ */