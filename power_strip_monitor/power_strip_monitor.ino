
#undef SERIAL_OUT


#ifndef SERIAL_OUT
#include <XBee.h>
#endif

#include <SPI.h>
#include "CS5467.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <EEPROM.h>

int nINT1 = 14;
int nINT2 = 15;

int CS1 = 17;
int CS2 = 18;

int XB_RESET = 2;

int LED1 = 10;
int LED2 = 9;

#ifndef SERIAL_OUT
XBee xbee = XBee();
#endif

CS5467 ic1 = CS5467(CS1);
CS5467 ic2 = CS5467(CS2);


#define PACKET_SIZE (4 + 12*4)
uint8_t datapacket[PACKET_SIZE];
#ifndef SERIAL_OUT
XBeeAddress64 destaddr = XBeeAddress64(0x00000000, 0x0000FFFF);
#endif

uint32_t time;

char send_pack;


// by default, max current is 20A -> 28.28mVpp, so gain should be 3.5360678925 / sqrt(2) = 2.50037758553
// Calculated Values
/*
#define DEFAULT_I_GAIN 10487344
#define DEFAULT_I_NORM 20.0
#define DEFAULT_I_OFFSET 8
*/

// Empirical Values
#define DEFAULT_I_GAIN 10676955 // 1.8 * 2 / sqrt(2) = 2.54558441227
#define DEFAULT_I_NORM 20.0
#define DEFAULT_I_OFFSET 0

// WITH 500 OHM R21
// by default, max Voltage is 250V -> 187.9mVpp ( out of 500mVpp ), gain should be 2.66098988824 / sqrt(2) = 1.88160399464
// Calculated Values

#define DEFAULT_V_GAIN 7892019 
#define DEFAULT_V_NORM 250.0
#define DEFAULT_V_OFFSET 0

/*
#define DEFAULT_V_GAIN 4448731 // 1.5 / sqrt(2) = 1.06066017178
#define DEFAULT_V_NORM 150.0
#define DEFAULT_V_OFFSET 0
*/

#define EE_BYTE_0 0xDE
#define EE_BYTE_1 0xF1

void eesave24(char addr, uint32_t val) {
  char i = 0;
  for (i = 0; i < 3; i++ ) {
     EEPROM.write(addr + i,(val >> (i*8))&0xFF);
  }
}

void eesavefloat(char addr, double val) {
  char * bs = (char*)&val;
  char i;
  for (i =0; i < 4; i++) {
     EEPROM.write(addr + i, bs[i]); 
  }
}

uint32_t eeload24(char addr) {
  char i = 0;
  uint32_t rval = 0;
  for (i = 0; i < 3; i++ ) {
     rval |= (uint32_t)EEPROM.read(addr + i) << (i*8);
  }
}

double eeloadfloat(char addr) {
  char rbs[4];
  for (char i = 0; i < 4; i++ ) {
     rbs[i] = EEPROM.read(addr + i); 
  }
  return *(double *)rbs;
}

uint32_t iGain;
double iNorm;
uint32_t iOffset;

uint32_t vGain;
double vNorm;
uint32_t vOffset;

