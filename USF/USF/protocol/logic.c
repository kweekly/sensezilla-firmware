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

#include "utils/scheduler.h"

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

uint16_t requested_report_interval;
uint32_t last_periodic_report_sent;
uint8_t using_monitor_list;

void logic_init() {
	if (xbee_get_type() == XBEE_TYPE_WIFI) {
		dest_mode = WIFI_SEND;
	} else {
		dest_mode = BROADCAST;
	}
	xbee_set_modem_status_callback(&mote_status_cb);
	xbee_set_tx_status_callback(&mote_tx_status_cb);
	xbee_set_rx_callback(&mote_packet_recieved_cb,rx_packet_buf);
		
	packet_set_handlers(&cmd_timesync_cb, &cmd_configure_sensor_cb, &cmd_actuate_cb);
}

uint16_t report_interval_needed() {
	if ( using_monitor_list ) {
		return 1;
	} else {
		return requested_report_interval;
	}	
}

void cmd_configure_sensor_cb(uint8_t mode, uint16_t fields_to_report, uint16_t sample_interval) {
	printf_P(PSTR("Configuring fields %02X => %02X, sample interval %ds\n"),report_fields_requested,fields_to_report,sample_interval);
	report_fields_requested = fields_to_report;
	requested_report_interval = sample_interval;
	last_periodic_report_sent = 0;
	using_monitor_list = 0;
	
	scheduler_reset(); //removes all tasks
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,MOTE_TASK_ID, 0, &xbee_wake);
	//scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,LED_BLIP_TASK_ID, 0, &task_led_blip_on);
	//scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,LED_BLIP_TASK_ID, 100, &task_led_blip_off);
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,TASK_REPORTING, 0, &task_begin_report);

	scheduler_add_task(SCHEDULER_MONITOR_LIST,TASK_REPORTING,0, &task_begin_report);
	
	#ifdef REPORT_TYPE_TEMP	
		if ( fields_to_report & (REPORT_TYPE_TEMP) )
			humid_setup_reporting_schedule(1);
		else 
			humid_sleep();
	#endif
	#ifdef REPORT_TYPE_OCCUPANCY
		if ( fields_to_report & (REPORT_TYPE_OCCUPANCY) ) {
			pir_wake(); // PIR must stay always-on to be effective
			pir_setup_reporting_schedule(1);
		}			
		else 
			if ( !(fields_to_report & (REPORT_TYPE_OCCUPANCY_CHANGED)) ) pir_sleep();
		
	#endif
	#ifdef REPORT_TYPE_LIGHT
		if ( fields_to_report & (REPORT_TYPE_LIGHT) )
			light_setup_reporting_schedule(1);
		else 
			if ( !(fields_to_report & (REPORT_TYPE_LIGHT_CHANGED)) )light_sleep();
	#endif
	#ifdef REPORT_TYPE_ACCEL
		if ( fields_to_report & (REPORT_TYPE_ACCEL) )
			accel_setup_reporting_schedule(5);
		else
			if ( !(fields_to_report & (REPORT_TYPE_ORIENTATION_CHANGED)) ) accel_sleep();
	#endif
	#ifdef REPORT_TYPE_GYRO
		if ( fields_to_report & (REPORT_TYPE_GYRO) )
			gyro_setup_reporting_schedule(1);
		else 
			gyro_sleep();
	#endif
	#ifdef REPORT_TYPE_OCCUPANCY_CHANGED
		if ( fields_to_report & (REPORT_TYPE_OCCUPANCY_CHANGED) ) {
			pir_setup_interrupt_schedule(0);
			using_monitor_list = 1;
		} else  {
			if ( !(fields_to_report & (REPORT_TYPE_OCCUPANCY)) ) pir_sleep();
		}		
	#endif
	#ifdef REPORT_TYPE_ORIENTATION_CHANGED
		if ( fields_to_report & (REPORT_TYPE_ORIENTATION_CHANGED) ) {
			accel_setup_interrupt_schedule(0);
			using_monitor_list = 1;
		} else {
			accel_configure_orientation_detection(ACCEL_ORIENTATION_ALL, NULL);
			if ( !(fields_to_report & (REPORT_TYPE_ACCEL ) )) accel_sleep();
		}		
	#endif
	#ifdef REPORT_TYPE_LIGHT_CHANGED
		if ( fields_to_report & (REPORT_TYPE_LIGHT_CHANGED) ) {
			light_setup_interrupt_schedule(0);
			using_monitor_list = 1;
		} else {
			if ( !(fields_to_report & (REPORT_TYPE_LIGHT) ) ) light_sleep();
		}
	#endif	
	#ifdef USE_DOOR_SENSORS
		if ( fields_to_report & (REPORT_TYPE_DOOR_SENSORS))
		door_sensors_setup_interrupt_schedule(1);
		else {
			scheduler_remove_tasks(SCHEDULER_MONITOR_LIST,DOOR_SENSOR_TASK_ID);
		}
	#endif
	
	#ifdef REPORT_TYPE_POWER_CH0 // should cover all the other channels
		if ( fields_to_report & (REPORT_TYPE_POWER_CH0) )
			powermon_setup_reporting_schedule(1);
		else {
			scheduler_remove_tasks(SCHEDULER_PERIODIC_SAMPLE_LIST, POWERMON_TASK_ID);
		}
	#endif
		
	#ifdef REPORT_TYPE_RSSI 
		if ( fields_to_report & (REPORT_TYPE_RSSI) )
			xbee_setup_reporting_schedule(1);
		else {
			scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST, MOTE_TASK_ID, 0, &xbee_wake);	
		}
	#endif
	
	#ifdef USE_PN532
		rfid_setup_interrupt_schedule(1, &rfid_detection_cb);
		using_monitor_list = 1;
	#endif
	
	#ifdef USE_MACHXO2_PMCO2
		machxo2_setup_reporting_schedule(1);
	#endif
	

	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,TASK_REPORTING, SCHEDULER_LAST_EVENTS, &task_print_report);
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,TASK_REPORTING, SCHEDULER_LAST_EVENTS, &task_send_report);
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,TASK_REPORTING, SCHEDULER_LAST_EVENTS, &report_poplast);
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,MOTE_TASK_ID, SCHEDULER_LAST_EVENTS, &xbee_sleep);
	
	scheduler_add_task(SCHEDULER_MONITOR_LIST,TASK_REPORTING, SCHEDULER_LAST_EVENTS, &task_print_report);
	scheduler_add_task(SCHEDULER_MONITOR_LIST,TASK_REPORTING, SCHEDULER_LAST_EVENTS, &task_check_send_report);
	scheduler_add_task(SCHEDULER_MONITOR_LIST,TASK_REPORTING, SCHEDULER_LAST_EVENTS, &report_poplast);
}


