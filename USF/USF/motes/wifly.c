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
#define WIFLY_CONNECT_TIMEOUT 3 // in seconds

#include "avrincludes.h"
#include "protocol/report.h"
#include "utils/scheduler.h"
#include "motes/wifly.h"
#include "drivers/pcint.h"
#include "drivers/rtctimer.h"

uint8_t mote_line_buffer[128];
uint16_t line_buffer_pos;
uint8_t escape_next_char;
#define LATCH_OFF		0
#define LATCH_PRIMED	1
#define LATCH_FIRED		2
uint8_t line_recieved_latch;

uint8_t mac_address_buffer[6];
uint8_t mac_valid;

uint8_t wifly_wake_status;

void _wifly_process_byte(char c);

void _wifly_read_rssi();

void (*rx_cb)(uint8_t * buf, uint16_t len);
void (*rdy_cb)(void);

volatile uint8_t trying_to_connect;
uint32_t try_connect;
uint32_t try_sleep;

void wifly_init() {
	uint8_t success = 0;
	line_buffer_pos = 0;
	escape_next_char = 0;
	mac_valid = 0;
	trying_to_connect = try_connect = 0;
	line_recieved_latch = LATCH_OFF;
	
	#ifdef MOTE_CONNECT
		MOTE_CONNECT = 0;
	#endif
	#ifdef MOTE_SLEEP
		MOTE_SLEEP = 0;
	#endif	
	
	MOTE_RESETN = 1;
	MOTE_RX_RTSN = 0;
	_delay_us(100);
	MOTE_RX_RTSN = 1;
	_delay_us(100);
	MOTE_RX_RTSN = 0;
	#ifdef MOTE_CONNECTED_PCINT
		pcint_register(MOTE_CONNECTED_PCINT,&_wifly_pcint_cb);
	#endif
	
	kputs("Trying to enter command mode...");
	MOTE_UART_INIT(UART_BAUD_SELECT_DOUBLE_SPEED(57600,F_CPU));
	_delay_ms(500);
	MOTE_UART_WRITE(3,"$$$");
	_delay_ms(500);
	
	success = 0;
	while ( !success && _wifly_readline() ) {
		if ( strstr(mote_line_buffer,"CMD") ) {
			success = 1;
		}
	}
	if ( success ) {
		kputs("Success!\n");
		_wifly_get_MAC();
	} else {
		kputs("FAIL!");
	}
	// flush anything left
	while(_wifly_readline());
	line_recieved_latch = LATCH_OFF;
	
	MOTE_RX_RTSN = 1;
	MOTE_UART_WRITE(sizeof("exit\r\n"),"exit\r\n");
	_delay_ms(10);
	#ifdef MOTE_SLEEP
		MOTE_SLEEP = 1;
		_delay_ms(1);
		MOTE_SLEEP = 0;
	#endif
	wifly_wake_status = 0;
	
}

void wifly_wake() {
	#ifdef MOTE_SLEEP
		MOTE_SLEEP = 0;
	#endif
	MOTE_RX_RTSN = 0;
	printf_P(PSTR("Waking Wifly %02X\n"),PINB);
	LED1 = 1;
	wifly_wake_status = 1;
	if ( !mac_valid ) {
		_delay_ms(250);
		MOTE_UART_WRITE(3,"$$$");
		_delay_ms(250);
		_wifly_get_MAC();
		MOTE_UART_WRITE(sizeof("exit\r\n"),"exit\r\n");
	}
	try_connect = rtctimer_read();

	if ( MOTE_CONNECTED_PIN ) {
		kputs("Wifly already ready!\n");
		if (rdy_cb)
			rdy_cb();
		trying_to_connect = 0;
	} else if ( MOTE_ASSOCIATED_PIN ) { // associated, but not connected
		_wifly_TCP_connect();
		trying_to_connect = 1;
	} else {
		trying_to_connect = 1;
	}
	MOTE_RX_RTSN = 1;
}

void wifly_wake_hook() {
	if ( wifly_wake_status ) {
		MOTE_RX_RTSN = 0;
		wifly_tick();
	}
}

void wifly_sleep_hook() {
	if (MOTE_RX_RTSN == 0 ) {
		MOTE_RX_RTSN = 1;
		wifly_tick();
	}
}

void wifly_get_ID(uint8_t ** uid_buf, uint8_t * uid_len) {
	*uid_buf = mac_address_buffer;
	*uid_len = sizeof(mac_address_buffer);
}

