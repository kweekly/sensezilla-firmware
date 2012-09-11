#include <XBee.h>
#include <SPI.h>
#include "CS5467.h"

int nINT1 = 14;
int nINT2 = 15;

int CS1 = 17;
int CS2 = 18;

int XB_RESET = 2;

int LED1 = 10;
int LED2 = 9;

XBee xbee = XBee();
CS5467 ic1 = CS5467(CS1);
CS5467 ic2 = CS5467(CS2);


#define PACKET_SIZE (4 + 12*4)
uint8_t datapacket[PACKET_SIZE];
XBeeAddress64 destaddr = XBeeAddress64(0x00000000, 0x00000000);

uint32_t time;

void setup() {
  
  xbee.begin(9600);
//  Serial.begin(9600);
  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(nINT1, INPUT);
  pinMode(nINT2, INPUT);
  
 // Serial.println("Initialize IC1");
  ic1.init();
 // Serial.println("Initialize IC2");
  ic2.init();
  
  time = 0;
}

void printLong(long regval) {
  Serial.print("The data: ");
  Serial.print((regval>>16)&0xFF,BIN);
  Serial.print(' ');
  Serial.print((regval>>8)&0xFF,BIN);
  Serial.print(' ');
  Serial.println((regval)&0xFF,BIN);
}


void makePacket() {
  char i;
  uint32_t* dpack = (uint32_t *)datapacket;
  
  dpack[0] = time;
  for (i=1;i<12;i++)
    dpack[i] = random(0xFFFFFFFF);
}

char ledon = 0;

void loop() {
  /*Serial.print("Forcing DC offset...");
  ic1.writereg( V1_OFF_REG, 10 );
  Serial.println(ic1.readreg( V1_OFF_REG ));
  
  Serial.print("Perform Calibration...");
  ic1.calibrateDCOffset(CAL_CHANNEL_V1);
  Serial.println(ic1.readreg( V1_OFF_REG ));*/
  makePacket();
  ZBTxRequest zbTx = ZBTxRequest( destaddr, datapacket, sizeof(datapacket) );
  xbee.send(zbTx);  
  delay(2000);  
  if (ledon)
    digitalWrite(LED1,LOW);
  else
    digitalWrite(LED1,HIGH);
   
  ledon = !ledon;
  time += 2;
}