void setup() {
  
  
  #ifdef SERIAL_OUT
    Serial.begin(9600);
    delay(500);
  #else
    xbee.begin(9600);
  #endif
  
  // check if EEPROM Initialized
  char off = 2;
  if ( !(EEPROM.read(0) == EE_BYTE_0 && EEPROM.read(1) == EE_BYTE_1 ) ) {
      eesave24(off,DEFAULT_I_GAIN);
      off += 3;
      eesavefloat(off,DEFAULT_I_NORM);
      off += 4;
      eesave24(off,DEFAULT_I_OFFSET);
      off += 3;
      eesave24(off,DEFAULT_V_GAIN);
      off += 3;
      eesavefloat(off,DEFAULT_V_NORM);
      off += 4;
      eesave24(off,DEFAULT_V_OFFSET);
      off += 3;
      EEPROM.write(0,EE_BYTE_0);
      EEPROM.write(1,EE_BYTE_1);
  }
  
  off = 2;
  iGain = eeload24(off);
  off += 3;
  iNorm = eeloadfloat(off);
  off += 4;
  iOffset = eeload24(off);
  off += 3;
  vGain = eeload24(off);
  off += 3;
  vNorm = eeloadfloat(off);
  off += 4;
  vOffset = eeload24(off);
  off += 3;
 
  #ifdef SERIAL_OUT
  Serial.println("Parameters:");
  Serial.print("iGain : 0b");Serial.println(iGain,BIN);
  Serial.print("iNorm : ");Serial.println(iNorm,10);
  Serial.print("iOffset : 0b");Serial.println(iOffset,BIN);
  Serial.print("vGain : 0b");Serial.println(vGain,BIN);
  Serial.print("vNorm : ");Serial.println(vNorm,10);
  Serial.print("vOffset : 0b");Serial.println(vOffset,BIN);  
  #endif
 
  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(nINT1, INPUT);
  pinMode(nINT2, INPUT);
  pinMode(XB_RESET, OUTPUT);
  digitalWrite(XB_RESET, HIGH);
  
    
  #ifdef SERIAL_OUT
  Serial.println("Initialize IC2");
  #endif
  ic2.init();
  ic2.writereg(MODES_REG,0x004001E1L); // HPF enabled, line frequency meas. enabled
  ic2.writereg(I1_GAIN_REG,iGain);
  ic2.writereg(I1_OFF_REG, iOffset);
  ic2.writereg(V1_GAIN_REG,vGain);
  ic2.writereg(V1_OFF_REG,vOffset);  
  ic2.writereg(I2_GAIN_REG,iGain);
  ic2.writereg(I2_OFF_REG, iOffset);
  ic2.writereg(V2_GAIN_REG,vGain);
  ic2.writereg(V2_OFF_REG,vOffset);  
  ic2.writereg(P1_OFF_REG,0);
  ic2.writereg(P2_OFF_REG,0);
  ic2.startConversion(1);
  
  #ifdef SERIAL_OUT
  Serial.println("Initialize IC1");
  #endif
  ic1.init();
  ic1.writereg(MODES_REG,0x004001E1L); // HPF enabled, line frequency meas. enabled
  ic1.writereg(I1_GAIN_REG,iGain);
  ic1.writereg(I1_OFF_REG, iOffset);
  ic1.writereg(V1_GAIN_REG,vGain);
  ic1.writereg(V1_OFF_REG,vOffset);
  ic1.writereg(I2_GAIN_REG,iGain);
  ic1.writereg(I2_OFF_REG, iOffset);
  ic1.writereg(V2_GAIN_REG,vGain);
  ic1.writereg(V2_OFF_REG,vOffset);    
  ic1.writereg(P1_OFF_REG,0);
  ic1.writereg(P2_OFF_REG,0);
  ic1.startConversion(1);

  
  #ifdef SERIAL_OUT
  Serial.println("Initialize Timer");
  #endif
  // Using Timer 1 for clock
  // clk = 4096000
  // clk/1024 = 4000
  // CTC mode, period = 4000
  noInterrupts();
    TCCR1A = 0;     // set entire TCCR1A register to 0
    TCCR1B = 0;     // same for TCCR1B
 
    // set compare match register to desired timer count:
    OCR1A = 3999;
    // turn on CTC mode:
    TCCR1B |= (1 << WGM12);
    // Set CS10 and CS12 bits for 1024 prescaler:
    TCCR1B |= (1 << CS10);
    TCCR1B |= (1 << CS12);
    // enable timer compare interrupt:
    TIMSK1 |= (1 << OCIE1A);
  interrupts();
  
  time = 0;
  send_pack = 0;
  #ifdef SERIAL_OUT
  Serial.println("Done with initialization routines");
  #endif
}

uint32_t true_power[4];
uint32_t RMS_voltage[4];
uint32_t RMS_current[4];

char phy_to_log_map[] = {2, 3, 0, 1};

