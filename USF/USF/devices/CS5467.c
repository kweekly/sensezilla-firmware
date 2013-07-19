/*
 * CS5467.c
 *
 * Created: 5/15/2013 4:40:43 PM
 *  Author: kweekly
 */ 
#include "devicedefs.h"
#ifdef USE_CS5467

#include "avrincludes.h"
#include "devices/CS5467.h"
#include "protocol/report.h"
#include "drivers/SPI.h"
#include "utils/scheduler.h"

#define FORALLCHIPS(c) for (uint8_t c = 0; c < POWERMON_NUM_CHIPS; c++)

#define CAL_CHANNEL_I1 1
#define CAL_CHANNEL_V1 2
#define CAL_CHANNEL_I2 4
#define CAL_CHANNEL_V2 8

#define PWR_RESET 0
#define PWR_SLEEP 1
#define PWR_WAKEUP 2
#define PWR_STANDBY 3

#define PAGE0 0
#define PAGE1 (1<<5)
#define PAGE2 (2<<5)
#define PAGE5 (5<<5)

#define CONFIG_REG     (PAGE0 | 0)

#define I1_REG         (PAGE0 | 1)
#define V1_REG         (PAGE0 | 2)
#define P1_REG         (PAGE0 | 3)
#define P1_AVG_REG         (PAGE0 | 4)
#define I1_RMS_REG         (PAGE0 | 5)
#define V1_RMS_REG         (PAGE0 | 6)

#define I2_REG         (PAGE0 | 7)
#define V2_REG         (PAGE0 | 8)
#define P2_REG         (PAGE0 | 9)
#define P2_AVG_REG         (PAGE0 | 10)
#define I2_RMS_REG         (PAGE0 | 11)
#define V2_RMS_REG         (PAGE0 | 12)

#define Q1_AVG_REG         (PAGE0 | 13)
#define Q1_REG         (PAGE0 | 14)

#define STATUS_REG     (PAGE0 | 15)

#define Q2_AVG_REG         (PAGE0 | 16)
#define Q2_REG         (PAGE0 | 17)

#define I1_PEAK_REG    (PAGE0 | 18)
#define V1_PEAK_REG    (PAGE0 | 19)
#define S1_REG    (PAGE0 | 20)
#define PF1_REG    (PAGE0 | 21)

#define I2_PEAK_REG    (PAGE0 | 22)
#define V2_PEAK_REG    (PAGE0 | 23)
#define S2_REG    (PAGE0 | 24)
#define PF2_REG    (PAGE0 | 25)

#define MASK_REG (PAGE0 | 26)
#define T_REG  (PAGE0 | 27)
#define CTRL_REG       (PAGE0 | 28)

#define E_PULSE_REG (PAGE0 | 29)
#define S_PULSE_REG (PAGE0 | 30)
#define Q_PULSE_REG  (PAGE0 | 31)
#define PAGE_REG (PAGE0 | 31)

#define I1_OFF_REG (PAGE1 | 0)
#define I1_GAIN_REG (PAGE1 | 1)
#define V1_OFF_REG  (PAGE1 | 2)
#define V1_GAIN_REG (PAGE1 | 3)
#define P1_OFF_REG (PAGE1 | 4)
#define I1_ACOFF_REG  (PAGE1 | 5)
#define V1_ACOFF_REG  (PAGE1 | 6)

#define I2_OFF_REG (PAGE1 | 7)
#define I2_GAIN_REG (PAGE1 | 8)
#define V2_OFF_REG  (PAGE1 | 9)
#define V2_GAIN_REG (PAGE1 | 10)
#define P2_OFF_REG (PAGE1 | 11)
#define I2_ACOFF_REG  (PAGE1 | 12)
#define V2_ACOFF_REG  (PAGE1 | 13)

#define PULSEWIDTH_REG (PAGE1 | 14)
#define PULSERATE_REG (PAGE1 | 15)
#define MODES_REG (PAGE1 | 16)
#define EPSILON_REG (PAGE1 | 17)
#define N_REG (PAGE1 | 19)
#define Q1_WB_REG (PAGE1 | 20)
#define Q2_WB_REG (PAGE1 | 21)
#define T_GAIN_REG (PAGE1 | 22)
#define T_OFF_REG (PAGE1 | 23)
#define T_SETTLE_REG (PAGE1 | 25)
#define LOAD_MIN_REG (PAGE1 | 26)
#define VF_RMS_REG (PAGE1 | 27)
#define G_REG (PAGE1 | 28)
#define TIME_REG (PAGE1 | 29)

#define V1_SAG_DUR_REG (PAGE2 | 0)
#define V1_SAG_LEVEL_REG  (PAGE2 | 1)
#define I1_FAULT_DUR_REG  (PAGE2 | 4)
#define I1_FAULT_LEVEL_DUR  (PAGE2 | 5)

#define V2_SAG_DUR_REG (PAGE2 | 8)
#define V2_SAG_LEVEL_REG  (PAGE2 | 9)
#define I2_FAULT_DUR_REG  (PAGE2 | 12)
#define I2_FAULT_LEVEL_DUR  (PAGE2 | 13)

