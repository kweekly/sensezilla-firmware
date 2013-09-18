/* 
 * File:   beeperPIC.c
 * Author: kweekly
 *
 * Created on September 17, 2013, 11:39 AM
 */

#include <pic.h>
#include <xc.h>
#include <math.h>

#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Detect (BOR disabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)

void delay_us(unsigned short ticks);

volatile unsigned long counter_ms;

// only timer can hit this interrupt
void interrupt isr(void) {
    PORTC ^= 0b00001100;
    TMR2IF = 0;
}

void delay_us(unsigned short ticks) {
    TMR1H = TMR1L = 0;
    while ( TMR1 < ticks / 8 );
}


/*
 * 
 */
int main(int argc, char** argv) {
    counter_ms = 0;

    // set oscillator to 4MHz
    OSCCON = 0b01100001;

    // MOSI = RA0 (AN0), SSEL = RA1 (AN1)
    // inputs
    TRISA |= 0b00000011;
    // enable pin change interrupt flags, but not ISR
    RAIE = 0;
    IOCA =   0b00000011;

    // speaker is on P1C(RC3/AN7) and P1D(RC2/AN6)
    // default to off
    RC3 = 0;
    RC2 = 1;

    // set to outputs
    TRISC &= 0b11110011;

    // turn off Analog
    ANSEL &= 0b00111100;
    CMCON0 = 0b00000111;

    // period of timer 2 for 4KHz (tick is 4/4 = 1MHz)
    PR2 = 125;

    // turn on Timer 2, 1:1 postscaler, prescaler
    T2IE = 0;
    T2CON =  0b00000100;

    // turn on Timer 1, prescaler of 8.  Tick is 125KHz = 8us
    TMR1H = TMR1L = 0;
    T1CON =  0b00110101;

    // turn on interrupts only when beeping
    TMR2IF = 0;
    TMR2IE = 1;
    GIE = 0;
    PEIE = 1;

    unsigned const short beep_len    = 20000;
    unsigned const short silence_len = 50000;
    while(1) {
        // wait for trigger-
        char pval;
        while (1) {
            pval = PORTA;
            if ( (pval & 0x03) == 0 ) {
                RAIF = 0;

                // make sure pins stay the same for at least 50uS
                delay_us(50);
                if ( RAIF == 0 ) { // no change
                    break;
                }
            }
        }

        PR2 = 125;
        for ( int c = 0; c < 4; c++ ) {
            GIE = 1;
            delay_us(beep_len);
            GIE = 0;
            delay_us(silence_len);
        }

        PR2 = 250;
        for ( int c = 0; c < 4; c++ ) {
            GIE = 1;
            delay_us(beep_len);
            GIE = 0;
            delay_us(silence_len);
        }
        // delay for a sec
        for ( long c = 0; c < 75; c++)
            delay_us(10000); // 10ms
    }
}

