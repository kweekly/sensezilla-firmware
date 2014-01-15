/*
 * htu21d.c
 *
 * Created: 11/19/2013 11:32:35 AM
 *  Author: kweekly
 */ 
/*
 * si7005.c
 *
 * Created: 3/12/2013 5:48:30 PM
 *  Author: kweekly
 */ 

#include "devicedefs.h"
#ifdef USE_HTU21D

#include "avrincludes.h"
#include "protocol/report.h"

#include "devices/htu21d.h"
#include "drivers/i2cmaster.h"
#include "utils/scheduler.h"
#include "utils/crc.h"

#define CMD_TRIG_TEMP_HM 0xE3
#define CMD_TRIG_TEMP_NHM 0xF3
#define CMD_TRIG_HUMID_HM 0xE5
#define CMD_TRIG_HUMID_NHM 0xF5
#define CMD_WRITE_USR 0xE6
#define CMD_READ_USR 0xE7
#define CMD_RESET 0xFE

void humid_init(void) { // nothing here since we cut power to the device normally	
	
}

void humid_sleep(void) {

}

void humid_wake(void) {

}

#define HUMID_CRC_POLY ((uint16_t)0x131)
uint8_t _humid_crc(uint8_t * input, uint8_t len) {
	return crc8(HUMID_CRC_POLY, input, len);
}

humid_reading_t humid_read(void) {
	humid_reading_t retval;
	unsigned char buf[3];
	uint8_t crc;
	
	if ( i2c_writereg(HUMID_ADDR, CMD_TRIG_HUMID_NHM, 0, NULL) ) {
		kputs("Error Starting humidity conversion");
		return retval;
	}
	
	_delay_ms(16);
	
	if ( i2c_readbytes(HUMID_ADDR, 3, buf ) ) {
		kputs("Error reading humidity result");
		return retval;
	}
	crc = _humid_crc(buf,2);
	if ( crc != buf[2] ) {
		printf_P(PSTR("Humid CRC error. Expected %02X got %02X."),crc,buf[2]);
		return retval;
	}
	retval.humidity = ((uint16_t)buf[0] << 8) | (buf[1] & 0xFC); 
	
	
	if ( i2c_writereg(HUMID_ADDR, CMD_TRIG_TEMP_NHM, 0, NULL) ) {
		kputs("Error Starting temperature conversion");
		return retval;
	}
		
	_delay_ms(50);

	if ( i2c_readbytes(HUMID_ADDR, 3, buf ) ){
		kputs("Error reading temperature result");
		return retval;
	}
	crc = _humid_crc(buf,2);
	if ( crc != buf[2] ) {
		printf_P(PSTR("Temp CRC error. Expected %02X got %02X."),crc,buf[2]);
		return retval;
	}
	retval.temperature = ((uint16_t)buf[0] << 8) | (buf[1] & 0xFC); 
	
	return retval;
}

uint8_t humid_reporting_error_occurred;

void humid_setup_reporting_schedule(uint16_t curtime) {
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,HUMID_TASK_ID, curtime, &_humid_reporting_readh);
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,HUMID_TASK_ID, curtime += 16, &_humid_reporting_readt);
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,HUMID_TASK_ID, curtime += 50, &_humid_reporting_finish);
	humid_reporting_error_occurred = 0;
}

void _humid_reporting_readh(void) {
	report_t * r = report_current();
	if ( ! r ) return;
	
	if ( i2c_writereg(HUMID_ADDR, CMD_TRIG_HUMID_NHM, 0, NULL) ) {
		kputs("Error Starting humidity conversion");
		return;
	}
}

void _humid_reporting_readt(void){ // this saves the humidity as well
	unsigned char buf[3];
	uint8_t crc;
	
	report_t * r = report_current();
	
	if ( !r || humid_reporting_error_occurred ) return;
	
	if ( i2c_readbytes(HUMID_ADDR, 3, buf ) ) {
		kputs("Error reading humidity result");
		return;
	}
	crc = _humid_crc(buf,2);
	if ( crc != buf[2] ) {
		printf_P(PSTR("Humid CRC error. Expected %02X got %02X."),crc,buf[2]);
		return;
	}
	r->temphumid.humidity = ((uint16_t)buf[0] << 8) | (buf[1] & 0xFC); 
	r->fields |= REPORT_TYPE_HUMID;
	
	if ( i2c_writereg(HUMID_ADDR, CMD_TRIG_TEMP_NHM, 0, NULL) ) {
		kputs("Error Starting temperature conversion");
		return;
	}
}
	
void _humid_reporting_finish(void) {
	uint8_t crc;
	unsigned char buf[3];
	
	report_t * r = report_current();
	
	if ( !r || humid_reporting_error_occurred ) return;

	if ( i2c_readbytes(HUMID_ADDR, 3, buf )  ){
		kputs("Error reading temperature result");
		goto error;
	}
	crc = _humid_crc(buf,2);
	if ( crc != buf[2] ) {
		printf_P(PSTR("Temp CRC error. Expected %02X got %02X."),crc,buf[2]);
		goto error;
	}
	r->temphumid.temperature = ((uint16_t)buf[0] << 8) | (buf[1] & 0xFC);
	r->fields |= REPORT_TYPE_TEMP;
	
	error:
	humid_sleep();
}

#define DBL_16B 65536.0

void humid_fmt_reading(humid_reading_t * reading, uint8_t maxlen, char * str) {
	float rh = (reading->humidity) * 125.0 / DBL_16B - 6.0;
	float temp = (reading->temperature) *175.72/ DBL_16B - 46.85;
	snprintf_P(str,maxlen,PSTR("RH=%5.2f%% T=%5.2fC"), rh,temp);
}

uint8_t humid_convert_real(humid_reading_t * reading, float * flt) {
	float rh = (reading->humidity) * 125.0 / DBL_16B - 6.0;
	float temp = (reading->temperature) *175.72/ DBL_16B - 46.85;
	flt[0] = rh;
	flt[1] = temp;
	return 2;
}

#endif