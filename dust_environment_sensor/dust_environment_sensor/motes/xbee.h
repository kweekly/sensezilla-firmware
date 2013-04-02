/*
 * xbee.h
 *
 * Created: 4/1/2013 12:52:13 PM
 *  Author: kweekly
 */ 


#ifndef XBEE_H_
#define XBEE_H_

#define XBEE_BROADCAST_64b_ADDR 0x000000000000FFFF
#define XBEE_BROADCAST_16b_ADDR 0xFFFF
#define XBEE_UNKNOWN_64b_ADDR	0x000000000000FFFE
#define XBEE_UNKNOWN_16b_ADDR	0xFFFE
typedef uint16_t xbee_16b_address;
typedef uint64_t xbee_64b_address;

// assume serial port is already initialized
void xbee_init();
void xbee_tick();

void xbee_AT_set(char cmd[2], uint8_t nBytes,uint8_t * val);

#define XBEE_AT_ERROR -1
#define XBEE_AT_INVALID_CMD -2
#define XBEE_AT_INVALID_PARAM -3
#define XBEE_AT_TIMEOUT -4
#define XBEE_AT_WAITING -10
int8_t xbee_AT_get(char cmd[2], uint8_t * buf);

// returns frame identifier
#define XBEE_TX_OPTION_DISABLE_ACK 0x01
#define XBEE_TX_OPTION_BROADCAST_PAN 0x04
uint8_t xbee_send_packet_16( xbee_16b_address addr, uint16_t nBytes, uint8_t * data, uint8_t options);
uint8_t xbee_send_packet_64( xbee_64b_address addr, uint16_t nBytes, uint8_t * data, uint8_t options);

#define XBEE_TX_STATUS_SUCCESS	0
#define XBEE_TX_STATUS_NO_ACK	1
#define XBEE_TX_STATUS_CCA_FAIL 2
#define XBEE_TX_STATUS_PURGED	3
void xbee_set_tx_status_callback(void (*cb)(uint8_t frame_id, uint8_t status));

void xbee_set_rx_callback(void (*cb)(xbee_16b_address addr_16b, xbee_64b_address addr_64b, uint8_t rssi, uint16_t nBytes), uint8_t * dest_buf);

#define XBEE_STATUS_HW_RESET		0
#define XBEE_STATUS_WDT_RESET		1
#define XBEE_STATUS_ASSOC			2
#define XBEE_STATUS_DISASSOC		3
#define XBEE_STATUS_SNYC_LOST		4
#define XBEE_STATUS_COORD_REALIGN	5
#define XBEE_STATUS_COORD_STARTED	6
void xbee_set_modem_status_callback(void (*cb)(uint8_t status));
uint8_t xbee_get_status();

void _xbee_frame_recieved(uint16_t nBytes, uint8_t * b);

// helper API routines
void _xbee_start_API_frame();
void _xbee_load_API_byte(uint8_t b);
void _xbee_load_API_bytes(uint16_t nBytes, uint8_t * b);
void _xbee_send_API_frame();

void _xbee_process_byte(uint8_t b);

#endif /* XBEE_H_ */