/*
 * xbee.c
 *
 * Created: 4/1/2013 12:52:02 PM
 *  Author: kweekly
 */ 
#include "devicedefs.h"

#if MOTE_TYPE == MOTE_XBEE
#include "avrincludes.h"
#include "motes/xbee.h"
#include "protocol/report.h"
#include "utils/scheduler.h"
#include "motes/xbee_AT_settings.h"

#define XBEE_TX_API_BUF_SIZE 128
#define XBEE_RX_API_BUF_SIZE 64
#define XBEE_AT_RESP_TIMEOUT 100 // in 100uS
#define XBEE_TX_RESP_TIMEOUT 1000

uint8_t *rx_client_buf;
void (*rx_callback)(xbee_16b_address addr_16b, xbee_64b_address addr_64b, uint8_t rssi, uint16_t nBytes);
void (*tx_callback)(uint8_t frame_id, uint8_t status);
void (*status_callback)(uint8_t status);

uint8_t xbee_hw_series;

uint8_t modem_status;
uint8_t modem_initialized;
uint8_t frame_id;

uint16_t xbee_rx_api_buf_len;
uint16_t xbee_rx_api_buf_pos;
uint8_t xbee_rx_api_buf[XBEE_RX_API_BUF_SIZE];
uint8_t esc_char_recv;

uint8_t *rx_at_cmd_buf;
int8_t rx_at_cmd_status;

uint8_t tx_status;

