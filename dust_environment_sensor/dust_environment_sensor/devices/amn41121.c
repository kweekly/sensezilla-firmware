/*
 * amn41121.c
 *
 * Created: 3/12/2013 5:49:45 PM
 *  Author: kweekly
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#include "drivers/i2cmaster.h"
#include "devicedefs.h"
#include "devices/si7005.h"

void pir_wake(void) {
	PIR_VCC = 1;
	// should wait for at least a while
	// datasheet says 7s (could be up to 30s)
}