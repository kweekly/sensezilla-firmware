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

void setup() {
  
  //xbee.begin(9600);
  Serial.begin(9600);
  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(nINT1, INPUT);
  pinMode(nINT2, INPUT);
  
  ic1.init();
  ic2.init();
  
}

void printLong(long regval) {
  Serial.print("The data: ");
  Serial.print((regval>>16)&0xFF,BIN);
  Serial.print(' ');
  Serial.print((regval>>8)&0xFF,BIN);
  Serial.print(' ');
  Serial.println((regval)&0xFF,BIN);
}


void loop() {

  digitalWrite(LED1,HIGH);
  digitalWrite(LED2,LOW);

  printLong(ic1.readreg(STATUS_REG));

  delay(1000);
  digitalWrite(LED1,LOW);
  digitalWrite(LED2,HIGH);

  printLong(ic2.readreg(STATUS_REG));

  delay(1000);
}