void xbee_init() {	
	MOTE_RESETN = 1;
	xbee_wake();
	MOTE_RX_RTSN = 0; // asserted

	modem_status = XBEE_STATUS_HW_RESET;
	frame_id = 1;
	modem_initialized = 0;
		
	// comms initialization
	int8_t resp;
	uint8_t buf[16];
	/*
	kputs("\tCheck if in AT mode...");
	MOTE_UART_INIT(UART_BAUD_SELECT(9600,F_CPU));
	_delay_ms(1000);
	while( (MOTE_UART_GETC() & 0xFF00) == 0 ); // flush input buffer
	char cmd1[] = "+++";
	MOTE_UART_WRITE(sizeof(cmd1)-1,cmd1);
	_delay_ms(1250);
	char ch1 = MOTE_UART_GETC(), ch2 = MOTE_UART_GETC(), ch3 = MOTE_UART_GETC();
	if (ch1 == 0x4F && ch2 == 0x4B && ch3 == 0x0D ) {// "OK" response
		kputs("yes, attempt correction\n");
		char cmd2[] = "ATBD7,AP2,WR,AC,CN\x0D";
		MOTE_UART_WRITE(sizeof(cmd2)-1,cmd2);	
		_delay_ms(500);
		MOTE_RESETN = 0;
		_delay_ms(100);
		MOTE_RESETN = 1;
		_delay_ms(500);
	} else {
		printf_P(PSTR("Maybe not %c%c%c\n"),ch1,ch2,ch3);
	}
	*/
		
	kputs("\tAttempt communication at 115200...");
	MOTE_UART_INIT(UART_BAUD_SELECT_DOUBLE_SPEED(115200,F_CPU));
	_delay_ms(100);
	resp = xbee_AT_get("HV",buf);
	
	if ( resp >= 0 ) { // success
		kputs("success, communication established.\n");
	} else if ( resp == XBEE_AT_TIMEOUT ) {
		kputs("timeout. (attempt correction)\n");
		kputs("\tAttempt communication at 9600...");
		MOTE_UART_INIT(UART_BAUD_SELECT(9600,F_CPU));
		_delay_ms(100);
		resp = xbee_AT_get("HV",buf);
		if ( resp >= 0 ) { // success
			kputs("success, changing to 115200...");
			buf[0] = 7;
			resp = xbee_AT_set_resp("BD",1,buf);
			if ( resp >= 0 ) {
				kputs("success.\n");
			} else {
				printf_P(PSTR("Error %d\n"),resp);
			}
			xbee_AT_set("CN",0,NULL);
			xbee_AT_set("AC",0,NULL);
		} else if ( resp == XBEE_AT_TIMEOUT ) {
			kputs("timeout. (OK)\n");
		} else {
			printf_P(PSTR("error %d\n"),resp);
		}
	
		kputs("\tAttempt communication at 115200...");
		MOTE_UART_INIT(UART_BAUD_SELECT_DOUBLE_SPEED(115200,F_CPU));
		_delay_ms(100);
		resp = xbee_AT_get("HV",buf);
		if ( resp >= 0 ) { // success
			kputs("success, communication established.\n");
		} else if ( resp == XBEE_AT_TIMEOUT ) {
			kputs("timeout. (bad)\n");
		} else {
			printf_P(PSTR("error %d\n"),resp);
		}	
	}
	
	
	
	if ( resp < 0 ) {
		return;		
	}
	
	size_t settings_len;
	const char * settings_str ;
	
	printf_P(PSTR("\tHV=%02X%02X "),buf[0],buf[1]);
	if ( buf[0] == 0x17 ) {
		xbee_hw_series = 1;
		kputs("(XBee Series 1 802.15.4)\n");
		settings_len = sizeof(XBEE_AT_SETTING_STR_S1);
		settings_str = XBEE_AT_SETTING_STR_S1;
	} else if ( buf[0] == 0x19 || buf[0] == 0x1A || buf[0] == 0x1E ) {
		xbee_hw_series = 2;
		kputs("(XBee Series 2 ZB)\n");
		settings_len = sizeof(XBEE_AT_SETTING_STR_S2);
		settings_str = XBEE_AT_SETTING_STR_S2;
	} else if ( buf[0] == 0x1F || buf[0] == 0x27 ) {
		xbee_hw_series = 6;
		kputs("(XBee Series 6 WiFi)\n");
		settings_len = sizeof(XBEE_AT_SETTING_STR_S6);
		settings_str = XBEE_AT_SETTING_STR_S6;
	} else {
		kputs("(Unknown)\n");
		return;
	}
	
	char snbuf[8];
	resp = xbee_AT_get("SH",snbuf);
	if ( resp < 0 ) {
		kputs("Error getting SH\n");
	} else {
		resp = xbee_AT_get("SL",snbuf+4);
		if ( resp < 0 ) {
			kputs("Error getting SL\n");
		} else {
			kputs("\tSerial No = ");
			for ( int c = 0; c < 8; c++ ) {
				printf_P(PSTR("%02X"),snbuf[c]);
			}
			kputs("\n");
		}
	}		
	
	uint16_t pos = 0;
	char atcmd[2];
	uint8_t atbuf[16];
	while ( pos < settings_len ) {
		// read atcmd
		atcmd[0] = pgm_read_byte(&(settings_str[pos]));
		atcmd[1] = pgm_read_byte(&(settings_str[pos+1]));
		if ( atcmd[0] == 'X' && atcmd[1] == 'X') break;
		// read value
		pos = pos + 2;
		uint8_t b,c;
		c=0;
		while(1){
			b = pgm_read_byte(&(settings_str[pos++]));
			if ( b == ',') break;
			atbuf[c++] = b;
		}
		printf_P(PSTR("\t%c%c <= "),atcmd[0],atcmd[1]);
		for ( b = 0; b < c; b++ ) {
			printf_P(PSTR("%02X"),atbuf[b]);
		}
		kputs(" ... ");
		
		resp = xbee_AT_set_resp(atcmd,c,atbuf);
		
		if ( resp < 0 ) {
			printf_P(PSTR("error %d, retry..."),resp);
			resp = xbee_AT_set_resp(atcmd,c,atbuf);
			if ( resp < 0 ) {
				printf_P(PSTR("error %d, fail.\n"),resp);
			} else {
				kputs("success.\n");
			}
		} else {
			printf_P(PSTR("success.\n"));
		}
		_delay_ms(200);
	}
	/*
	printf_P(PSTR("Writing to nonvolatile... "));
	resp = xbee_AT_set_resp("AC",0,NULL);
	if ( resp < 0 ) {
		printf_P(PSTR("error on AC %d"),resp);
	}
	resp = xbee_AT_set_resp("WR",0,NULL);
	if ( resp < 0 ) {
		printf_P(PSTR("error on WR %d"),resp);
	}
	kputs("\n");
	*/
	modem_initialized = 1;
}

