/*
 * xbee.c
 *
 * Created: 4/1/2013 12:52:02 PM
 *  Author: kweekly
 */ 

#include "all.h"

#define XBEE_TX_API_BUF_SIZE 128
#define XBEE_RX_API_BUF_SIZE 64
#define XBEE_AT_RESP_TIMEOUT 5 // in 10ms periods

uint8_t *rx_client_buf;
void (*rx_callback)(xbee_16b_address addr_16b, xbee_64b_address addr_64b, uint8_t rssi, uint16_t nBytes);
void (*tx_callback)(uint8_t frame_id, uint8_t status);
void (*status_callback)(uint8_t status);

uint8_t modem_status;
uint8_t frame_id;

uint16_t xbee_rx_api_buf_len;
uint16_t xbee_rx_api_buf_pos;
uint8_t xbee_rx_api_buf[XBEE_RX_API_BUF_SIZE];
uint8_t esc_char_recv;

uint8_t *rx_at_cmd_buf;
int8_t rx_at_cmd_status;

void xbee_init() {
	rx_client_buf = rx_callback = tx_callback = status_callback = NULL;
	modem_status = XBEE_STATUS_HW_RESET;
	frame_id = 1;
	esc_char_recv = xbee_rx_api_buf_len = xbee_rx_api_buf_pos = 0;
	rx_at_cmd_status = 0;
}

void xbee_tick() {
	unsigned int ch;
	
	while(1) {	
		ch = MOTE_UART_GETC();
		if ( ch & 0xFF00 == 0 ) { // no errors
			_xbee_process_byte(ch);
		}
		else {
			break;
		}
	}		
}

void _xbee_frame_recieved(uint16_t nBytes, uint8_t * b) {
	if ( nBytes == 0 ) return;
	switch(b[0]) { // API ID byte
		case 0x88: // at command response
			if ( rx_at_cmd_status == XBEE_AT_WAITING ) {
				for ( uint8_t c = 0; c < nBytes - 5; c++ )
					rx_at_cmd_buf[c] = b[c+5];
				rx_at_cmd_status = -b[4];
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
			}
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

// negative if error
int8_t xbee_AT_get(char cmd[2], uint8_t * buf) {
	rx_at_cmd_buf = buf;
	
	_xbee_start_API_frame();
	_xbee_load_API_byte(0x08);
	_xbee_load_API_byte(0x01); // frame ID is not 0
	_xbee_load_API_byte(cmd[0]);
	_xbee_load_API_byte(cmd[1]);
	_xbee_send_API_frame();
	rx_at_cmd_status = XBEE_AT_WAITING;
	
	uint8_t c = XBEE_AT_RESP_TIMEOUT;
	while (c--) {
		xbee_tick();
		if ( rx_at_cmd_status != XBEE_AT_WAITING ){
			return rx_at_cmd_status;
		}			
		_delay_ms(10);
	}
	return XBEE_AT_RESP_TIMEOUT;
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

void xbee_set_tx_status_callback(void (*cb)(uint8_t frame_id, uint8_t status)) { tx_callback = cb;	}
void xbee_set_rx_callback(void (*cb)(xbee_16b_address addr_16b, xbee_64b_address addr_64b, uint8_t rssi, uint16_t nBytes), uint8_t * dest_buf) { rx_callback = cb; rx_client_buf = dest_buf; }
void xbee_set_modem_status_callback(void (*cb)(uint8_t status)) {status_callback = cb; }
uint8_t xbee_get_status() { return modem_status; }


uint16_t xbee_tx_api_buf_len;
uint16_t xbee_tx_api_buf_pos;
uint8_t xbee_tx_api_buf[XBEE_TX_API_BUF_SIZE];

void _xbee_start_API_frame() {
	xbee_tx_api_buf_len = 0;
	xbee_tx_api_buf_pos = 3;
}

void _xbee_load_API_byte(uint8_t b) {
	if ( xbee_tx_api_buf_pos == XBEE_TX_API_BUF_SIZE ) 
	{
		kputs("XBEE API buffer overflow\n");
		return;
	}
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
	xbee_tx_api_buf[xbee_tx_api_buf_pos] = 0xFF;
	for ( uint16_t c = 3; c < xbee_tx_api_buf_pos; c++)
		xbee_tx_api_buf[xbee_tx_api_buf_pos] -= xbee_tx_api_buf[c];
	
	MOTE_UART_WRITE(xbee_tx_api_buf_pos+1,xbee_tx_api_buf);
}

void _xbee_process_byte(uint8_t b) {
	if ( b == 0x7E ) { // restart reception no matter what ( since others are escaped )
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
				for (uint16_t c = 3; c < xbee_rx_api_buf_len + 3; c++ ) {
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