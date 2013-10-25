/*
 * power.c
 *
 * Created: 7/3/2013 4:44:06 PM
 *  Author: kweekly
 */ 

#include "devicedefs.h"
#include "avrincludes.h"
#include "protocol/datalink.h"


void avr_doze(void) {
	// go into idle mode
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	sleep_cpu();
	sleep_disable();
}

void avr_sleep(void) {
//	kputs("I go to sleep.\n");
	uart_flush();
	uart1_flush();
	datalink_sleep_hook();
	// go into sleep (only external interrupt or timer can wake us up)
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);
	sleep_enable();
	sleep_cpu();
	sleep_disable();
	datalink_wake_hook();
//	kputs("I awaken\n");
}
