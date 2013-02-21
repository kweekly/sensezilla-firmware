
#define SERIAL_OUT
#undef DUST_OUT
#define XBEE_OUT


#ifdef XBEE_OUT
#include <XBee.h>
#endif

#include <SPI.h>
#include "CS5467.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <EEPROM.h>


#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define cbiv(...) cbi(__VA_ARGS__)
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#define sbiv(...) sbi(__VA_ARGS__)
#define rbi(sfr, bit) ((_SFR_BYTE(sfr) & _BV(bit))!=0)
#define rbiv(...) rbi(__VA_ARGS__)



#define MOTE_RADIO_INHIBIT PORTA,7
#define MOTE_SLEEPN PORTA,6
#define MOTE_TIMEN PORTA,5
#define CS1 PORTA,4
#define CS2 PORTA,3
#define CS3 PORTA,2
#define CS1a 28
#define CS2a 27
#define CS3a 26
#define LED2 PORTA,1
#define LED1 PORTA,0

#define EXP_SCK PORTB,7
#define EXP_MISO PORTB,6
#define EXP_MOSI PORTB,5
#define GATE5 PORTB,4
#define GATE4 PORTB,3
#define GATE3 PORTB,2
#define GATE2 PORTB,1
#define GATE1 PORTB,0
#define GATE5d DDRB,4
#define GATE4d DDRB,3
#define GATE3d DDRB,2
#define GATE2d DDRB,1
#define GATE1d DDRB,0

#define GATE6 PORTC,7
#define GATE6d DDRC,7
#define CS_EXP PORTC,6
#define MOTE_RESETN PORTC,1
#define MOTE_RX_CTSN PORTC,0

#define XIN PORTD,7
#define MOTE_RX_RTSN PORTD,6
#define MOTE_TX_CTSN PORTD,5
#define MOTE_TX_RTSN PORTD,4
#define ATMEGA_TO_MOTE PORTD,3
#define MOTE_TO_ATMEGA PORTD,2
#define ATMEGA_TO_FTDI PORTD,1
#define FTDI_TO_ATMEGA PORTD,0

#ifdef XBEE_OUT
XBee xbee = XBee();
#endif

#define NUM_IC 3
CS5467 ics[] = {CS5467(CS1a), CS5467(CS2a),  CS5467(CS3a)};



#define PACKET_SIZE (4 + 3*2*NUM_IC*4)
uint8_t datapacket[PACKET_SIZE];

#ifdef XBEE_OUT
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
  
  sbi(DDRA,0);
  #ifdef SERIAL_OUT
    Serial.begin(115200);
    delay(500);
    Serial.println("Serial begin.");
  #endif
  
  #if defined XBEE_OUT 
    xbee.setSerial(Serial1);
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

 
  // Set up port directions and initial values
  PORTA = 0b01111101; // all active-low signals are high, LED 0 goes on
  DDRA = 0xFF; // all outputs
  PORTB = 0x1F; // all gates on
  DDRB = 0x1F; // all gates output 
  PORTC = 0b11000010; // gate6 on, cs_Exp on, mote_reset disabled
  DDRC  = 0b10000010; 
  PORTD = 0b01100000; 
  DDRD  = 0b11101010;
  
  // turn on clocks for ICs. CLK/4 on OC2A
  // Compare mode, Toggle OC2A on compare match
  TCCR2A = 0;
  TCCR2B = 0;
  OCR2A = 1;
  
  // Toggle on compare match
  TCCR2A |= (1<<COM2A0);
  //turn on CTC mode
  TCCR2A |= (1<< WGM21);
  // no prescaler
  TCCR2B |= (1<<CS20);
  
  
   
  for ( int c = 0; c < NUM_IC; c++ ) {
      #ifdef SERIAL_OUT
      Serial.print("Initialize IC");
      Serial.println(c);
      #endif
      ics[c].init();
      ics[c].writereg(MODES_REG,0x004001E1L); // HPF enabled, line frequency meas. enabled
      ics[c].writereg(I1_GAIN_REG,iGain);
      ics[c].writereg(I1_OFF_REG, iOffset);
      ics[c].writereg(V1_GAIN_REG,vGain);
      ics[c].writereg(V1_OFF_REG,vOffset);  
      ics[c].writereg(I2_GAIN_REG,iGain);
      ics[c].writereg(I2_OFF_REG, iOffset);
      ics[c].writereg(V2_GAIN_REG,vGain);
      ics[c].writereg(V2_OFF_REG,vOffset);  
      ics[c].writereg(P1_OFF_REG,0);
      ics[c].writereg(P2_OFF_REG,0);
      ics[c].startConversion(1);
  }

  
  #ifdef SERIAL_OUT
  Serial.println("Initialize Timer");
  #endif
  // Using Timer 1 for clock
  // clk = 16384000
  // clk/1024 = 16000
  // CTC mode, period = 16000
  noInterrupts();
    TCCR1A = 0;     // set entire TCCR1A register to 0
    TCCR1B = 0;     // same for TCCR1B
 
    // set compare match register to desired timer count:
    OCR1A = 15999;
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

uint32_t true_power[NUM_IC*2];
uint32_t RMS_voltage[NUM_IC*2];
uint32_t RMS_current[NUM_IC*2];

ISR(TIMER1_COMPA_vect)
{
    time += 1;
    if (rbiv(LED2))
      cbiv(LED2);
    else
      sbiv(LED2);
      
    sbiv(LED1);
    
    for ( int c = 0; c < NUM_IC; c++ ) {
      true_power[c*2] = ics[c].readreg(P1_AVG_REG);
      true_power[c*2+1] = ics[c].readreg(P2_AVG_REG);  
      
      RMS_voltage[c*2] = ics[c].readreg(V1_RMS_REG);
      RMS_voltage[c*2+1] = ics[c].readreg(V2_RMS_REG); 
      
      RMS_current[c*2] = ics[c].readreg(I1_RMS_REG);
      RMS_current[c*2+1] = ics[c].readreg(I2_RMS_REG); 

    }

    cbiv(LED1);  
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
  for (i=0;i<NUM_IC*2;i++) {
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

#ifdef XBEE_OUT
Rx64Response xbrx = Rx64Response();
Tx64Request xbtx;
#endif

#define CMD_SYNC_TIME   0x01
#define CMD_CNTL_POWER  0x02

void loop() {
   
   #ifdef XBEE_OUT
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
              #ifdef SERIAL_OUT
              Serial.print("Timesynch recieved, time is: ");
              Serial.println(time,DEC);
              #endif
              xbtx = Tx64Request( destaddr, datapacket, 5);
              xbee.send(xbtx);
           }
           else if ( xbrx.getData(0) == CMD_CNTL_POWER ) {
              char cntl = xbrx.getData(1);
              #ifdef SERIAL_OUT
              Serial.print("Cntl recieved, Bits are: ");
              Serial.println(cntl,BIN);
              #endif
              DDRB = cntl & 0x1F;
              if ( cntl & 0x20 ) 
                 DDRC |= 0x80; 
              else
                 DDRC &= ~0x80;
           }
       }
   }
   #endif

  if (send_pack) {
      makePacket();
      
      #ifdef XBEE_OUT
        xbtx = Tx64Request( destaddr, datapacket, sizeof(datapacket) );
        xbee.send(xbtx);  
      #endif
   
      send_pack = 0;
   }

}
