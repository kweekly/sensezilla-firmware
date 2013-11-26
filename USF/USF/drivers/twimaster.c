/*************************************************************************
* Title:    I2C master library using hardware TWI interface
* Author:   Peter Fleury <pfleury@gmx.ch>  http://jump.to/fleury
* File:     $Id: twimaster.c,v 1.3 2005/07/02 11:14:21 Peter Exp $
* Software: AVR-GCC 3.4.3 / avr-libc 1.2.3
* Target:   any AVR device with hardware TWI 
* Usage:    API compatible with I2C Software Library i2cmaster.h
**************************************************************************/
#include "devicedefs.h"
#include <inttypes.h>
#include <compat/twi.h>

#include "i2cmaster.h"
#include "avrincludes.h"


/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#error "F_CPU Not defined!"
#define F_CPU 4000000UL
#endif

/* I2C clock in Hz */
#define SCL_CLOCK		100000L
#define SCL_CLOCK_FAST  400000L

#define I2C_TIMEOUT 5000 // in 1us increments

/*************************************************************************
 Initialization of the I2C bus interface. Need to be called only once
*************************************************************************/
void i2c_init(void)
{
  /* initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1 */
  
  TWSR = 0;                         /* no prescaler */
  TWBR = ((F_CPU/SCL_CLOCK)-16)/2;  /* must be > 10 for stable operation */

  // reset
  TWCR = 0;
  
}/* i2c_init */


/*************************************************************************	
  Issues a start condition and sends address and transfer direction.
  return 0 = device accessible, 1= failed to access device
*************************************************************************/
unsigned char i2c_start(unsigned char address)
{
	uint16_t tmout = I2C_TIMEOUT;
    uint8_t   twst;

	// send START condition
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	// wait until transmission completed
	while(!(TWCR & (1<<TWINT)) && --tmout) { _delay_us(1); }
	if ( !tmout ) {
		kputs("I2C Timeout\n");
		return 1;
	}

	// check value of TWI Status Register. Mask prescaler bits.
	twst = TW_STATUS & 0xF8;
	if ( (twst != TW_START) && (twst != TW_REP_START)) {
		 i2c_stop();
		 return 1;
	}		 

	// send device address
	TWDR = address;
	TWCR = (1<<TWINT) | (1<<TWEN);

	// wail until transmission completed and ACK/NACK has been received
	while(!(TWCR & (1<<TWINT)) && --tmout) { _delay_us(1); }
	if ( !tmout ) {
		kputs("I2C Timeout\n");
		return 1;
	}

	// check value of TWI Status Register. Mask prescaler bits.
	twst = TW_STATUS & 0xF8;
	if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) {
		i2c_stop();
		return 1;
	}

	return 0;

}/* i2c_start */


/*************************************************************************
 Issues a repeated start condition and sends address and transfer direction 

 Input:   address and transfer direction of I2C device
 
 Return:  0 device accessible
          1 failed to access device
*************************************************************************/
unsigned char i2c_rep_start(unsigned char address)
{
    return i2c_start( address );

}/* i2c_rep_start */


/*************************************************************************
 Terminates the data transfer and releases the I2C bus
*************************************************************************/
void i2c_stop(void)
{
    /* send stop condition */
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	
	// assume stop condition finishes soon
	_delay_us(10);
	
	TWCR = 0; // reset TWI hardware
	
	// wait until stop condition is executed and bus released or there is an error
	/*while(TWCR & (1<<TWSTO))  {
		if ( TWSR & 0xF8 == 0x38 ) {
			kputs("Bus collision!\n");
			break;
		}
	}*/
}/* i2c_stop */


/*************************************************************************
  Send one byte to I2C device
  
  Input:    byte to be transfered
  Return:   0 write successful 
            1 write failed
*************************************************************************/
unsigned char i2c_write( unsigned char data )
{	
	uint16_t tmout = I2C_TIMEOUT;
    uint8_t   twst;
    
	// send data to the previously addressed device
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);

	// wait until transmission completed
	while(!(TWCR & (1<<TWINT)) && --tmout) {_delay_us(1); }
	if (!tmout) {
		kputs("I2C Timeout\n");
		return 1;
	}

	// check value of TWI Status Register. Mask prescaler bits
	twst = TW_STATUS & 0xF8;
	if( twst != TW_MT_DATA_ACK) return 1;
	return 0;

}/* i2c_write */


/*************************************************************************
 Read one byte from the I2C device, request more data from device 
 
 Return:  byte read from I2C device
*************************************************************************/
unsigned char i2c_readAck(void)
{
	uint16_t tmout = I2C_TIMEOUT;
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	while(!(TWCR & (1<<TWINT)) && --tmout){ _delay_us(1); }
	if ( !tmout ) {
		kputs("I2C Timeout\n");
		return 1;
	}

    return TWDR;

}/* i2c_readAck */


/*************************************************************************
 Read one byte from the I2C device, read is followed by a stop condition 
 
 Return:  byte read from I2C device
*************************************************************************/
unsigned char i2c_readNak(void)
{
	uint16_t tmout = I2C_TIMEOUT;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)) && --tmout) {_delay_us(1);}
	if (!tmout) {
		kputs("I2C Timeout\n");
		return 0xFF;
	}
	
    return TWDR;

}/* i2c_readNak */

unsigned char i2c_readbytes(unsigned char addr, unsigned char nBytes, unsigned char * buf) {
	unsigned char c = 0;
	if ( i2c_start(addr + I2C_READ) ) return 1;
	
	while ( nBytes-- > 0) {
		if ( nBytes == 0 )
		buf[c++] = i2c_readNak();
		else
		buf[c++] = i2c_readAck();
	}
	i2c_stop();
	return 0;	
}

unsigned char i2c_readreg(unsigned char addr, unsigned char reg, unsigned char nBytes, unsigned char * buf) {
	unsigned char c = 0;
	if ( i2c_start(addr + I2C_WRITE) ) return 1;
	i2c_write(reg);
	i2c_rep_start(addr + I2C_READ);
	
	while ( nBytes-- > 0) {
		if ( nBytes == 0 )
			buf[c++] = i2c_readNak();
		else
			buf[c++] = i2c_readAck();
	}
	i2c_stop();
	return 0;
}


unsigned char i2c_writereg(unsigned char addr, unsigned char reg, unsigned char nBytes, unsigned char * buf) {
	unsigned char c = 0;
	//kputs("i2c_write\n");
	//i2c_start_wait(addr + I2C_WRITE);
	if ( i2c_start(addr + I2C_WRITE) ) {
		//kputs("fail\n");
		return 1;
	}		
	//kputs("done\n");
	i2c_write(reg);
	while ( nBytes-- > 0 ) {
		i2c_write(buf[c++]);		
	}
	i2c_stop();
	return 0;	
}