#define T_MEAS_REG (PAGE5 | 26)

// Empirical Values
#define DEFAULT_I_GAIN 10676955L // 1.8 * 2 / sqrt(2) = 2.54558441227
#define DEFAULT_I_NORM 20.0
#define DEFAULT_I_OFFSET 0L

// WITH 500 OHM R21
// by default, max Voltage is 250V -> 187.9mVpp ( out of 500mVpp ), gain should be 2.66098988824 / sqrt(2) = 1.88160399464
// Calculated Values

#define DEFAULT_V_GAIN 7892019L
#define DEFAULT_V_NORM 250.0
#define DEFAULT_V_OFFSET 0L


uint8_t current_page[POWERMON_NUM_CHIPS];

void powermon_init() {
	spi_init(SPI_MODE0 | SPI_CLKDIV16 | SPI_MSBFIRST);
	FORALLCHIPS(c) {
		_powermon_CSH(c);
		current_page[c] = 0xAB;
	}
	
	// turn on clocks for ICs. CLK/4 on OC2A
	// Compare mode, Toggle OC2A on compare match
	OCR2A = 1;
	
	// Toggle on compare match + turn on CTC mode
	TCCR2A = (1<<COM2A0) | (1<<WGM21);
		
	// no prescaler
	TCCR2B = (1<<CS20);
	
	_delay_ms(10); // give time to settle the clock

	FORALLCHIPS(c) {
		powermon_writereg(c, CTRL_REG, 0x00000006L);// gain set to +/-250mV
		// clear status bit
		powermon_writereg(c,STATUS_REG,0x800000);
		powermon_reset(c);
		_delay_ms(10); // give some time to settle
		powermon_writereg(c, CTRL_REG, 0x00000006L);// gain set to +/-250mV
		_delay_ms(10); // more time to settle
		powermon_wait_until_ready(c);
		
		powermon_writereg(c, MODES_REG,0x004001E1L); // HPF enabled, line frequency meas. enabled
		powermon_set_gains_and_offsets(DEFAULT_I_GAIN, DEFAULT_I_OFFSET, DEFAULT_V_GAIN, DEFAULT_V_OFFSET);
		powermon_writereg(c, P1_OFF_REG,0);
		powermon_writereg(c, P2_OFF_REG,0);
		powermon_startconversion(c,1);
		powermon_wait_until_ready(c);
	}
}

void powermon_set_gains_and_offsets(uint32_t iGain, uint32_t iOffset, uint32_t vGain, uint32_t vOffset) {
	FORALLCHIPS(c) {
		powermon_writereg(c, I1_GAIN_REG,iGain);
		powermon_writereg(c, I1_OFF_REG, iOffset);
		powermon_writereg(c, V1_GAIN_REG,vGain);
		powermon_writereg(c, V1_OFF_REG,vOffset);
		powermon_writereg(c, I2_GAIN_REG,iGain);
		powermon_writereg(c, I2_OFF_REG, iOffset);
		powermon_writereg(c, V2_GAIN_REG,vGain);
		powermon_writereg(c, V2_OFF_REG,vOffset);
	}
}

 void powermon_read(powermon_reading_t retval[POWERMON_NUM_CHANNELS]) {
	
	FORALLCHIPS(c) {
      retval[c*2].true_power = powermon_readreg(c, P1_AVG_REG);
      retval[c*2+1].true_power =  powermon_readreg(c, P2_AVG_REG);
      
      retval[c*2].RMS_voltage =  powermon_readreg(c, V1_RMS_REG);
      retval[c*2+1].RMS_voltage =  powermon_readreg(c, V2_RMS_REG);
      
      retval[c*2].RMS_current =  powermon_readreg(c, I1_RMS_REG);
      retval[c*2+1].RMS_current =  powermon_readreg(c, I2_RMS_REG);
	  
	  //printf_P(PSTR("%d V1:%ld I1:%ld V2:%ld I2:%ld\n"),c,retval.RMS_voltage[c*2],retval.RMS_current[c*2],retval.RMS_voltage[c*2+1],retval.RMS_current[c*2+1]);
	}
	
}

void powermon_reset(uint8_t chipno) {
	_powermon_CSL(chipno);
	spi_transfer(0b10000000);
	_powermon_CSH(chipno);
}

void powermon_startconversion(uint8_t chipno, uint8_t continuous) {
   _powermon_CSL(chipno);
   spi_transfer(0b11100000 | (continuous<<3));
   _powermon_CSH(chipno);
}

void powermon_wait_until_ready(uint8_t chipno) {
	uint32_t stat = powermon_readreg(chipno,STATUS_REG);
	uint16_t cntr = 0;
	while (!( stat & 0x800000 )) {
		stat = powermon_readreg(chipno, STATUS_REG);
		if ( cntr++ % (1<<14L) == 0) {
			//powermon_reset(chipno);
			printf_P(PSTR("%d Still waiting for ready %08lX\n"),chipno, stat);	
		}
	}
	printf_P(PSTR("\tPower chip %d status %08lX\n"),chipno+1,stat);
	// clear status bit
	powermon_writereg(chipno,STATUS_REG,0x800000);
}

