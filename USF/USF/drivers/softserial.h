/*
 * softserial.h
 *
 * Created: 10/23/2013 2:52:05 PM
 *  Author: kweekly
 */ 


#ifndef SOFTSERIAL_H_
#define SOFTSERIAL_H_
#ifdef USE_SOFTSERIAL
#ifndef SOFTSERIAL_BUFSIZE
	#define SOFTSERIAL_BUFSIZE 28
#endif

#ifndef SOFTSERIAL_TX
	#error "SOFTSERIAL_TX not defined"
#endif

#ifndef SOFTSERIAL_RX_PIN
	#error "SOFTSERIAL_RX_PIN not defined"
#endif

#ifndef SOFTSERIAL_RX_PCINT
	#error "SOFTSERIAL_RX_PCINT not defined"
#endif

void softserial_init();
// note that we cannot recieve while writing
void softserial_write(uint8_t len, const uint8_t * bytes);
uint8_t softserial_available();
uint8_t softserial_read();

void _softserial_rx_isr();
#endif
#endif /* SOFTSERIAL_H_ */