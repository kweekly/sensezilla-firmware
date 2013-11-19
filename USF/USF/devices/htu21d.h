/*
 * htu21d.h
 *
 * Created: 11/19/2013 11:32:24 AM
 *  Author: kweekly
 */ 


#ifndef HTU21D_H_
#define HTU21D_H_

typedef struct {
	uint16_t humidity;
	uint16_t temperature;
} humid_reading_t;

void humid_init(void);
void humid_sleep(void);
void humid_wake(void);

humid_reading_t humid_read(void);

// for reporting framework
void humid_setup_reporting_schedule(uint16_t starttime);
void _humid_reporting_readh(void);
void _humid_reporting_readt(void); // this saves the humidity as well
void _humid_reporting_finish(void);

void humid_fmt_reading(humid_reading_t * reading, uint8_t maxlen, char * str);
uint8_t humid_convert_real(humid_reading_t * reading, float * flt) ;

#endif /* HTU21D_H_ */