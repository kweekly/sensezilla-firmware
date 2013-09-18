/*
 * MACHXO2_PMCO2.h
 *
 * Created: 9/12/2013 1:04:11 PM
 *  Author: kweekly
 */ 


#ifndef MACHXO2_PMCO2_H_
#define MACHXO2_PMCO2_H_



void machxo2_init();
uint8_t machxo2_readreg(uint8_t address);
void machxo2_readregs(uint8_t address, uint8_t len, uint8_t * data);
void machxo2_writereg(uint8_t address, uint8_t value);
void machxo2_writeregs(uint8_t address, uint8_t len, uint8_t * data);
void machxo2_setup_reporting_schedule(uint16_t starttime);

void _machxo2_read();

#endif /* MACHXO2_PMCO2_H_ */