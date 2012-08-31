#include "Arduino.h"
#include <SPI.h>
#include "CS5467.h"



CS5467::CS5467(int _pin) {
  CSpin = _pin;
}

void CS5467::init() {
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  SPI.setDataMode(SPI_MODE0);

  digitalWrite(CSpin,HIGH);
  pinMode(CSpin,OUTPUT);
  
  writereg(CTRL_REG, 3);  
}


long CS5467::readreg(char address) {
    long retval;
    digitalWrite(CSpin,LOW);
    SPI.transfer(((address&0x1F)<<1));
    retval = SPI.transfer(0xFF) << 16;
    retval |= (SPI.transfer(0xFF) << 8);
    retval |= SPI.transfer(0xFF);
    digitalWrite(CSpin,HIGH);
    return retval;
    
}

void CS5467::writereg(char address, long data) {
   digitalWrite(CSpin,LOW);
   SPI.transfer(((address&0x1F)<<1) | 0x40 );
   SPI.transfer((data>>16)&0xFF);
   SPI.transfer((data>>8)&0xFF);
   SPI.transfer(data&0xFF);
   digitalWrite(CSpin,HIGH);
} 
