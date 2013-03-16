#include <XBee.h>

// this constant won't change:
#define SERIAL_OUT
#define LOGGER_MODE

#ifndef SERIAL_OUT
XBee xbee = XBee();
#endif

#define PACKET_SIZE (4 + 2*4)
uint8_t datapacket[PACKET_SIZE];
#ifndef SERIAL_OUT 
XBeeAddress64 destaddr = XBeeAddress64(0x00000000, 0x0000FFFF);
#endif



unsigned int sample_period = 1;
unsigned int sample_countdown;

uint32_t time;

const int resetPin = 2; // xbee reset pin
const int pulsePin = 3;    // pin that the interrupt is attached to


unsigned long pHighTime = 0;
unsigned long pLowTime = 0;
unsigned long pRef = 0;

char falling = 1;
unsigned long falling_ts = 0;
unsigned long rising_ts = 0;

char pReady = 0;

#define LOGGER_LEN 10
unsigned long loggerbuf[LOGGER_LEN];
#define TYPE_FALL 2
#define TYPE_RISE 1
#define TYPE_NONE 0
unsigned char logger_types[LOGGER_LEN];
char log_start = 0;
char log_end = 0;

ISR(TIMER1_COMPA_vect)
{
    time += 1;
    if ( --sample_countdown == 0 ) {
       // reset counters
       if (falling) { // currently HIGH
           pHighTime += millis() - rising_ts;
           rising_ts = millis();
       } else {
          pLowTime += millis() - falling_ts;
          falling_ts = millis(); 
       }
       pReady = 1;
       sample_countdown = sample_period;
    }
}

void pulseISR() {
   if (falling) {
     #ifdef LOGGER_MODE
     loggerbuf[log_end] = millis();
     logger_types[log_end] = TYPE_FALL;
     log_end = (log_end + 1) % LOGGER_LEN; 
     #else
     falling_ts = millis();
     pHighTime += falling_ts - rising_ts;
     #endif
     attachInterrupt(1, pulseISR, RISING);
   } else {
     #ifdef LOGGER_MODE
     loggerbuf[log_end] = millis();
     logger_types[log_end] = TYPE_RISE;
     log_end = (log_end + 1) % LOGGER_LEN;      
     #else
     rising_ts = millis();
     pLowTime += rising_ts - falling_ts;
     #endif
     attachInterrupt(1, pulseISR, FALLING);
   }
   falling = !falling;
}

void setup() {
  log_start = log_end = 0;
  for ( int c = 0; c < LOGGER_LEN; c++ ) {
     logger_types[c] =  TYPE_NONE;
  }  
  
  // initialize the pulse pin as a input:
  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin,HIGH);
  
  pinMode(pulsePin, INPUT);
  if (digitalRead(pulsePin) == HIGH) {
    attachInterrupt(1, pulseISR, FALLING);
    falling = 1;
  }
  else {
    attachInterrupt(1, pulseISR, RISING);
    falling = 0;
  }
  pReady = 0;
  
  // initialize serial communication:
  #ifdef SERIAL_OUT
    Serial.begin(38400,SERIAL_8N2);
    delay(2000);
  #else
    xbee.begin(9600);
  #endif
 
  #ifndef LOGGER_MODE
    // Using Timer 1 for clock
  // clk = 8000000
  // clk/256 = 31250
  // CTC mode, period = 31250
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B
 
  // set compare match register to desired timer count:
  OCR1A = 31249;
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // Set CS10 bit for 256 prescaler:
  TCCR1B |= (1 << CS12);
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);
  #endif
  
  sample_countdown = 5;
  
  interrupts();
  
  #ifdef LOGGER_MODE
  for (int c = 3; c > 0; c--) {
    Serial.print("Logger mode starting, beginning in ");
    Serial.println(c,DEC);
    delay(1000); 
  }
  #endif
}

void makePacket() {
  uint32_t *dpack = (uint32_t *)datapacket;
  dpack[0] = time;
  dpack[1] = pLowTime;
  dpack[2] = pHighTime;  
}


#ifndef SERIAL_OUT
Rx64Response xbrx = Rx64Response();
Tx64Request xbtx;
#endif

#define CMD_SYNC_TIME 0x01


void loop() {
   #ifdef LOGGER_MODE
   if (log_start != log_end) {
     Serial.print("t=");
     Serial.print(loggerbuf[log_start],DEC);
     if ( logger_types[log_start] == TYPE_FALL ) {
       Serial.print(" FALLING");
     } else if ( logger_types[log_start] == TYPE_RISE ) {
       Serial.print(" RISING"); 
     }
     log_start = (log_start + 1)%LOGGER_LEN;
     Serial.println();
     delay(100);
   }
   #else
   #ifndef SERIAL_OUT
   xbee.readPacket();

   if ( xbee.getResponse().isAvailable() ) {
       if ( xbee.getResponse().getApiId() == RX_64_RESPONSE) {
           xbee.getResponse().getRx64Response(xbrx);
           if ( xbrx.getData(0) == CMD_SYNC_TIME ) {
              for (char c = 0; c < 4; c++ ) {
                 datapacket[c] = xbrx.getData(c + 1); 
              }
              destaddr = xbrx.getRemoteAddress64();
              time = *(uint32_t *)(datapacket);
              xbtx = Tx64Request( destaddr, datapacket, 4);
              xbee.send(xbtx);
           }
       }
   }
   #endif
  
  if (pReady) {
    
    #ifdef SERIAL_OUT
      Serial.print("TIME: ");
      Serial.print(time,DEC);
      Serial.print(" HIGH: ");
      Serial.print(pHighTime,DEC);
      Serial.print(" LOW: ");
      Serial.print(pLowTime,DEC); 
      Serial.print(" DC: ");
      Serial.print(100.0*((double)pLowTime)/((double)pLowTime + pHighTime),2);
      Serial.println("%");
    #else
        makePacket();       
    
        xbtx = Tx64Request( destaddr, datapacket, sizeof(datapacket) );
        xbee.send(xbtx);  
   #endif

    pHighTime = pLowTime = pReady = 0;  
  }
  #endif
}










