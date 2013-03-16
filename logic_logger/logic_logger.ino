
#define BUF_LEN 64
volatile unsigned long buftimes[BUF_LEN];
volatile unsigned char buffer[BUF_LEN];
volatile unsigned char portno[BUF_LEN];
volatile char buf_start, buf_end;
volatile char ovf; 

ISR(PCINT0_vect) {
   buftimes[buf_end] = millis();
   buffer[buf_end] = PINB;
   portno[buf_end] = 0;
   
   if ((buf_end + 1)%BUF_LEN == buf_start) {
     ovf = 1;
   } else {
      buf_end = (buf_end + 1)%BUF_LEN;
   }
   PCIFR |= 1;
}

ISR(PCINT1_vect) {
   buftimes[buf_end] = millis();
   buffer[buf_end] = PINC;
   portno[buf_end] = 1;
   
   if ((buf_end + 1)%BUF_LEN == buf_start) {
     ovf = 1;
   } else {
      buf_end = (buf_end + 1)%BUF_LEN;
   }
   PCIFR |= 2;
}

ISR(PCINT2_vect) {
   buftimes[buf_end] = millis();
   buffer[buf_end] = PIND;
   portno[buf_end] = 1;
   
   if ((buf_end + 1)%BUF_LEN == buf_start) {
     ovf = 1;
   } else {
      buf_end = (buf_end + 1)%BUF_LEN;
   } 
   PCIFR |= 4;
}

#define CHANNELS 13
volatile unsigned long time_ms;
volatile unsigned char pingpong;
volatile unsigned int countsPing[CHANNELS], countsPong[CHANNELS];
volatile unsigned int numPing, numPong;

ISR(TIMER1_COMPA_vect)
{
    time_ms++;
    
    volatile unsigned int * counts;
    volatile unsigned int * num;
    if (pingpong) {
      counts = countsPing;
      num = &numPing;
    } else {
      counts = countsPong;
      num = &numPong;
    }
    
    counts[0] += (PINB & _BV(0)) != 0;
    counts[1] += (PINB & _BV(1)) != 0;
    counts[2] += (PINB & _BV(2)) != 0;
    counts[3] += (PINB & _BV(3)) != 0;
    
    counts[4] += (PIND & _BV(2)) != 0;
    counts[5] += (PIND & _BV(3)) != 0;
    counts[6] += (PIND & _BV(6)) != 0;
    counts[7] += (PIND & _BV(7)) != 0;
    counts[8] += (PIND & _BV(4)) != 0;
    counts[9] += (PIND & _BV(5)) != 0;
    
    counts[10] += (PINC & _BV(0)) != 0;
    counts[11] += (PINC & _BV(1)) != 0;
    counts[12] += (PINC & _BV(2)) != 0;
    
    (*num)++;
}


// We are using Digital 2-10, Analog 0-2
// That is PD2-7, PB0-2, PC0-2, for a total of 13

void setup() {
   Serial.begin(115200,SERIAL_8N2);
   delay(500);
   Serial.println("# Logger Configurating ");
   DDRD &= 0b00000011;
   DDRB &= 0b11111000;
   DDRC &= 0b11111000;
   
   // Enable IOC
   
   // this is for port B
   PCMSK0 = 0b11111100;
   // this is for port C
   PCMSK1 = 0b00000011;
   // this is for port D
   PCMSK2 = 0b00000011;
   pinMode(A0, INPUT);
   pinMode(A1, INPUT);
   pinMode(A2, INPUT);
   
   //enable all interrupt vectors
   //PCICR = 0x07;
   // disable all interrupt vectors
   PCICR = 0;
   
  // Using Timer 1 for clock 1ms interrupt
  // clk = 16000000
  // clk/1000 = 16000
  // CTC mode, period = 16000, prescale = 1
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B
 
  // set compare match register to desired timer count:
  // OCR1A = 15999;
  OCR1A = 7999; // try 500us?
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // Set CS10 bit for 1 prescaler:
  TCCR1B |= (1 << CS10);
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);
   time_ms = 0;
  
   
   buf_start = buf_end = ovf = 0;
   numPing = numPong = 0;
   for ( int c = 0; c < CHANNELS; c++ ) {
     countsPing[c] = 0;
     countsPong[c] = 0; 
   }
   pingpong = 1;
   
   interrupts();
   Serial.println("# Logger Started");
}

void loop() {
    volatile unsigned int * counts;
    volatile unsigned int * num;
    if (pingpong) {
      counts = countsPing;
      num = &numPing;
    } else {
      counts = countsPong;
      num = &numPong;
    }
  
  if (*num >= 100) {
      pingpong = !pingpong;
      Serial.print(time_ms,DEC);
      Serial.print(' ');
      Serial.print(*num,DEC);
      for ( int c = 0; c < CHANNELS; c++ ) {
        Serial.print(' ');
         Serial.print(counts[c],DEC); 
         counts[c] = 0;
      }
      Serial.println();
      *num = 0;      
  }
     
  /*
  if (ovf) {
    //Serial.println("# Overflow!");
    ovf = 0; 
  }
  
  if  ( buf_end != buf_start ) {
     Serial.print(buftimes[buf_start],DEC);
     Serial.print(' ');
     Serial.print(portno[buf_start],DEC);
     unsigned char val = buffer[buf_start];
     for (int c = 0; c < 8; c++) {
       if ( val & 1 ) {
         Serial.print(" 1");
       } else {
         Serial.print(" 0");
       }
       val >>= 1;
     }
     Serial.println();
    
     buf_start = (buf_start + 1)%BUF_LEN;
  }
  */
}
