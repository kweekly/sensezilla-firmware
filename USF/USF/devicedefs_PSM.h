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


// LEDs
#define LED_BLIP_TASK_ID 0x01



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