/*
 * softserial.c
 *
 * Created: 10/23/2013 2:51:58 PM
 *  Author: kweekly
 */ 
#include "devicedefs.h"
#ifdef USE_SOFTSERIAL
#include "avrincludes.h"
#include "drivers/pcint.h"
#include "drivers/softserial.h"

uint8_t buffer[SOFTSERIAL_BUFSIZE];
uint8_t bufstart,bufend;
uint8_t transmitting;
uint8_t receiving;
uint8_t bit_no;

// for 9600 baud
#define BIT_PERIOD_0_5	52
#define BIT_PERIOD		104
#define BIT_PERIOD_1_5  156
#define BIT_PERIOD_2   208
#define RX_COMPENSATE_START 60

ISR(TIMER0_COMPA_vect) {
	//TCNT0 = 0;
	//EXP_SCK = 1;
	if ( transmitting ) {
		if (bit_no == 8) { // send stop bit
			SOFTSERIAL_TX = 1;
			bit_no ++;
		} else if ( bit_no == 9 ) { // start next byte or stop transmitting
			bit_no = 0;
			if ( bufstart == bufend - 1 ) {
				transmitting = 0;
				bufstart = bufend = 0;
				TIMSK0 = 0;
			} else {
				SOFTSERIAL_TX = 0;
				bufstart ++;
			}
		} else {
			uint8_t ch = buffer[bufstart];
			if (ch & _BV(bit_no))
				SOFTSERIAL_TX = 1;
			else
				SOFTSERIAL_TX = 0;
			bit_no ++;
		}		
	} else if ( receiving ) {
		OCR0A = BIT_PERIOD;
		if ( bit_no == 8 )  {
			TIMSK0 = 0;
			receiving = 0;
		} else {
			buffer[bufend] |= (SOFTSERIAL_RX_PIN << bit_no);
			bit_no++;
			if ( bit_no == 8 ) {
				bufend = (bufend + 1) % SOFTSERIAL_BUFSIZE;
			}
		}
	}
	//EXP_SCK = 0;
}

void _softserial_rx_isr() {
	if (!transmitting && !receiving && (SOFTSERIAL_RX_PIN == 0)) {
		//EXP_SCK = 1;
		bit_no = 0;
		TCNT0 = 0;
		OCR0A = BIT_PERIOD_1_5 - RX_COMPENSATE_START;
		TIFR0 |= 0xFF;
		TIMSK0 |= _BV(OCIE0A);
		//pcint_unregister(SOFTSERIAL_RX_PCINT);
		receiving = 1;
		buffer[bufend] = 0;
		//EXP_SCK = 0;
	}
	
}

void softserial_init() {
	transmitting = 0;
	receiving = 0;
	bufstart = bufend = 0;
	
	TIMSK0 = 0 ; // interrupts disabled
	TCCR0A = 0x02; // CTC mode
	TCCR0B = 0x02; // 8x prescaling (1 tick is 1us)
	
	SOFTSERIAL_TX = 1;
	pcint_register(SOFTSERIAL_RX_PCINT, &_softserial_rx_isr);
}

void softserial_write(uint8_t len, const uint8_t * bytes) {
	if ( receiving ) {
		kputs("Cannot write while recieving\n");
		return;
	}
	transmitting = 1;
	memcpy(buffer,bytes,len);
	bufstart = 0;
	bufend = len;
	
	bit_no = 0;
	SOFTSERIAL_TX = 0;
	TCNT0 = 0;
	OCR0A = BIT_PERIOD;
	TIFR0 = 0xFF;
	TIMSK0 |= _BV(OCIE0A);
}

uint8_t softserial_available() {
	if ( transmitting ) return 0;
	
	return (bufend + SOFTSERIAL_BUFSIZE - bufstart)%SOFTSERIAL_BUFSIZE;
}

uint8_t softserial_read() {
	if ( transmitting || bufstart == bufend) return 0xFF;
	
	uint8_t r = buffer[bufstart];
	bufstart = (bufstart + 1)%SOFTSERIAL_BUFSIZE;
	return r;
}
#endif