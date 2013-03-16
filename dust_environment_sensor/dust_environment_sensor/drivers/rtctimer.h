/*
 * rtctimer.h
 *
 * Created: 3/15/2013 4:21:32 PM
 *  Author: kweekly
 */ 


#ifndef RTCTIMER_H_
#define RTCTIMER_H_
// this uses the 32KHz crystal attached to TOSC2, interrupt every second


void rtctimer_init();
uint32_t rtctimer_read();
void rtctimer_set_periodic_alarm(uint16_t period_s, void (* cb)(void));



#endif /* RTCTIMER_H_ */