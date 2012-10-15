#include "Arduino.h"
#include <SPI.h>
#include "CS5467.h"



CS5467::CS5467(int _pin) {
  CSpin = _pin;
  current_page = 255;
}

void CS5467::init() {
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  SPI.setDataMode(SPI_MODE0);

  digitalWrite(CSpin,HIGH);
  pinMode(CSpin,OUTPUT);
  
  writereg(CTRL_REG, (1L<<12L) | (1<<5) | 6);// gain set to +/-50mV  

  
  softwareReset();
  
  waitUntilReady();
  
}

void CS5467::softwareReset() {
  digitalWrite(CSpin,LOW);
  SPI.transfer(0b10000000);
  digitalWrite(CSpin,HIGH);
  
  waitUntilReady();
}

void CS5467::waitUntilReady() {
  long stat = readreg(STATUS_REG);
  while (!( stat & 0x800000 )) { 
    stat = readreg(STATUS_REG);
    //Serial.println(stat,BIN);
  }
}  


void CS5467::changePage(char address) {
   char page = (0xFF & (address>>5));
   if ( page != current_page ) {
      writereg( PAGE_REG, page ); 
      current_page = page;
   }
}

unsigned long CS5467::readreg(char address) {
    if( address != PAGE_REG ){
       changePage(address); 
    }
    unsigned long retval;
    digitalWrite(CSpin,LOW);
    SPI.transfer(((address&0x1F)<<1));
    retval = ((unsigned long)0xFF  & SPI.transfer(0xFF)) << (unsigned long)16L;
    retval |= ((unsigned long)0xFF  & SPI.transfer(0xFF)) << (unsigned long)8L;
    retval |= ((unsigned long)0xFF  & SPI.transfer(0xFF));
    digitalWrite(CSpin,HIGH);
    return retval;
    
}

void CS5467::writereg(char address, unsigned long data) {
    if( address != PAGE_REG ){
       changePage(address); 
    }

   digitalWrite(CSpin,LOW);
   SPI.transfer(((address&0x1F)<<1) | 0x40 );
   SPI.transfer((data>>16)&0xFF);
   SPI.transfer((data>>8)&0xFF);
   SPI.transfer(data&0xFF);
   digitalWrite(CSpin,HIGH);
} 

void CS5467::calibrateDCOffset( char channel ) {
   if ( channel == CAL_CHANNEL_I1 ) {
      writereg(I1_GAIN_REG,1); 
   } else if ( channel == CAL_CHANNEL_V1 ) {
      writereg(V1_GAIN_REG,1);     
   } else if ( channel == CAL_CHANNEL_I2 ) {
      writereg(I2_GAIN_REG,1);     
   } else if ( channel == CAL_CHANNEL_V2 ) {
      writereg(V2_GAIN_REG,1);     
   }
   
   digitalWrite(CSpin,LOW);
   SPI.transfer(0x80 | channel);
   digitalWrite(CSpin,HIGH);
   
   waitUntilReady();
}
void CS5467::calibrateACOffset( char channel ) {
   if ( channel == CAL_CHANNEL_I1 ) {
      writereg(I1_ACOFF_REG,0); 
   } else if ( channel == CAL_CHANNEL_V1 ) {
      writereg(V1_ACOFF_REG,0);     
   } else if ( channel == CAL_CHANNEL_I2 ) {
      writereg(I2_ACOFF_REG,0);     
   } else if ( channel == CAL_CHANNEL_V2 ) {
      writereg(V2_ACOFF_REG,0);     
   }
   
   digitalWrite(CSpin,LOW);
   SPI.transfer(0xA0 | channel);
   digitalWrite(CSpin,HIGH);
   
   waitUntilReady();  
}
     
void CS5467::calibrateDCGain( char channel ) { 
   digitalWrite(CSpin,LOW);
   SPI.transfer(0x90 | channel);
   digitalWrite(CSpin,HIGH);
   
   waitUntilReady(); 
}
void CS5467::calibrateACGain( char channel ) {
   if ( channel == CAL_CHANNEL_I1 ) {
      writereg(I1_GAIN_REG,1); 
   } else if ( channel == CAL_CHANNEL_V1 ) {
      writereg(V1_GAIN_REG,1);     
   } else if ( channel == CAL_CHANNEL_I2 ) {
      writereg(I2_GAIN_REG,1);     
   } else if ( channel == CAL_CHANNEL_V2 ) {
      writereg(V2_GAIN_REG,1);     
   }
   
   digitalWrite(CSpin,LOW);
   SPI.transfer(0xB0 | channel);
   digitalWrite(CSpin,HIGH);
   
   waitUntilReady();   
}

void CS5467::startConversion(char continuous) {
   digitalWrite(CSpin,LOW);
   SPI.transfer(0b11100000 | (continuous<<3));
   digitalWrite(CSpin,HIGH);
   
   waitUntilReady();
}

