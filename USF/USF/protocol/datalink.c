/*
 * data_link.c
 *
 * Created: 9/25/2013 11:50:04 AM
 *  Author: kweekly
 */ 
#include "devicedefs.h"
#include "avrincludes.h"
#include "protocol/datalink.h"



#if MOTE_TYPE == MOTE_XBEE
	#include "motes/xbee.h"
	xbee_64b_address dest64;
	xbee_16b_address dest16;
	xbee_ipv4_address destipv4;
	enum{
		NO_SEND,
		BROADCAST,
		SEND64,
		SEND16,
		WIFI_SEND
	} dest_mode;
#elif MOTE_TYPE == MOTE_WIFLY
	#include "motes/wifly.h"
#elif MOTE_TYPE == NONE
	void datalink_init(){}
	void datalink_wake(){}
	void datalink_send_packet_to_host(uint8_t * packetbuf, uint16_t len){}
	void datalink_latch_destination_address(){}
	void datalink_sleep(){}
	void datalink_setup_rssi_reporting(uint16_t starttime){}
	void datalink_fmt_reading(int8_t * reading,size_t bufsize,char * buf){}
#endif


// XBEE TRANSLATION LAYER
#if MOTE_TYPE == MOTE_XBEE
	// callbacks
	uint8_t rx_packet_buf[128];
	void xbee_status_cb(uint8_t status);
	void xbee_packet_recieved_cb(xbee_16b_address addr_16b, xbee_64b_address addr_64b, uint8_t rssi, uint16_t nBytes);
	void xbee_tx_status_cb(uint8_t frame_id, uint8_t status);
	void (*rx_cb)(uint8_t * packetbuf, uint16_t len);
	void (*rdy_cb)(void);

	void datalink_init() {
		if (xbee_get_type() == XBEE_TYPE_WIFI) {
			dest_mode = WIFI_SEND;
			} else {
			dest_mode = BROADCAST;
		}
		xbee_set_modem_status_callback(&xbee_status_cb);
		xbee_set_tx_status_callback(&xbee_tx_status_cb);
		xbee_set_rx_callback(&xbee_packet_recieved_cb,rx_packet_buf);		
	}
	
	void datalink_set_rx_callback(void (*r)(uint8_t * packetbuf, uint16_t len)) {
		rx_cb = r;
	}
	
	void datalink_set_ready_callback(void (*rcb)(void)) {
		rdy_cb = rcb;
	}

	xbee_16b_address last_rx_addr_16b;
	xbee_64b_address last_rx_addr_64b;

	void xbee_packet_recieved_cb(xbee_16b_address addr_16b, xbee_64b_address addr_64b, uint8_t rssi, uint16_t nBytes) {
		if ( nBytes == 0 ) return;
	
		last_rx_addr_16b = addr_16b;
		last_rx_addr_64b = addr_64b;
	 
		if ( rx_cb )
			rx_cb(rx_packet_buf,nBytes);
	}


	void xbee_tx_status_cb(uint8_t frame_id, uint8_t status){
		if ( status != 0 )
			printf_P(PSTR("Last TX message had error code %d\n"),status);
	}


	void xbee_status_cb(uint8_t status) {
		printf_P(PSTR("Modem status is now %d\n"),status);
	}
	
	
	void datalink_send_packet_to_host(uint8_t * packetbuf, uint16_t len) {
		int8_t resp;
		uint8_t txresp;
		uint8_t buf[1];
		xbee_tick();
		
		resp = xbee_AT_get("AI",buf);
		if ( resp < 0 ) {
			kputs("Radio not responding. (AI)\n");
			buf[0] = (xbee_get_status()!=XBEE_STATUS_ASSOC); // assume no change from before
		}
		
		redo_send:
		if (xbee_get_status() == XBEE_STATUS_ASSOC) {
			if ( buf[0] != 0 ) {
				printf_P(PSTR("Radio not ready, AI=%02X, correcting.\n"),buf[0]);
				xbee_set_status(XBEE_STATUS_DISASSOC);
				return;
			}
			if (dest_mode == BROADCAST) {
				xbee_send_packet_64(XBEE_BROADCAST_64b_ADDR,len,packetbuf,XBEE_TX_OPTION_BROADCAST_PAN);
				} else if ( dest_mode == SEND16 ){
				xbee_send_packet_16(dest16,len,packetbuf,0);
				} else if (dest_mode == SEND64) {
				xbee_send_packet_64(dest64,len,packetbuf,0);
				} else if (dest_mode == WIFI_SEND) {
				uint8_t ipbuf[4];
				resp = xbee_AT_get("MY",ipbuf);
				if ( resp < 0 ) {
					kputs("Radio not responding. (MY)\n");
					return;
				}
				printf_P(PSTR("My IP address=%d.%d.%d.%d\n"),ipbuf[0],ipbuf[1],ipbuf[2],ipbuf[3]);
				destipv4 = ((uint32_t)(ipbuf[0])<<24UL) | ((uint32_t)(ipbuf[1])<<16UL) | (((uint32_t)ipbuf[2])<<8UL) | 1;
				xbee_send_ipv4_packet(destipv4, len, packetbuf);
			}
			txresp = xbee_wait_for_send();
			//printf_P(PSTR("TX resp = %02X\n"),txresp);
			} else {
			if (buf[0] == 0) {
				printf_P(PSTR("Radio ready, but did not send status update, correcting.\n"));
				xbee_set_status(XBEE_STATUS_ASSOC);
				goto redo_send;
			}
			else printf_P(PSTR("Radio not ready to send packet AI=%02X\n"),buf[0]);
		}
		xbee_tick();
	}
	
	void datalink_latch_destination_address() {
		if ( last_rx_addr_16b != XBEE_UNKNOWN_16b_ADDR ) {
			dest_mode = SEND16;
			dest16 = last_rx_addr_16b;
		} else if ( last_rx_addr_64b != XBEE_UNKNOWN_64b_ADDR ) {
			dest_mode = SEND64;
			dest64 = last_rx_addr_64b;
		}
	}
	
	inline void datalink_wake() {		xbee_wake();	rdy_cb();  }
	inline void datalink_sleep() {		xbee_sleep();	}
	inline void datalink_tick() {		xbee_tick();	}
	inline void datalink_setup_rssi_reporting(uint16_t starttime) {		xbee_setup_reporting_schedule(starttime);	}
	inline void datalink_fmt_reading(int8_t * reading,size_t bufsize,char * buf) {		xbee_fmt_reading(reading, bufsize, buf);	}
	
	char xbeeuid[8];
	void datalink_get_ID(uint8_t ** uid_buf, uint8_t * uid_len) { *uid_buf = xbeeuid; *uid_len = sizeof(xbeeuid); }
#elif MOTE_TYPE==MOTE_WIFLY
	
	inline void datalink_init() {
		wifly_init();	
	}

	inline void datalink_send_packet_to_host(uint8_t * packetbuf, uint16_t len) {
		wifly_send_packet(packetbuf, len);
	}
	
	
	inline void datalink_set_rx_callback(void (*r)(uint8_t * packetbuf, uint16_t len)) {
		wifly_set_rx_callback(r);
	}
	
	inline void datalink_set_ready_callback(void (*rcb)(void)) {
		wifly_set_rdy_callback(rcb);
	}


	void datalink_latch_destination_address() {} // always connects to the same server (invalid)
	inline void datalink_wake() { 		wifly_wake();	}
	inline void datalink_sleep() {		wifly_sleep();	}
	inline void datalink_tick() {		wifly_tick();	}
	inline void datalink_setup_rssi_reporting(uint16_t starttime) {		wifly_setup_reporting_schedule(starttime);	}
	inline void datalink_fmt_reading(int8_t * reading,size_t bufsize,char * buf) {		wifly_fmt_reading(reading, bufsize, buf);	}	
	inline void datalink_get_ID(uint8_t ** uid_buf, uint8_t * uid_len) { wifly_get_ID(uid_buf, uid_len); }
#endif