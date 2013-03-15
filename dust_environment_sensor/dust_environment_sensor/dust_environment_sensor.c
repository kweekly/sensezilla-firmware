/*
 * dust_environment_sensor.c
 *
 * Created: 3/12/2013 3:59:40 PM
 *  Author: kweekly
 */ 



#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "drivers/uart.h"
#include "drivers/i2cmaster.h"

#include "devices/amn41121.h"
#include "devices/l3gd20.h"
#include "devices/lis3dh.h"
#include "devices/si7005.h"
#include "devices/tsl2560.h"

#include "devicedefs.h"

static FILE mystdout = FDEV_SETUP_STREAM(uart_putc, NULL, _FDEV_SETUP_WRITE);


void board_power_down_devices(void);
void board_sleep(void);



int main(void)
{
	cli();	
	LED1 = 1;
	uart_init(UART_BAUD_SELECT(9600,F_CPU));
	stdout = &mystdout;
	sei();
	
	kputs("Setting DDR registers\n");
	DDRA = DDRA_SETTING;
	DDRB = DDRB_SETTING;
	DDRC = DDRC_SETTING;
	DDRD = DDRD_SETTING;
	DIDR0 = 0;
	DIDR1 = 0;
	HUMID_VCC = 1;
	
	kputs("Initializing i2c\n");
	i2c_init();
	
	kputs("Initializing Devices\n");
	light_init();
	gyro_init();
	
	LED1 = 0;
	
	unsigned long timer = 0;
	char pirlat = 0;
	light_reading_t lightlevel;
	
	board_power_down_devices();
    while(1)
    {
		board_sleep();
    }
}

void board_power_down_devices(void) {
	PIR_VCC = 0;
	HUMID_VCC = 0;
	LED1 = 0;
	LED2 = 0;
	
	light_sleep();
}

void board_sleep(void) {
	
	
}