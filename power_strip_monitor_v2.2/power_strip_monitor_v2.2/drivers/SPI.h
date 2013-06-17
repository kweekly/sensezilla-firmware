/*
 * SPI.h
 *
 * Created: 5/15/2013 5:16:47 PM
 *  Author: kweekly
 */ 


#ifndef SPI_H_
#define SPI_H_

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

#define SPI_CLKDIV2 0x80
#define SPI_CLKDIV4 0x00
#define SPI_CLKDIV8 0x81
#define SPI_CLKDIV16 0x01
#define SPI_CLKDIV32 0x82
#define SPI_CLKDIV64 0x02
#define SPI_CLKDIV128 0x03

#define SPI_LSBFIRST 0x20
#define SPI_MSBFIRST 0x00

void spi_init(uint8_t mode);
uint8_t spi_transfer(uint8_t data);



#endif /* SPI_H_ */