ISR(TIMER1_COMPA_vect)
{
    time += 1;
    digitalWrite(LED2, !digitalRead(LED2));
    digitalWrite(LED1, HIGH);
    true_power[phy_to_log_map[0]] = ic1.readreg(P1_AVG_REG);
    true_power[phy_to_log_map[1]] = ic1.readreg(P2_AVG_REG);  
    
    RMS_voltage[phy_to_log_map[0]] = ic1.readreg(V1_RMS_REG);
    RMS_voltage[phy_to_log_map[1]] = ic1.readreg(V2_RMS_REG); 
    RMS_current[phy_to_log_map[0]] = ic1.readreg(I1_RMS_REG);
    RMS_current[phy_to_log_map[1]] = ic1.readreg(I2_RMS_REG); 
    /*
    RMS_voltage[0] = ic1.readreg(V1_REG);
    RMS_voltage[1] = ic1.readreg(V2_REG); 
    RMS_current[0] = ic1.readreg(I1_REG);
    RMS_current[1] = ic1.readreg(I2_REG); 
    */
    true_power[phy_to_log_map[2]] = ic2.readreg(P1_AVG_REG);
    true_power[phy_to_log_map[3]] = ic2.readreg(P2_AVG_REG);   
    
    RMS_voltage[phy_to_log_map[2]] = ic2.readreg(V1_RMS_REG);
    RMS_voltage[phy_to_log_map[3]] = ic2.readreg(V2_RMS_REG);     
    RMS_current[phy_to_log_map[2]] = ic2.readreg(I1_RMS_REG);
    RMS_current[phy_to_log_map[3]] = ic2.readreg(I2_RMS_REG); 
    
    /*
    RMS_voltage[2] = ic2.readreg(V1_REG);
    RMS_voltage[3] = ic2.readreg(V2_REG);     
    RMS_current[2] = ic2.readreg(I1_REG);
    RMS_current[3] = ic2.readreg(I2_REG); 
    */
    digitalWrite(LED1, LOW);
    
    send_pack = 1;
}

float itof(uint32_t reg, char sign_bit) {
  uint32_t rval = 0;
   char i = 0;

   if(sign_bit) {
     if ( reg & (uint32_t)0x00800000L ) 
       rval = 0x80000000L;
     reg <<= 1;
     reg &= 0xFFFFFFL;
   }
   if ( reg == 0 ) return 0;


   while ( !(reg & (uint32_t)0x00800000L) ) {
     reg <<= 1;
     i++;
   }
   i++;
   rval = rval | ((uint32_t)(127-i)<<(uint32_t)23) | (reg & 0x7FFFFFL);
   return *(float *)&rval;
}

void makePacket() {
  char i;
  float* dpack = (float *)datapacket;
  dpack[0] = *(float *)&time;
  char off = 1;
  for (i=0;i<4;i++) {
      dpack[off + i*3] = iNorm * vNorm * itof(true_power[i],1);
      dpack[off + i*3 + 1] = iNorm * itof(RMS_current[i],0);
      dpack[off + i*3 + 2] = vNorm * itof(RMS_voltage[i],0);
      #ifdef SERIAL_OUT
      Serial.print(i,DEC);
      Serial.print(": ");
      Serial.print(true_power[i],BIN);
      Serial.print(" ");
      Serial.print(dpack[off + i*3],10);
      Serial.print(" ");
      Serial.print(dpack[off + i*3 + 1],10);
      Serial.print(" ");
      Serial.println(dpack[off + i*3 + 2],10);
      #endif
  }
  #ifdef SERIAL_OUT
  Serial.println();
  #endif
}

char ledon = 0;

#ifndef SERIAL_OUT
Rx64Response xbrx = Rx64Response();
Tx64Request xbtx;
#endif

#define CMD_SYNC_TIME 0x01


void loop() {
   
   #ifndef SERIAL_OUT
   xbee.readPacket();

   if ( xbee.getResponse().isAvailable() ) {
       if ( xbee.getResponse().getApiId() == RX_64_RESPONSE) {
           xbee.getResponse().getRx64Response(xbrx);
           if ( xbrx.getData(0) == CMD_SYNC_TIME ) {
              for (char c = 0; c < 5; c++ ) {
                 datapacket[c] = xbrx.getData(c); 
              }
              destaddr = xbrx.getRemoteAddress64();
              time = *(uint32_t *)(datapacket + 1);
              xbtx = Tx64Request( destaddr, datapacket, 5);
              xbee.send(xbtx);
           }
       }
   }
   #endif

  if (send_pack) {
      makePacket();
      
      #ifndef SERIAL_OUT
        xbtx = Tx64Request( destaddr, datapacket, sizeof(datapacket) );
/*        datapacket[0] = 0xAA;
        xbtx = Tx64Request( destaddr, datapacket, 1 );*/
        xbee.send(xbtx);  
      #endif
   
      send_pack = 0;
   }

}
