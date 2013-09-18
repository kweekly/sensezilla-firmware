/*
 * MACHXO2_PMCO2.c
 *
 * Created: 9/12/2013 1:04:46 PM
 *  Author: kweekly
 */ 
#include "devicedefs.h"
#ifdef USE_MACHXO2_PMCO2
#include "avrincludes.h"
#include "utils/scheduler.h"
#include "protocol/report.h"
#include "drivers/SPI.h"
#include "devices/MACHXO2_PMCO2.h"


#define MACHXO2_ADDR_ID			0x01
#define MACHXO2_ADDR_TEMP		0x02
#define MACHXO2_ADDR_PM_CHANNELS 0x10
#define MACHXO2_ADDR_PM_COUNTER  0x30
#define MACHXO2_ADDR_CO2_CHANNELS 0x40


void machxo2_init() {
	spi_init(SPI_MODE0 | SPI_CLKDIV4);
	EXP_CSN = 1;
	
	uint8_t ver = machxo2_readreg(MACHXO2_ADDR_ID);
	if ( ver != 0xFF ) {
		printf_P(PSTR("MACHXO2 detected ver=%02X\n"),ver);
	} else {
		kputs("Error: MACHXO2 not detected\n");
	}
	machxo2_writereg(MACHXO2_ADDR_TEMP, 0xAB);
	if ( machxo2_readreg(MACHXO2_ADDR_TEMP) != 0xAB) {
		kputs("Error: MACHXO2 write test failed\n");
	}
}

uint8_t machxo2_readreg(uint8_t address) {
	uint8_t retval;
	EXP_CSN = 0;
	spi_transfer(address);
	retval = spi_transfer(0xFF);
	EXP_CSN = 1;
	return retval;
}

void machxo2_readregs(uint8_t address, uint8_t len, uint8_t * data) {
	EXP_CSN = 0;
	spi_transfer(address);
	for ( size_t c = 0; c < len; c++ )
		data[c] = spi_transfer(0xFF);
	EXP_CSN = 1;
}

void machxo2_writereg(uint8_t address, uint8_t value) {
	EXP_CSN = 0;
	spi_transfer(address | 0x80);
	spi_transfer(value);
	EXP_CSN = 1;
}

void machxo2_writeregs(uint8_t address, uint8_t len, uint8_t * data) {
	EXP_CSN = 0;
	spi_transfer(address | 0x80);
	for ( size_t c = 0; c < len; c++ )
		spi_transfer(data[c]);
	EXP_CSN = 1;
}

void _machxo2_read() {
	machxo2_readregs(MACHXO2_ADDR_PM_CHANNELS,MACHXO2_NUM_PMINPUTS * 4, (uint8_t *)(report_current()->pm));
	machxo2_readregs(MACHXO2_ADDR_PM_COUNTER,4,(uint8_t *)(&(report_current()->pm_timer)));
	machxo2_readregs(MACHXO2_ADDR_CO2_CHANNELS,MACHXO2_NUM_CO2INPUTS * 2, (uint8_t *)(report_current()->co2));
}

void machxo2_setup_reporting_schedule(uint16_t starttime) {
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST, MACHXO2_TASK_ID, starttime, &_machxo2_read);
}

#endif