uint8_t xbee_get_type() {
	return xbee_hw_series;
}

void xbee_sleep(){
	//kputs("XB Sleep\n");
#ifdef LOW_POWER
	MOTE_RX_RTSN = 1; // de-asserted
	MOTE_SLEEP = 1;
#endif
}

void xbee_wake() {
	//kputs("XB Wake\n");
	MOTE_RX_RTSN = 0; // asserted
	MOTE_SLEEP = 0;
}

void xbee_poll_coordinator() {
	xbee_AT_set("FP",0,NULL);	
}

void xbee_tick() {
	unsigned int ch;
	
	while(1) {	
		ch = MOTE_UART_GETC();
		//printf_P(PSTR("%04X "),ch);
		if ( (ch & 0xFF00) == 0 || (ch & 0xFF00) == 0x1000 ) { // no errors, or framing error ( sometimes OK)
			_xbee_process_byte(ch);
		}
		else {
			break;
		}
	}		
}

void _xbee_frame_recieved(uint16_t nBytes, uint8_t * b) {
	/*
	kputs("\nXB RCV: ");
	for ( int c = 0; c < nBytes; c++)
		printf("%02X",b[c]);
	kputs("\n");
	*/
	
	if ( nBytes == 0 ) return;
	switch(b[0]) { // API ID byte
		case 0x88: // at command response
			if ( rx_at_cmd_status == XBEE_AT_WAITING ) {
				if ( rx_at_cmd_buf ) {
					for ( uint8_t c = 0; c < nBytes - 5; c++ )
						rx_at_cmd_buf[c] = b[c+5];
				}
				if(b[4]) { // error
					rx_at_cmd_status = -b[4];
				} else {
					rx_at_cmd_status = nBytes-5;
				}					
			}
			break;
		case 0x80: // rx 64b packet
			if(rx_callback) {
				xbee_64b_address addr_64;
				uint8_t * abytes = (uint8_t *)(&addr_64);
				abytes[7] = b[1];
				abytes[6] = b[2];
				abytes[5] = b[3];
				abytes[4] = b[4];
				abytes[3] = b[5];
				abytes[2] = b[6];
				abytes[1] = b[7];
				abytes[0] = b[8];
				uint8_t rssi = b[9];
				memcpy(rx_client_buf,b + 11,nBytes - 11);
				rx_callback(XBEE_UNKNOWN_16b_ADDR, addr_64, rssi, nBytes - 11);
			}
			break;
		case 0x81: // rx 16b packet
			if(rx_callback) {
				xbee_16b_address addr_16;
				uint8_t * abytes = (uint8_t *)(&addr_16);
				abytes[1] = b[1];
				abytes[0] = b[2];
				uint8_t rssi = b[3];
				memcpy(rx_client_buf,b + 5,nBytes - 5);
				rx_callback(addr_16, XBEE_UNKNOWN_64b_ADDR, rssi, nBytes - 5);
			}
			break;
		case 0x89: // tx resp
			if ( tx_callback ) {
				tx_callback(b[1],b[2]);
				tx_status = b[2];
			}
			break;
		case 0x8A:// modem status
			modem_status = b[2];
			status_callback(b[2]);
			break;
		default:
			printf_P(PSTR("API ID %02X not recognized or not implemented.\n"),b[0]);
	}
}

