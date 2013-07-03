/*
 * pcint.h
 *
 * Created: 3/27/2013 2:34:52 PM
 *  Author: kweekly
 */ 


#ifndef PCINT_H_
#define PCINT_H_


void pcint_init();
void pcint_latchall();
void pcint_register(uint8_t intnum, void (*cb)(void));
void pcint_unregister(uint8_t intnum);
void pcint_check();
#endif /* PCINT_H_ */