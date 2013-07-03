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

#define HW_VERSION 2

// DDRs
#if HW_VERSION==1
#define DDRB_SETTING	0b10110010
#elif HW_VERSION==2
#define DDRB_SETTING	0b10110000
#endif
#define DDRA_SETTING	0b10111100
#define DDRC_SETTING	0b00000000 // hopefully automatically initialized by i2c port
#define DDRD_SETTING	0b01111010


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
#define MOTE_SLEEPN		REGISTER_BIT(PORTA,5)
#if HW_VERSION==1
#define MOTE_TIMEN		REGISTER_BIT(PORTA,3)
#define MOTE_TX_RTSN	REGISTER_BIT(PORTB,2)
#define MOTE_RX_CTSN	REGISTER_BIT(PORTB,0)
#define MOTE_TX_CTSN	REGISTER_BIT(PORTB,1)
#define MOTE_RESETN		EXP_CSN  // workaround
#elif HW_VERSION==2
#define MOTE_STATUS		REGISTER_BIT(PORTB,2)
#define MOTE_ASSOC		REGISTER_BIT(PORTB,1)
#define MOTE_RESETN		REGISTER_BIT(PORTA,4)
#endif
#define MOTE_TX_CTSN	REGISTER_BIT(PORTB,0)
#define MOTE_RX_RTSN	REGISTER_BIT(PORTD,6)
#define MOTE_UART_GETC	uart1_getc
#define MOTE_UART_WRITE uart1_write
#define MOTE_UART_INIT  uart1_init
#define XBEE_RTS_ENABLED

// LEDs
#define LED_BLIP_TASK_ID 0x01
#define LED1		REGISTER_BIT(PORTD,5)
#define LED2		REGISTER_BIT(PORTD,4)

// PIR sensor
#define PIR_TASK_ID 0x11
#define PIR_VCC		REGISTER_BIT(PORTA,7)
#define PIR_OUT		REGISTER_BIT(PORTD,7)
#define PIR_OUT_PCINT 31
#define PIR_OUT_PIN	REGISTER_BIT(PIND,7)

// Accelerometer LIS3DH
#define ACCEL_TASK_ID 0x10
#define ACCEL_ADDR	0b00110010
#define ACCEL_INT1_PCINT 0
#define ACCEL_INT1_PIN REGISTER_BIT(PINA,0)
#define ACCEL_INT1	REGISTER_BIT(PORTA,0)
#define ACCEL_INT2_PCINT 1
#define ACCEL_INT2_PIN REGISTER_BIT(PINA,1)
#define ACCEL_INT2	REGISTER_BIT(PORTA,1)


// Gyro L3GD20
#define GYRO_TASK_ID 0x20
#define GYRO_ADDR	0b11010110
#define GYRO_INT2_PCINT 11
#define GYRO_INT2	REGISTER_BIT(PORTB,3)

// Temp/Humid SI7005
#define HUMID_TASK_ID 0x30
#define TEMP_TASK_ID 0x40
#define HUMID_ADDR	0b10000000
#if HW_VERSION==1
#define HUMID_VCC REGISTER_BIT(PORTA,2)
#elif HW_VERSION==2
#define HUMID_CSN  REGISTER_BIT(PORTA,2)
#define HUMID_VCC	REGISTER_BIT(PORTA,3)
#endif

// Amb. Light
#define LIGHT_TASK_ID 0x50
#define LIGHT_ADDR	0b01010010
//#define LIGHT_ADDR	0b01010110
#define LIGHT_INT_PCINT 6
#define LIGHT_INT	REGISTER_BIT(PORTA,6)

// RFID Reader PN532
#define RFID_TASK_ID 0x60
#define RFID_CSN EXP_CSN


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