void xbee_AT_set(char cmd[2], uint8_t nBytes,uint8_t * val) {
	_xbee_start_API_frame();
	_xbee_load_API_byte(0x08);
	_xbee_load_API_byte(0x00); // frame ID is 0
	_xbee_load_API_byte(cmd[0]);
	_xbee_load_API_byte(cmd[1]);
	_xbee_load_API_bytes(nBytes, val);
	_xbee_send_API_frame();
}

int8_t _xbee_wait_for_AT_resp(uint8_t * buf) {
	rx_at_cmd_buf = buf;
	rx_at_cmd_status = XBEE_AT_WAITING;
	
	int16_t c = XBEE_AT_RESP_TIMEOUT;
	while (c--) {
		xbee_tick();
		if ( rx_at_cmd_status != XBEE_AT_WAITING ){
			return rx_at_cmd_status;
		}
		_delay_us(100);
	}
	return XBEE_AT_TIMEOUT;	
}

int8_t xbee_AT_set_resp(char cmd[2], uint8_t nBytes,uint8_t * val) {
	_xbee_start_API_frame();
	_xbee_load_API_byte(0x08);
	_xbee_load_API_byte(0x01); // frame ID is not 0
	_xbee_load_API_byte(cmd[0]);
	_xbee_load_API_byte(cmd[1]);
	_xbee_load_API_bytes(nBytes, val);
	_xbee_send_API_frame();
	
	return _xbee_wait_for_AT_resp(NULL);
}

// negative if error
int8_t xbee_AT_get(char cmd[2], uint8_t * buf) {
	//printf_P(PSTR("Xbee get %c%c\n"),cmd[0],cmd[1]);
	
	_xbee_start_API_frame();
	_xbee_load_API_byte(0x08);
	_xbee_load_API_byte(0x01); // frame ID is not 0
	_xbee_load_API_byte(cmd[0]);
	_xbee_load_API_byte(cmd[1]);
	_xbee_send_API_frame();
	
	return _xbee_wait_for_AT_resp(buf);
}


uint8_t xbee_send_packet_16( xbee_16b_address addr, uint16_t nBytes, uint8_t * data, uint8_t options) {

	if ( nBytes >= 100 ) {
		kputs("Maximum packet size exceeded\n");
		return 0;
	}
	_xbee_start_API_frame();
	_xbee_load_API_byte(0x01);
	uint8_t retval = frame_id;
	_xbee_load_API_byte(frame_id++);
	if ( !frame_id ) frame_id++; // reset to 1 if wraparound
	_xbee_load_API_byte((uint8_t)(addr>>8));
	_xbee_load_API_byte((uint8_t)(addr));
	_xbee_load_API_byte(options);
	_xbee_load_API_bytes(nBytes,data);
	_xbee_send_API_frame();
	return retval;
}
uint8_t xbee_send_packet_64( xbee_64b_address addr, uint16_t nBytes, uint8_t * data, uint8_t options) {

	if ( nBytes >= 100 ) {
		kputs("Maximum packet size exceeded\n");
		return 0;
	}
	
	_xbee_start_API_frame();
	_xbee_load_API_byte(0x00);
	uint8_t retval = frame_id;
	_xbee_load_API_byte(frame_id++);
	if ( !frame_id ) frame_id++; // reset to 1 if wraparound
	uint8_t * abytes = (uint8_t *)(&addr);
	/*
	printf("Send to:");
	for(int c =0 ;c < 8; c++)
	printf("%02X",abytes[7-c]);
	printf("\n");
	*/
	_xbee_load_API_byte(abytes[7]);
	_xbee_load_API_byte(abytes[6]);
	_xbee_load_API_byte(abytes[5]);
	_xbee_load_API_byte(abytes[4]);
	_xbee_load_API_byte(abytes[3]);
	_xbee_load_API_byte(abytes[2]);
	_xbee_load_API_byte(abytes[1]);
	_xbee_load_API_byte(abytes[0]);
	_xbee_load_API_byte(options);
	_xbee_load_API_bytes(nBytes,data);
	_xbee_send_API_frame();
	return retval;
}