void _wifly_get_MAC() {
	kputs("Getting MAC Address...");
	MOTE_UART_WRITE(sizeof("get mac\r\n"),"get mac\r\n");
	uint8_t success = 0;
	while ( !success && _wifly_readline() ) {
		char * strpos;
		//printf(mote_line_buffer);
		if ( (strpos = strstr(mote_line_buffer,"Mac Addr=")) ) {
			success = 1;
			strpos += sizeof("Mac Addr=") - 1;
			strpos[17] = 0;
			printf_P(PSTR("MAC Address: %s\n"),strpos);
			for(size_t i = 0; i < 6; i++) {
				mac_address_buffer[i] = (uint8_t)strtoul(strpos + i*3,NULL,16);
			}
			mac_valid = 1;
			break;
		}
	}
	if ( !success ) {
		kputs("FAIL!");
		for ( size_t i = 0; i < 6; i++ ) {
			mac_address_buffer[i] = 0;
		}
	}	
	line_recieved_latch = LATCH_OFF;
}

void _wifly_TCP_connect() {
	MOTE_UART_WRITE(3,"$$$");
	_delay_ms(250);
	MOTE_UART_WRITE(sizeof("\r\nopen\r\nexit\r\n"),"\r\nopen\r\nexit\r\n");
}

void _wifly_pcint_cb() {
	if ( trying_to_connect && MOTE_CONNECTED_PIN && rdy_cb) {
		kputs("Wifly ready!\n");
		trying_to_connect = 0;
		rdy_cb();
	}
}

void wifly_sleep() {
	MOTE_RX_RTSN = 1;
	kputs("Sleeping Wifly\n");
	trying_to_connect = 0;
	#ifdef LOW_POWER
		#ifdef MOTE_SLEEP
			MOTE_SLEEP = 1;
			_delay_ms(1);
			MOTE_SLEEP = 0;
		#else
			MOTE_UART_FLUSH();
			_delay_ms(250);
			MOTE_UART_WRITE(3,"$$$");
			_delay_ms(250);
			MOTE_UART_WRITE(sizeof("\r\nclose\r\nsleep\r\n"),"\r\nclose\r\nsleep\r\n");
		#endif
		wifly_wake_status = 0;
	#endif
	LED1 = 0;
	try_sleep = rtctimer_read();
}

void wifly_send_packet(uint8_t * buf, uint16_t len) {
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

void wifly_set_rdy_callback(void (*r)(void)) {
	rdy_cb = r;
}

void wifly_tick() {
	int ch;
	//EXP_SCK = 1;
	uint32_t curtime = rtctimer_read();
	if ( trying_to_connect) {
		if((curtime - try_connect) > WIFLY_CONNECT_TIMEOUT || curtime < try_connect ){
			printf_P(PSTR("Timeout trying to TCP connect %02X\n"),PINB);
			wifly_sleep();
		}	
		else if ( !MOTE_ASSOCIATED_PIN && MOTE_CONNECTED_PIN) {
			_wifly_TCP_connect();
		}
	}
	
	#ifdef LOW_POWER
	if (wifly_wake_status == 0 && MOTE_ASSOCIATED_PIN && curtime - try_sleep > 3) { 
		kputs("WiFly needs to go to sleep, bye bye.");
		wifly_sleep();
	}
	#endif
	
	
	while(line_recieved_latch != LATCH_FIRED) {
		ch = MOTE_UART_GETC();
		uint8_t err = ch>>8;
		if ( err == 0x01 ) break;
		else if (err) {
			//printf_P(PSTR("UART ERROR %02X\n"),err);
			continue;
		} else {
			_wifly_process_byte(ch & 0xFF);
		}
	}	

	//EXP_SCK = 0;
}

void _wifly_process_byte(char c) {
	if (c == 0x0A) {
		if ( line_buffer_pos > 0 ) {
			mote_line_buffer[line_buffer_pos++] = 0;
			//printf_P(PSTR("RCV %s\n"),mote_line_buffer);
			if ( rx_cb && line_recieved_latch == LATCH_OFF)
				rx_cb(mote_line_buffer,line_buffer_pos);
			line_buffer_pos = 0;
			if ( line_recieved_latch == LATCH_PRIMED ) {
				line_recieved_latch = LATCH_FIRED;	
			}
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

uint8_t _wifly_readline() {
	line_recieved_latch = LATCH_PRIMED;
	uint8_t tmout = 10;
	wifly_tick();
	while ( tmout-- && line_recieved_latch == LATCH_PRIMED )  {
		wifly_tick();
		_delay_ms(100);
	}
	uint8_t ret = (line_recieved_latch == LATCH_FIRED);
	return ret;
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