void rtc_timer_cb(void) {
	uint32_t curtime = rtctimer_read();
	if ( curtime < last_periodic_report_sent || curtime - last_periodic_report_sent  >= requested_report_interval ) {
		scheduler_start(SCHEDULER_PERIODIC_SAMPLE_LIST);
		last_periodic_report_sent = curtime;
	}
	if ( using_monitor_list ) {
		scheduler_start(SCHEDULER_MONITOR_LIST);
	}
}

void task_begin_report(void) {
	report_new(rtctimer_read());
}


void task_print_report(void) {
	if ( report_current()->fields != 0)
		report_print_human(report_current());
	//report_print(report_current());
}

void task_check_send_report(void) {
	if ( report_current()->fields != 0) {
		uint8_t packetbuf[128];
		xbee_wake();		
		uint16_t len = _construct_report_packet(packetbuf);
		_send_packet_to_host(packetbuf, len);
		xbee_sleep();
	}
}


void task_send_report(void) {
	uint8_t packetbuf[128];
	uint16_t len = _construct_report_packet(packetbuf);
	_send_packet_to_host(packetbuf, len);
}

uint16_t _construct_report_packet(uint8_t * buf) {
	uint16_t len = 0;
	report_t * report = report_current();
	len += packet_construct_sensor_data_header(report->time,report->fields,buf);
	len += report_populate_real_data(report,buf + len);
	return len;
}

void _send_packet_to_host(uint8_t * packetbuf, uint16_t len) {
#ifdef DISABLE_XBEE
	return;
#else
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
#endif
}

xbee_16b_address last_rx_addr_16b;
xbee_64b_address last_rx_addr_64b;

void mote_packet_recieved_cb(xbee_16b_address addr_16b, xbee_64b_address addr_64b, uint8_t rssi, uint16_t nBytes) {
	if ( nBytes == 0 ) return;
	
	last_rx_addr_16b = addr_16b;
	last_rx_addr_64b = addr_64b;
	
	/*
	printf_P(PSTR("RCV: "));
	for ( int c = 0; c < nBytes; c++ ) {
		printf("%02X",rx_packet_buf[c]);
	}
	printf_P(PSTR("\n"));
	*/
	
	packet_recieved(rx_packet_buf,nBytes);
}


void mote_tx_status_cb(uint8_t frame_id, uint8_t status){
	if ( status != 0 )
		printf_P(PSTR("Last TX message had error code %d\n"),status);
}


void mote_status_cb(uint8_t status) {
	printf_P(PSTR("Modem status is now %d\n"),status);
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


void cmd_actuate_cb(uint16_t fields_to_affect, uint8_t * actuation_data) {
	
}
#ifdef USE_PN532
void rfid_detection_cb(uint8_t * uid, uint8_t uidlen) {
	uint8_t buf[128];
	xbee_wake();
	printf_P(PSTR("t=%10ld UID: "),rtctimer_read());
	for(int c =0; c < uidlen; c++)
		printf_P(PSTR("%02X"),uid[c]);
	kputs("\n");
	
	// Beeper trigger
	spi_pause();
	EXP_CSN = 0;
	EXP_MOSI = 0;
	_delay_us(250);
	EXP_CSN = 1;
	EXP_MOSI = 1;
	spi_resume();
	
	uint16_t pos = packet_construct_RFID_detected_header(rtctimer_read(), buf);
	memcpy(buf+pos,uid,uidlen);
	_send_packet_to_host(buf,pos+uidlen);
	xbee_sleep();
}
#endif

#ifdef USE_AMN41121
void pir_detection_cb(uint8_t occupied) {
	
}
#endif

#ifdef USE_TSL2560
void light_detection_cb(uint8_t level) {
	printf_P(PSTR("Light level is now %d\n"),level);
}
#endif


#ifdef USE_LIS3DH
void accel_click_cb(unsigned char detect_mask) {
	
}

void accel_orientation_cb(unsigned char orientation) {
	
	
}
#endif


void task_led_blip_on(void) {
	LED1 = 1;
}

void task_led_blip_off(void) {
	LED1 = 0;
}