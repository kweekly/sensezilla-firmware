/*
 * dust_environment_sensor.c
 *
 * Created: 3/12/2013 3:59:40 PM
 *  Author: kweekly
 */ 



#include <avr/io.h>
#include <avr/sleep.h>
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

#include "utils/scheduler.h"

#include "devicedefs.h"

static FILE mystdout = FDEV_SETUP_STREAM(uart_putc, NULL, _FDEV_SETUP_WRITE);


void board_power_down_devices(void);
void avr_doze(void);

void task_led_blip_on(void);
void task_led_blip_off(void);

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
	
	kputs("Initializing i2c\n");
	i2c_init();
	
	
	kputs("Initializing Scheduler and tasks\n");
	scheduler_init();
	scheduler_add_task(LED_BLIP_TASK_ID, 0, &task_led_blip_on);
	scheduler_add_task(LED_BLIP_TASK_ID, 500, &task_led_blip_off);
	
	
	kputs("Initializing Devices... Light,");
	light_init();
	kputs("Gyro, ");
	gyro_init();
	kputs("Accell, ");
	accel_init();
	kputs("Humid, ");
	humid_init();
	kputs("PIR\n");
	pir_init();
	
	kputs("Powering down all devices\n");
	board_power_down_devices();
		
    while(1)
    {
		kputs("Gogo scheduler\n");
		scheduler_start();
		_delay_ms(1000);
    }
}

void task_led_blip_on(void) {
	LED1 = 1;
}

void task_led_blip_off(void) {
	LED1 = 0;
}

void board_power_down_devices(void) {
	LED1 = 0;
	LED2 = 0;
	
	light_sleep();
	gyro_sleep();
	accel_sleep();
	humid_sleep();
	pir_sleep();
}


void avr_doze(void) {
	// go into idle mode
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	sleep_cpu();
	sleep_disable();	
}	