void powermon_change_page(uint8_t chipno, uint8_t address) {
	uint8_t page = (0xFF & (address>>5));
	if ( page != current_page[chipno] ) {
		powermon_writereg(chipno, PAGE_REG, page );
		current_page[chipno] = page;
	}
}

uint32_t powermon_readreg(uint8_t chipno, uint8_t address) {
    if( address != PAGE_REG ){
	    powermon_change_page(chipno,address);
    }
    uint32_t retval;
    _powermon_CSL(chipno);
    spi_transfer(((address&0x1F)<<1));
    retval = ((uint32_t)0xFF  & spi_transfer(0xFF)) << (uint32_t)16L;
    retval |= ((uint32_t)0xFF  & spi_transfer(0xFF)) << (uint32_t)8L;
    retval |= ((uint32_t)0xFF  & spi_transfer(0xFF));
    _powermon_CSH(chipno);
    return retval;
}

void powermon_writereg(uint8_t chipno, uint8_t address, uint32_t data) {
    if( address != PAGE_REG ){
	    powermon_change_page(chipno,address);
    }

     _powermon_CSL(chipno);
    spi_transfer(((address&0x1F)<<1) | 0x40 );
    spi_transfer((data>>16)&0xFF);
    spi_transfer((data>>8)&0xFF);
    spi_transfer(data&0xFF);
    _powermon_CSH(chipno);
}

void powermon_setup_reporting_schedule(uint16_t starttime) {
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,POWERMON_TASK_ID,starttime,&_powermon_write_report);
}

void powermon_fmt_reading(powermon_reading_t reading[POWERMON_NUM_CHANNELS],int16_t maxlen, char * str) {
	float rval[POWERMON_NUM_CHANNELS * 3];
	uint16_t len;
	
	powermon_convert_real(reading, (float*)(&rval));
	for(uint8_t c = 0; c < POWERMON_NUM_CHANNELS; c++ ) {	
		len = snprintf_P(str,maxlen,PSTR(" CH%d (%.1fW,%dmA,%.0fV)"),c+1,rval[c*3],(int)(1000*rval[c*3+1]+.5),rval[c*3+2]);
		maxlen -= len;
		str += len;
		if (maxlen < 0) return;
	}
	
}

uint8_t powermon_convert_real(powermon_reading_t reading[POWERMON_NUM_CHANNELS],float * fltptr) {
	for (uint8_t c = 0; c < POWERMON_NUM_CHANNELS; c++ ) {
		fltptr[c*3] =		DEFAULT_I_NORM * DEFAULT_V_NORM * _powermon_itof(reading[c].true_power,1);
		fltptr[c*3 + 1] =	DEFAULT_I_NORM * _powermon_itof(reading[c].RMS_current,0);
		fltptr[c*3 + 2] =	DEFAULT_V_NORM * _powermon_itof(reading[c].RMS_voltage,0);
	}	
	return 3*POWERMON_NUM_CHANNELS;
}

void _powermon_write_report() {
	powermon_read(report_current()->power);
	report_current()->fields |= ((1<<POWERMON_NUM_CHANNELS)-1); // power bits are the lower 6 (or 4) bits
}

void _powermon_CSL(uint8_t chipno) {
	if (chipno == 0) {
		POWERMON_CS1 = 0;
	} 
	#ifdef POWERMON_CS2
	else if (chipno == 1) {
		POWERMON_CS2 = 0;
	}
	#endif
	#ifdef POWERMON_CS3
	else if (chipno == 2) {
		POWERMON_CS3 = 0;
	}
	#endif
	#ifdef POWERMON_CS4
	else if (chipno == 3) {
		POWERMON_CS4 = 0;
	}
	#endif
	else {
		printf_P(PSTR("Error: Invalid chipno %d\n"),chipno);
	}
}

void _powermon_CSH(uint8_t chipno) {
	if (chipno == 0) {
		POWERMON_CS1 = 1;
	}
	#ifdef POWERMON_CS2
	else if (chipno == 1) {
		POWERMON_CS2 = 1;
	}
	#endif
	#ifdef POWERMON_CS3
	else if (chipno == 2) {
		POWERMON_CS3 = 1;
	}
	#endif
	#ifdef POWERMON_CS4
	else if (chipno == 3) {
		POWERMON_CS4 = 1;
	}
	#endif
	else {
		printf_P(PSTR("Error: Invalid chipno %d\n"),chipno);
	}
}

float _powermon_itof(uint32_t reg, char sign_bit) {
	  uint32_t rval = 0;
	  uint8_t i = 0;

	  if(sign_bit) {
		  if ( reg & (uint32_t)0x00800000L )
		  rval = 0x80000000L;
		  reg <<= 1;
		  reg &= 0xFFFFFFL;
	  }
	  if ( reg == 0 ) return 0;


	  while ( !(reg & (uint32_t)0x00800000L) ) {
		  reg <<= 1;
		  i++;
	  }
	  i++;
	  rval = rval | ((uint32_t)(127-i)<<(uint32_t)23) | (reg & 0x7FFFFFL);
	  return *(float *)&rval;
}

#endif