uint8_t xbee_send_ipv4_packet( xbee_ipv4_address addr, uint16_t nBytes, uint8_t * data ) {
	if (nBytes > 1400) {
		kputs("Maximum packet size exceeded\n");
		return 0;
	}
	
	_xbee_start_API_frame();
	_xbee_load_API_byte(0x20);
	uint8_t retval = frame_id;
	_xbee_load_API_byte(frame_id++);
	if ( !frame_id ) frame_id++; // reset to 1 if wraparound
	uint8_t * abytes = (uint8_t *)(&addr);
	_xbee_load_API_byte(abytes[3]);
	_xbee_load_API_byte(abytes[2]);
	_xbee_load_API_byte(abytes[1]);
	_xbee_load_API_byte(abytes[0]);
	_xbee_load_API_byte(0x1D);	
	_xbee_load_API_byte(0x51); // remote port 7505
	_xbee_load_API_byte(0x1D);
	_xbee_load_API_byte(0x51); // source port 7505
	_xbee_load_API_byte(0);//UDP
	_xbee_load_API_byte(0); //ignored for UDP
	_xbee_load_API_bytes(nBytes,data);
	_xbee_send_API_frame();
	return retval;
}

uint8_t xbee_wait_for_send() {
	tx_status = 0xFF;
	int16_t timeout = XBEE_TX_RESP_TIMEOUT;
	while (tx_status == 0xFF && timeout--) {
		xbee_tick();
		_delay_us(100);
	}		
	return tx_status;
}


void xbee_set_tx_status_callback(void (*cb)(uint8_t frame_id, uint8_t status)) { tx_callback = cb;	}
void xbee_set_rx_callback(void (*cb)(xbee_16b_address addr_16b, xbee_64b_address addr_64b, uint8_t rssi, uint16_t nBytes), uint8_t * dest_buf) { rx_callback = cb; rx_client_buf = dest_buf; }
void xbee_set_modem_status_callback(void (*cb)(uint8_t status)) {status_callback = cb; }
uint8_t xbee_get_status() { return modem_status; }
void xbee_set_status(uint8_t stat){modem_status = stat;}

uint16_t xbee_tx_api_buf_len;
uint16_t xbee_tx_api_buf_pos;
uint8_t xbee_tx_api_buf[XBEE_TX_API_BUF_SIZE];
uint8_t xbee_tx_api_buf_checksum;

void _xbee_start_API_frame() {
	xbee_tx_api_buf_len = 0;
	xbee_tx_api_buf_pos = 3;
	xbee_tx_api_buf_checksum = 0xFF;
}

void _xbee_load_API_byte(uint8_t b) {
	if ( xbee_tx_api_buf_pos == XBEE_TX_API_BUF_SIZE ) 
	{
		kputs("XBEE API buffer overflow\n");
		return;
	}
	xbee_tx_api_buf_checksum -= b;
	if (b == 0x7E || b == 0x7D || b == 0x11 || b == 0x13) {
		xbee_tx_api_buf[xbee_tx_api_buf_pos++] = 0x7d;
		xbee_tx_api_buf[xbee_tx_api_buf_pos++] = b ^ 0x20;
	} else {
		xbee_tx_api_buf[xbee_tx_api_buf_pos++] = b;
	}
	xbee_tx_api_buf_len  += 1;
}

void _xbee_load_API_bytes(uint16_t nBytes, uint8_t * b) {
	while(nBytes--) {
		_xbee_load_API_byte(*b++);
	}
}

