/*
 * logic.c
 *
 * Created: 7/10/2013 5:30:34 PM
 *  Author: kweekly
 */ 


#include "devicedefs.h"
#include "avrincludes.h"
#include "motes/xbee.h"
#include "protocol/report.h"
#include "protocol/sensor_packet.h"
#include "device_headers.h"

#include "drivers/uart.h"
#include "drivers/i2cmaster.h"
#include "drivers/rtctimer.h"
#include "drivers/pcint.h"
#include "drivers/SPI.h"
#include "drivers/power.h"

#include "logic.h"




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

void logic_init() {
	if (xbee_get_type() == XBEE_TYPE_WIFI) {
		dest_mode = WIFI_SEND;
	} else {
		dest_mode = BROADCAST;
	}
	xbee_set_modem_status_callback(&mote_status_cb);
	xbee_set_tx_status_callback(&mote_tx_status_cb);
	xbee_set_rx_callback(&mote_packet_recieved_cb,rx_packet_buf);
	
	accel_configure_click(&accel_click_cb);
	accel_configure_orientation_detection(ACCEL_ORIENTATION_ALL,&accel_orientation_cb);
	
	packet_set_handlers(&cmd_timesync_cb, &cmd_configure_sensor_cb, &cmd_actuate_cb);
}


void task_begin_report(void) {
	report_new(rtctimer_read());
}

void task_print_report(void) {
	report_print_human(report_current());
	// 	report_print(report_current());
}

uint16_t _construct_report_packet(uint8_t * buf) {
	uint16_t len = 0;
	report_t * report = report_current();
	len += packet_construct_sensor_data_header(report->time,report->fields,buf);
	len += report_populate_real_data(report,buf + len);
	return len;
}


void task_send_report(void) {
	uint8_t packetbuf[128];
	int8_t resp;
	xbee_tick();
	
	redo_send:
	if (xbee_get_status() == XBEE_STATUS_ASSOC) {
		uint16_t len = _construct_report_packet(packetbuf);
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
				kputs("Radio not responding");
				return;
			}
			printf_P(PSTR("My IP address=%d.%d.%d.%d\n"),ipbuf[0],ipbuf[1],ipbuf[2],ipbuf[3]);
			destipv4 = ((uint32_t)(ipbuf[0])<<24UL) | ((uint32_t)(ipbuf[1])<<16UL) | (((uint32_t)ipbuf[2])<<8UL) | 1;
			xbee_send_ipv4_packet(destipv4, len, packetbuf);
		}
	} else {
		uint8_t buf[1];
		resp = xbee_AT_get("AI",buf);
		if ( resp < 0 ) {
			kputs("Radio not responding.");
			return;
		}
		if (buf[0] == 0) {
			printf_P(PSTR("Radio ready, but did not send status update, correcting.\n"));
			xbee_set_status(XBEE_STATUS_ASSOC);
			goto redo_send;
		}
		else printf_P(PSTR("Radio not ready to send packet AI=%02X\n"),buf[0]);
	}
	xbee_tick();
}

xbee_16b_address last_rx_addr_16b;
xbee_64b_address last_rx_addr_64b;

void mote_packet_recieved_cb(xbee_16b_address addr_16b, xbee_64b_address addr_64b, uint8_t rssi, uint16_t nBytes) {
	if ( nBytes == 0 ) return;
	
	last_rx_addr_16b = addr_16b;
	last_rx_addr_64b = addr_64b;
	packet_recieved(rx_packet_buf,nBytes);
}


void mote_tx_status_cb(uint8_t frame_id, uint8_t status){
	printf_P(PSTR("Last TX message had error code %d\n"),status);
}


void mote_status_cb(uint8_t status) {
	printf_P(PSTR("Modem status is now %d\n"),status);
}

void accel_click_cb(unsigned char detect_mask) {
	
}

void accel_orientation_cb(unsigned char orientation) {
	
	
}

void cmd_timesync_cb(uint32_t new_time) {
	printf_P(PSTR("Time is now: %ld\n"),new_time);
	rtctimer_write(new_time);
	if ( last_rx_addr_16b != XBEE_UNKNOWN_16b_ADDR ) {
		dest_mode = SEND16;
		dest16 = last_rx_addr_16b;
	} else if ( last_rx_addr_64b != XBEE_UNKNOWN_64b_ADDR ) {
		dest_mode = SEND64;
		dest64 = last_rx_addr_64b;
	}
}
	
void cmd_configure_sensor_cb(uint8_t mode, uint16_t fields_to_report, uint16_t sample_interval) {
	
}

void cmd_actuate_cb(uint16_t fields_to_affect, uint8_t * actuation_data) {
	
}
