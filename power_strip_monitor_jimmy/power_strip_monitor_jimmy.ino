//#include <XBee.h>
#include <SPI.h>
#include "CS5467.h"


int nINT1 = 14;
int nINT2 = 15;

int CS1 = 17;
int CS2 = 18;

int XB_RESET = 2;

int LED1 = 10;
int LED2 = 9;

int calibrationPhase = 0;

//XBee xbee = XBee();
CS5467 ic1 = CS5467(CS1);
CS5467 ic2 = CS5467(CS2);

void setup() {
  
  //xbee.begin(9600);
  Serial.begin(9600);
  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(nINT1, INPUT);
  pinMode(nINT2, INPUT);
  
  Serial.println("Initialize IC1");
  ic1.init();
  Serial.println("Initialize IC2");
  ic2.init();
}

void waitSerial() {
  char status = 0;
  while(!status) {
    status = Serial.available();
  }
  Serial.read();
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
  Serial.println("Software Reset Begin");
  ic1.softwareReset();
  ic1.waitUntilReady();
  ic2.softwareReset();
  ic2.waitUntilReady();
  Serial.println("Software Reset Complete");

  Serial.println("Perform DC Offset Calibration...");
  ic1.calibrateDCOffset(CAL_CHANNEL_I1);
  ic1.waitUntilReady();
  ic1.calibrateDCOffset(CAL_CHANNEL_V1);
  ic1.waitUntilReady();
  ic1.calibrateDCOffset(CAL_CHANNEL_I2);
  ic1.waitUntilReady();
  ic1.calibrateDCOffset(CAL_CHANNEL_V2);
  ic1.waitUntilReady();
  ic2.calibrateDCOffset(CAL_CHANNEL_I1);
  ic2.waitUntilReady();
  ic2.calibrateDCOffset(CAL_CHANNEL_V1);
  ic2.waitUntilReady();
  ic2.calibrateDCOffset(CAL_CHANNEL_I2);
  ic2.waitUntilReady();
  ic2.calibrateDCOffset(CAL_CHANNEL_V2);
  ic2.waitUntilReady();

  Serial.println("Chip 1...");
  Serial.print("DC Offset I1: ");
  Serial.println(ic1.readreg( I1_OFF_REG ));
  Serial.print("DC Offset V1: ");
  Serial.println(ic1.readreg( V1_OFF_REG ));
  Serial.print("DC Offset I2: ");
  Serial.println(ic1.readreg( I2_OFF_REG ));
  Serial.print("DC Offset V2: ");
  Serial.println(ic1.readreg( V2_OFF_REG ));
  delay(2000);
  Serial.println("Chip 2...");
  Serial.print("DC Offset I1: ");
  Serial.println(ic1.readreg( I1_OFF_REG ));
  Serial.print("DC Offset V1: ");
  Serial.println(ic1.readreg( V1_OFF_REG ));
  Serial.print("DC Offset I2: ");
  Serial.println(ic1.readreg( I2_OFF_REG ));
  Serial.print("DC Offset V2: ");
  Serial.println(ic1.readreg( V2_OFF_REG ));
  delay(2000);
  
  waitSerial();
  
  Serial.println("Perform AC Gain Calibration...");
  ic1.highPassFilters(CAL_CHANNEL_I1, 1);
  ic1.highPassFilters(CAL_CHANNEL_V1, 1);
  ic1.highPassFilters(CAL_CHANNEL_I2, 1);
  ic1.highPassFilters(CAL_CHANNEL_V2, 1);
  ic2.highPassFilters(CAL_CHANNEL_I1, 1);
  ic2.highPassFilters(CAL_CHANNEL_V1, 1);
  ic2.highPassFilters(CAL_CHANNEL_I2, 1);
  ic2.highPassFilters(CAL_CHANNEL_V2, 1);
  ic1.calibrateACGain(CAL_CHANNEL_I1);
  ic1.waitUntilReady();
  ic1.calibrateACGain(CAL_CHANNEL_V1);
  ic1.waitUntilReady();
  ic1.calibrateACGain(CAL_CHANNEL_I2);
  ic1.waitUntilReady();
  ic1.calibrateACGain(CAL_CHANNEL_V2);
  ic1.waitUntilReady();
  ic2.calibrateACGain(CAL_CHANNEL_I1);
  ic2.waitUntilReady();
  ic2.calibrateACGain(CAL_CHANNEL_V1);
  ic2.waitUntilReady();
  ic2.calibrateACGain(CAL_CHANNEL_I2);
  ic2.waitUntilReady();
  ic2.calibrateACGain(CAL_CHANNEL_V2);
  ic2.waitUntilReady();
  
  Serial.println("Chip 1...");
  Serial.print("AC Gain I1: ");
  Serial.println(ic1.readreg( I1_GAIN_REG ));
  Serial.print("AC Gain V1: ");
  Serial.println(ic1.readreg( V1_GAIN_REG ));
  Serial.print("AC Gain I2: ");
  Serial.println(ic1.readreg( I2_GAIN_REG ));
  Serial.print("AC Gain V2: ");
  Serial.println(ic1.readreg( V2_GAIN_REG ));
  delay(2000);
  Serial.println("Chip 2...");
  Serial.print("AC Gain I1: ");
  Serial.println(ic1.readreg( I1_GAIN_REG ));
  Serial.print("AC Gain V1: ");
  Serial.println(ic1.readreg( V1_GAIN_REG ));
  Serial.print("AC Gain I2: ");
  Serial.println(ic1.readreg( I2_GAIN_REG ));
  Serial.print("AC Gain V2: ");
  Serial.println(ic1.readreg( V2_GAIN_REG ));
  delay(2000);
  
  waitSerial();
  
  Serial.println("Perform AC Offset Calibration...");
  ic1.calibrateACOffset(CAL_CHANNEL_I1);
  ic1.waitUntilReady();
  ic1.calibrateACOffset(CAL_CHANNEL_V1);
  ic1.waitUntilReady();
  ic1.calibrateACOffset(CAL_CHANNEL_I2);
  ic1.waitUntilReady();
  ic1.calibrateACOffset(CAL_CHANNEL_V2);
  ic1.waitUntilReady();
  ic2.calibrateACOffset(CAL_CHANNEL_I1);
  ic2.waitUntilReady();
  ic2.calibrateACOffset(CAL_CHANNEL_V1);
  ic2.waitUntilReady();
  ic2.calibrateACOffset(CAL_CHANNEL_I2);
  ic2.waitUntilReady();
  ic2.calibrateACOffset(CAL_CHANNEL_V2);
  ic2.waitUntilReady();

  Serial.println("Chip 1...");
  Serial.print("AC Offset I1: ");
  Serial.println(ic1.readreg( I1_ACOFF_REG ));
  Serial.print("AC Offset V1: ");
  Serial.println(ic1.readreg( V1_ACOFF_REG ));
  Serial.print("AC Offset I2: ");
  Serial.println(ic1.readreg( I2_ACOFF_REG ));
  Serial.print("AC Offset V2: ");
  Serial.println(ic1.readreg( V2_ACOFF_REG ));
  delay(2000);
  Serial.println("Chip 2...");
  Serial.print("AC Offset I1: ");
  Serial.println(ic1.readreg( I1_ACOFF_REG ));
  Serial.print("AC Offset V1: ");
  Serial.println(ic1.readreg( V1_ACOFF_REG ));
  Serial.print("AC Offset I2: ");
  Serial.println(ic1.readreg( I2_ACOFF_REG ));
  Serial.print("AC Offset V2: ");
  Serial.println(ic1.readreg( V2_ACOFF_REG ));
  delay(2000);

  waitSerial();
}