void _xbee_send_API_frame() {
	xbee_tx_api_buf[0] = 0x7E;
	xbee_tx_api_buf[1] = (uint8_t)((xbee_tx_api_buf_len >> 8) & 0xFF);
	xbee_tx_api_buf[2] = (uint8_t)((xbee_tx_api_buf_len) & 0xFF);
	xbee_tx_api_buf[xbee_tx_api_buf_pos] = xbee_tx_api_buf_checksum;
	
	int8_t cntr = XBEE_AT_RESP_TIMEOUT;
	if (modem_initialized) {
		 while(--cntr && MOTE_TX_CTSN_PIN) {
			 _delay_us(100);
		 } // wait for CTS to go low
		 if ( !cntr ) {
			 kputs("Timeout waiting for CTS to go low\n");
			 return;
		 }
	}
	
	MOTE_UART_WRITE(xbee_tx_api_buf_pos+1,xbee_tx_api_buf);
}

void _xbee_process_byte(uint8_t b) {
	//printf("%02X",b);
	if ( b == 0x7E ) { // restart reception no matter what ( since others are escaped )
		if ( xbee_rx_api_buf_pos ) {
			kputs("Warning, partial xbee API frame discarded.\n");
		}
		xbee_rx_api_buf[0] = 0x7E;
		xbee_rx_api_buf_pos = 1;
		esc_char_recv = 0;
	} else if ( xbee_rx_api_buf_pos == 0 ){ // ignore any characters not in a frame
	}
	else {
		if ( b == 0x7d ) {
			esc_char_recv = 1;
		} else {
			if ( esc_char_recv ) b ^= 0x20;
			esc_char_recv = 0;
			xbee_rx_api_buf[xbee_rx_api_buf_pos] = b;
			if ( xbee_rx_api_buf_pos == 1 ) { // length MSB
				xbee_rx_api_buf_len = ((uint16_t)b)<<8;
			} else if ( xbee_rx_api_buf_pos == 2 ) { // length LSB
				xbee_rx_api_buf_len |= ((uint16_t)b);
			}
			xbee_rx_api_buf_pos++;
			if ( xbee_rx_api_buf_pos >= 3 && xbee_rx_api_buf_pos >= xbee_rx_api_buf_len + 4 ) {// packet finished
				uint8_t checksum = 0;
				for (uint16_t c = 3; c < xbee_rx_api_buf_len + 4; c++ ) {
					checksum += xbee_rx_api_buf[c];
				}
				if ( checksum != 0xFF ) {
					kputs("XBee API checksum error\n");
				} else {
					_xbee_frame_recieved(xbee_rx_api_buf_len,xbee_rx_api_buf + 3);
				}
				xbee_rx_api_buf_pos = 0; // packet processing done
			}
		}
	}
}


void _xbee_read_rssi() {
	uint8_t buf[1];
	int8_t rssi;
	int8_t retval;
	if ( xbee_hw_series == 1 || xbee_hw_series == 2) {
		retval = xbee_AT_get("DB",buf);
		if ( retval < 0 ) {
			kputs("Couldn't read DB\n");
			return;
		}			
		rssi = -buf[0];
	} else if ( xbee_hw_series == 6 ) {
		retval = xbee_AT_get("LM",buf);
		if ( retval < 0) {
			kputs("Couldn't read LLM\n");
			return;
		}
		rssi = buf[0];
	} else {
		rssi = 0;
	}
	
	report_current()->rssi  = rssi;
	report_current()->fields |= REPORT_TYPE_RSSI;	
}

void xbee_setup_reporting_schedule(uint16_t start_time) {
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,MOTE_TASK_ID, start_time += 15, &xbee_poll_coordinator);// ensure that this happens before the report is sent
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,MOTE_TASK_ID, start_time += 50, &_xbee_read_rssi);// ensure that this happens before the report is sent
	
}

void xbee_fmt_reading(int8_t * reading,size_t bufsize,char * buf) {
	snprintf_P(buf,bufsize,PSTR("RSSI=%4ddBm"),*reading);
}
#endif