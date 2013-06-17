/*
 * console_PSM.c
 *
 * Created: 5/21/2013 5:12:45 PM
 *  Author: kweekly
 */ 


#include "all.h"

#define CONSOLE_BUFFER_LEN 128
char console_buffer[CONSOLE_BUFFER_LEN];
int console_buffer_pos;

void console_process_cmd();
extern void turn_on_gate(char gateno);
extern void turn_off_gate(char gateno);
extern void toggle_gate(char gateno);

void console_init() {
	console_buffer_pos = 0;
}

void console_check() {
	int rcch = uart_getc();
	char ch;
	if ( (rcch & 0xFF00) == 0 ) {
		ch = rcch & 0xFF;
		if ( ch == '\x0A' || ch == '\x0D') {
			console_buffer[console_buffer_pos] = 0;
			console_process_cmd();	
			console_buffer_pos = 0;
		} else if (console_buffer_pos==0 && (ch == '\x0D' || ch == '\x0A')) {}
		 else {
			console_buffer[console_buffer_pos++] = ch;
		}
		
		if ( console_buffer_pos > CONSOLE_BUFFER_LEN ) {
			kputs("Error: Console buffer overflow!\n");
			console_buffer_pos = 0;
		}
	}
}

void console_process_cmd() {
	if ( console_buffer_pos == 0 ) return;
	char cmd = console_buffer[0];
	if ( cmd == 'h' ) { // read hex power values
		powermon_reading_t reading = powermon_read();
		for (int c = 0; c < POWERMON_NUM_CHANNELS; c++ ) {
			printf_P(PSTR("%d %08lX %08lX %08lX"),c,reading.true_power[c],reading.RMS_current[c],reading.RMS_voltage[c]);
			if ( c < POWERMON_NUM_CHANNELS - 1 )
				printf(",");
		}
		printf("\n");
	} else if (cmd == 'g' ) {
		uint32_t iGain,iOffset,vGain,vOffset;
		int r;
		r = sscanf_P(console_buffer+1,PSTR(" %08lx %08lX %08lx %08lx"),&iGain, &iOffset, &vGain, &vOffset);
		if ( r == 4 ) {
			powermon_set_gains_and_offsets(iGain,iOffset,vGain,vOffset);
			printf_P(PSTR("g OK\n"));
		}
	} else if ( cmd == '+' ) {
		turn_on_gate(console_buffer[1]-'0');
		printf_P(PSTR("+ OK\n"));
	} else if ( cmd == '-' ) {
		printf_P(PSTR("- OK\n"));
		turn_off_gate(console_buffer[1]-'0');
	} else if ( cmd == 't' ) {
		printf_P(PSTR("t OK\n"));
		toggle_gate(console_buffer[1]-'0');
	}
}