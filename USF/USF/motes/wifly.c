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
#include "motes/wifly_settings.h"

uint8_t mote_line_buffer[128];
uint16_t line_buffer_pos;
uint8_t escape_next_char;

uint8_t mote_cmd_resp_buf[16];
uint8_t cmd_resp_recieved;

void _wifly_process_byte(char c);

uint8_t in_command_mode;
void _wifly_enter_command_mode();
void _wifly_exit_command_mode();
void _wifly_wait_for_command_resp();

void _wifly_read_rssi();

void (*rx_cb)(uint8_t * buf, uint16_t len);

#define CMD_SET_BR "set u i 115200\n"
#define CMD_ENTER_CMD_MODE "$$$"
#define CMD_EXIT_CMD_MODE "exit\n"

void wifly_init() {
	line_buffer_pos = 0;
	in_command_mode = 0;
	escape_next_char = 0;
	
	MOTE_RESETN = 1;
	wifly_wake();
	MOTE_RX_RTSN = 0;
	
	kputs("Auto-force baud rate to 9600...\n");
	MOTE_UART_INIT(UART_BAUD_SELECT(9600,F_CPU));
	_wifly_enter_command_mode();
	MOTE_UART_WRITE(sizeof(CMD_SET_BR),CMD_SET_BR);
	_delay_ms(10);
	
	kputs("Switching to 115200...\n");
	MOTE_UART_INIT(UART_BAUD_SELECT_DOUBLE_SPEED(115200,F_CPU));
	_wifly_enter_command_mode();
	
	size_t settings_len = sizeof(WIFLY_SETTINGS_STR);
	const char * settings_str  = WIFLY_SETTINGS_STR;
	char cmdbuf[16];
	uint8_t cmdlen = 0;
	for ( size_t c = 0; c < settings_len; c++ ) {
		char ch = pgm_read_byte(&(settings_str[c]));
		if ( ch == '\n') {
			kputs("\t");
			uart_write(cmdlen,cmdbuf);
			kputs(" ... ");
			cmdbuf[cmdlen++] = '\n';
			if (wifly_exec_command(cmdbuf,cmdlen)) {
				kputs("success.\n");
			} else {
				kputs("failed.\n");
			}
			cmdlen = 0;
		} else {
			cmdbuf[cmdlen++] = ch;
		}
		
	}
	
	_wifly_exit_command_mode();
	
}

void wifly_wake() {
	MOTE_RX_RTSN = 0; // asserted
}

void wifly_sleep() {
	MOTE_RX_RTSN = 1; // de-asserted	
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
		printf_P(PSTR("%02X"),buf[c]);
		if ( buf[c] == 0x7D || buf[c] == '\n' ) {
			MOTE_UART_PUTC(0x7D);
			MOTE_UART_PUTC(buf[c] ^ 0x20);
		} else {
			MOTE_UART_PUTC(buf[c]);
		}
	}
	kputs("\n");
	MOTE_UART_PUTC('\n');
}

void wifly_set_rx_callback(void (*r)(uint8_t * buf, uint16_t len)) {
	rx_cb = r;
}

void wifly_tick() {
	int ch;
	while( ((ch = MOTE_UART_GETC()) & 0xFF00) == 0 ){
		_wifly_process_byte(ch & 0xFF);
	}
}

void _wifly_process_byte(char c) {
	if (c == '\n') {
		if ( line_buffer_pos > 0 ) {
			mote_line_buffer[line_buffer_pos++] = 0;
			if ( in_command_mode ) {
				strncpy(mote_cmd_resp_buf, mote_line_buffer, sizeof(mote_cmd_resp_buf));
				cmd_resp_recieved = 1;
			} else {
				rx_cb(mote_line_buffer,line_buffer_pos);
			}
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

void _wifly_enter_command_mode() {
	_delay_ms(250);
	wifly_flush();
	MOTE_UART_WRITE(sizeof(CMD_ENTER_CMD_MODE),CMD_ENTER_CMD_MODE);
	_delay_ms(250);
	_wifly_wait_for_command_resp();
	if ( memcmp_P(mote_cmd_resp_buf, "CMD", sizeof("CMD"))) {
		printf_P(PSTR("Mote returned %s instead of CMD\n"),mote_cmd_resp_buf);
	}
	in_command_mode = 1;
}

void _wifly_exit_command_mode() {
	_delay_ms(10);
	wifly_flush();
	MOTE_UART_WRITE(sizeof(CMD_EXIT_CMD_MODE),CMD_EXIT_CMD_MODE);
	_wifly_wait_for_command_resp();
	if ( memcmp_P(mote_cmd_resp_buf, "EXIT", sizeof("EXIT"))) {
		printf_P(PSTR("Mote returned %s instead of EXIT\n"),mote_cmd_resp_buf);
	}
	in_command_mode = 0;
}

void _wifly_wait_for_command_resp() {
	int tries = WIFLY_CMD_RESP_TIMEOUT;
	cmd_resp_recieved = 0;
	mote_cmd_resp_buf[0] = 0;
	while(tries-- && !cmd_resp_recieved) {
		wifly_tick();
		_delay_us(100);
	}
}

uint8_t wifly_exec_command(uint8_t * cmd, uint16_t len) {
	if ( in_command_mode ) {
		MOTE_UART_WRITE(len,cmd);
		_wifly_wait_for_command_resp();
		if ( memcmp_P(mote_cmd_resp_buf, "AOK", sizeof("AOK")) ) {
			return 0;
		}
		return 1;
	}
	return 0;
}

void wifly_flush() {
	while( (MOTE_UART_GETC() & 0xFF00) == 0 ); // flush input buffer
}
#endif