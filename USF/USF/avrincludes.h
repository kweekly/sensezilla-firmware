/*
 * avrincludes.h
 *
 * Created: 7/3/2013 2:47:27 PM
 *  Author: kweekly
 */ 


#ifndef AVRINCLUDES_H_
#define AVRINCLUDES_H_

#include <stddef.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Convenience
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "drivers/uart.h"
#define kputs(X) uart_puts_p(PSTR(X))


#endif /* AVRINCLUDES_H_ */