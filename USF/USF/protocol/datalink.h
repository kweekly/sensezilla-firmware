/*
 * data_link.h
 
 * Manages a point-to-point link from mote to collector
 *
 * Created: 9/25/2013 11:49:53 AM
 *  Author: kweekly
 */ 


#ifndef DATALINK_H_
#define DATALINK_H_

typedef enum {
	DISCONNECTED = 0,
	DISASSOCIATED = 1,
	ASSOCIATED = 2
} datalink_status_t;

void datalink_init();
datalink_status_t datalink_status();
void datalink_send_packet_to_host(uint8_t * packetbuf, uint16_t len);
void datalink_set_rx_callback(void (*rx_cb)(uint8_t * packetbuf, uint16_t len));
void datalink_latch_destination_address();
void datalink_get_ID(uint8_t ** uid_buf, uint8_t * uid_len);

void datalink_set_ready_callback(void (*rdy_cb)(void));

void datalink_wake();
void datalink_sleep();
void datalink_tick();

// reporting
void datalink_setup_rssi_reporting(uint16_t starttime);
void datalink_fmt_reading(int8_t * reading,size_t bufsize,char * buf);

#endif /* DATA_LINK_H_ */