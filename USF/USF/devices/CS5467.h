/*
 * CS5467.h
 *
 * Created: 5/15/2013 4:41:02 PM
 *  Author: kweekly
 */ 


#ifndef CS5467_H_
#define CS5467_H_

typedef struct {
	uint32_t true_power;
	uint32_t RMS_current;
	uint32_t RMS_voltage;
} powermon_reading_t;

void powermon_init();
void powermon_set_gains_and_offsets(uint32_t iGain, uint32_t iOffset, uint32_t vGain, uint32_t vOffset);

void powermon_read(powermon_reading_t reading[POWER_STRIP_MONITOR]);
void powermon_reset(uint8_t chipno);
void powermon_wait_until_ready(uint8_t chipno);
void powermon_change_page(uint8_t chipno, uint8_t address);
uint32_t powermon_readreg(uint8_t chipno, uint8_t address);
void powermon_writereg(uint8_t chipno, uint8_t address, uint32_t data);
void powermon_startconversion(uint8_t chipno, uint8_t continuous);

void powermon_setup_reporting_schedule(uint16_t starttime);
void powermon_fmt_reading(powermon_reading_t reading[POWERMON_NUM_CHANNELS],int16_t maxlen, char * str);
uint8_t powermon_convert_real(powermon_reading_t reading[POWERMON_NUM_CHANNELS],float * fltptr);

void _powermon_write_report();

void _powermon_CSL(uint8_t chipno);
void _powermon_CSH(uint8_t chipno);

float _powermon_itof(uint32_t reg, char sign_bit);


#endif /* CS5467_H_ */