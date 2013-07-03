/*
 * amn41121.h
 *
 * Created: 3/12/2013 5:49:54 PM
 *  Author: kweekly
 */ 


#ifndef AMN41121_H_
#define AMN41121_H_

void pir_init();
void pir_sleep();
void pir_wake(void);
#define pir_read() (PIR_OUT_PIN)
void __pir_pcint_cb();


void pir_setup_reporting_schedule(uint16_t starttime);
void _pir_reporting_write_report();

void pir_fmt_reading(float * reading, uint8_t maxlen, char * str);

#endif /* AMN41121_H_ */