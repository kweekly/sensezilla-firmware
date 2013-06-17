/*
 * devicedefs.h
 *
 * Created: 3/12/2013 4:27:35 PM
 *  Author: kweekly
 */ 


#ifndef DEVICEDEFS_H_
#define DEVICEDEFS_H_

typedef struct
{
	unsigned int bit0:1;
	unsigned int bit1:1;
	unsigned int bit2:1;
	unsigned int bit3:1;
	unsigned int bit4:1;
	unsigned int bit5:1;
	unsigned int bit6:1;
	unsigned int bit7:1;
} _io_reg;

#define REGISTER_BIT(rg,bt) ((volatile _io_reg*)&rg)->bit##bt 

// DDRs
#define DDRA_SETTING	0b11111111
#define DDRB_SETTING	0b10111111
#define DDRC_SETTING	0b11000010
#define DDRD_SETTING	0b11101010


// Expansion
#define EXP_SCK		REGISTER_BIT(PORTB,7)
#define EXP_MISO	REGISTER_BIT(PORTB,6)
#define EXP_MOSI	REGISTER_BIT(PORTB,5)
#define EXP_CSN		REGISTER_BIT(PORTB,4)

#define EXP_SCK_DDR		REGISTER_BIT(DDRB,7)
#define EXP_MISO_DDR	REGISTER_BIT(DDRB,6)
#define EXP_MOSI_DDR	REGISTER_BIT(DDRB,5)
#define EXP_CSN_DDR		REGISTER_BIT(DDRB,4)

// Mote
#define MOTE_TASK_ID	0x05
#define MOTE_RESETN		REGISTER_BIT(PORTC,1)
#define MOTE_SLEEPN		REGISTER_BIT(PORTA,6)
#define MOTE_TIMEN		REGISTER_BIT(PORTA,5)
#define MOTE_TX_RTSN	REGISTER_BIT(PORTD,4)
#define MOTE_TX_CTSN	REGISTER_BIT(PORTD,5)
#define MOTE_RX_CTSN	REGISTER_BIT(PORTC,0)
#define MOTE_RX_RTSN	REGISTER_BIT(PORTD,6)
#define MOTE_UART_GETC	uart1_getc
#define MOTE_UART_WRITE uart1_write
#define MOTE_UART_INIT  uart1_init

// LEDs
#define LED_BLIP_TASK_ID 0x01
#define LED1		REGISTER_BIT(PORTA,0)
#define LED2		REGISTER_BIT(PORTA,1)

// TRIAC gate controls
#define GATE1		REGISTER_BIT(PORTB,0)
#define GATE2		REGISTER_BIT(PORTB,1)
#define GATE3		REGISTER_BIT(PORTB,2)
#define GATE4		REGISTER_BIT(PORTB,3)
#define GATE5		REGISTER_BIT(PORTB,4)
#define GATE6		REGISTER_BIT(PORTC,7)

// CS5467 Chips
#define POWERMON_TASK_ID 0x10
#define POWERMON_NUM_CHIPS 3
#define POWERMON_NUM_CHANNELS 6

#define POWERMON_CS1 REGISTER_BIT(PORTA,4)
#define POWERMON_CS2 REGISTER_BIT(PORTA,3)
#define POWERMON_CS3 REGISTER_BIT(PORTA,2)

#ifndef sbi
#define sbi(X,Y) (X) |= _BV(Y)
#endif

#ifndef cbi
#define cbi(X,Y) (X) &= ~_BV(Y)
#endif

#ifndef tbs
#define tbs(X,Y) (((X) & _BV(Y)) != 0)
#endif

#ifndef tbc
#define tbc(X,Y) (((X) & _BV(Y)) == 0)
#endif



// Convenience
#include <stdio.h>
#include <avr/pgmspace.h>
#include "drivers/uart.h"
#define kputs(X) uart_puts_p(PSTR(X))

#endif /* DEVICEDEFS_H_ */