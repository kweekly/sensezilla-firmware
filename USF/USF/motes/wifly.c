/*
 * wifly.c
 *
 * Created: 9/25/2013 12:47:09 PM
 *  Author: kweekly
 */ 
#include "devicedefs.h"

#if MOTE_TYPE == MOTE_WIFLY
#define WIFLY_CMD_RESP_TIMEOUT 100 // in 100us
#define WIFLY_WAKE_TIMEOUT 100

#include "avrincludes.h"
#include "protocol/report.h"
#include "utils/scheduler.h"
#include "motes/wifly.h"

uint8_t mote_line_buffer[128];
uint16_t line_buffer_pos;
uint8_t escape_next_char;

uint8_t mote_cmd_resp_buf[16];
uint8_t cmd_resp_recieved;

void _wifly_process_byte(char c);

void _wifly_read_rssi();

void (*rx_cb)(uint8_t * buf, uint16_t len);

void wifly_init() {
	line_buffer_pos = 0;
	escape_next_char = 0;
	
	MOTE_RESETN = 1;
	wifly_wake();
	
	MOTE_UART_INIT(UART_BAUD_SELECT_DOUBLE_SPEED(115200,F_CPU));
}

void wifly_wake() {
	MOTE_RX_RTSN = 0;
}

void wifly_sleep() {
	MOTE_RX_RTSN = 1;
	#ifdef LOW_POWER
	//MOTE_RX_RTSN = 1; // de-asserted
	MOTE_UART_FLUSH();
	MOTE_RX = 0;
	_delay_us(400); // send a break char
	MOTE_RX = 1;
	MOTE_UART_INIT(UART_BAUD_SELECT_DOUBLE_SPEED(115200,F_CPU));
	#endif
}

void wifly_send_packet(uint8_t * buf, uint16_t len) {
	// wait for CTS to go LOW
	int8_t cntr = WIFLY_WAKE_TIMEOUT;
	while(--cntr && MOTE_TX_CTSN_PIN) {
		 _delay_us(100);
	 } // wait for CTS to go low
	 if ( !cntr ) {
		 kputs("Timeout waiting for CTS to go low\n");
		 return;
	 }	
	
	for ( uint16_t c = 0; c < len; c++ ) {
		//printf_P(PSTR("%02X"),buf[c]);
		if ( buf[c] == 0x7D || buf[c] == '\n' ) {
			MOTE_UART_PUTC(0x7D);
			MOTE_UART_PUTC(buf[c] ^ 0x20);
		} else {
			MOTE_UART_PUTC(buf[c]);
		}
	}
	//kputs("\n");
	MOTE_UART_PUTC('\n');
}

void wifly_set_rx_callback(void (*r)(uint8_t * buf, uint16_t len)) {
	rx_cb = r;
}

void wifly_tick() {
	int ch;
	while( ((ch = MOTE_UART_GETC()) & 0xFF00) == 0 ){
		uart_putc(ch);
		_wifly_process_byte(ch & 0xFF);
	}
}

void _wifly_process_byte(char c) {
	if (c == '\n') {
		if ( line_buffer_pos > 0 ) {
			mote_line_buffer[line_buffer_pos++] = 0;
			rx_cb(mote_line_buffer,line_buffer_pos);
			line_buffer_pos = 0;
		}
	} else {
		if ( escape_next_char ) {
			mote_line_buffer[line_buffer_pos++] = c ^ 0x20;
			escape_next_char = 0;
		} else if ( c == 0x7D ) {
			escape_next_char = 1;
		} else {
			mote_line_buffer[line_buffer_pos++] = c;
		}
	}
}

void wifly_setup_reporting_schedule(uint16_t starttime) {
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,MOTE_TASK_ID, starttime += 50, &_wifly_read_rssi);// ensure that this happens before the report is sent
}

void wifly_fmt_reading(int8_t * reading,size_t bufsize,char * buf) {
	snprintf_P(buf,bufsize,PSTR("RSSI=%4ddBm"),*reading);
}

void _wifly_read_rssi() {
	report_current()->rssi = 0;
	report_current()->fields |= REPORT_TYPE_RSSI;
}

void wifly_flush() {
	while( (MOTE_UART_GETC() & 0xFF00) == 0 ); // flush input buffer
}
#endif