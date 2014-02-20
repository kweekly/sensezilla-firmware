/*
 * logic.c
 *
 * Created: 7/10/2013 5:30:34 PM
 *  Author: kweekly
 */ 


#include "devicedefs.h"
#include "avrincludes.h"
#include "protocol/report.h"
#include "protocol/sensor_packet.h"
#include "protocol/datalink.h"
#include "device_headers.h"
#include "protocol/recordstore.h"

#include "drivers/uart.h"
#include "drivers/i2cmaster.h"
#include "drivers/rtctimer.h"
#include "drivers/pcint.h"
#include "drivers/SPI.h"
#include "drivers/power.h"

#include "utils/scheduler.h"

#include "logic.h"


uint16_t requested_report_interval;
uint32_t last_periodic_report_taken;
uint8_t using_monitor_list;
uint8_t invalid_packets_recieved;
uint8_t first_init;

#ifdef USE_RECORDSTORE
uint16_t requested_recordstore_interval;
uint32_t last_recordstore_sent;
#endif

#define ERROR_RETRY_INTERVAL	60
#define ERROR_NUM_RETRIES		3
uint8_t num_retries_remaining;
uint32_t last_retry_tried;

void logic_init() {		
	datalink_set_rx_callback(&datalink_rx_callback);
	packet_set_handlers(&cmd_timesync_cb, &cmd_configure_sensor_cb, &cmd_actuate_cb);
	
	datalink_set_ready_callback(&_datalink_rdy_cb);
#ifdef USE_RECORDSTORE
	recordstore_init();
#endif

	first_init = 0;
}

uint16_t report_interval_needed() {
	if ( using_monitor_list ) {
		return 1;
	} else {
		return requested_report_interval;
	}
}

void datalink_rx_callback(uint8_t * data, uint16_t len) {
	kputs("Recieved: ");
	for ( int d = 0; d < len; d++ ) {
		printf_P(PSTR("%02X"),data[d]);
	}
	kputs("\n");
	if (packet_recieved(data,len)) {
		invalid_packets_recieved = 0;
	} else {
		invalid_packets_recieved++;
		if ( invalid_packets_recieved >= 3 ) {
			kputs("Too many invalid packets recieved, resetting...\n");
			datalink_reset();
		}
	}
}

void cmd_configure_sensor_cb(uint8_t mode, uint16_t fields_to_report, uint16_t sample_interval, uint16_t recordstore_interval) {
	uint8_t fields_changed = 0;
	printf_P(PSTR("Configuring sensor\n"));
	if (mode & CONFIGURE_FIELDS_TO_REPORT && (report_fields_requested != fields_to_report || !first_init)) {
		printf_P(PSTR("\tfields %02X => %02X\n"),report_fields_requested,fields_to_report);
		report_fields_requested = fields_to_report;
		fields_changed = 1;
	}
	if (mode & CONFIGURE_SAMPLE_INTERVAL && (requested_report_interval != sample_interval || !first_init)) {
		printf_P(PSTR("\tsample interval %d => %d\n"),requested_report_interval,sample_interval);
		requested_report_interval = sample_interval;
		last_periodic_report_taken = -1;
	}
	if ( mode & CONFIGURE_RECORDSTORE_INTERVAL && (requested_recordstore_interval != recordstore_interval || first_init)) {
		printf_P(PSTR("\trecordstore interval %d => %d\n"),requested_recordstore_interval,recordstore_interval);
		requested_recordstore_interval = recordstore_interval;
		last_recordstore_sent = -1;
	}
	first_init = 1;
	using_monitor_list = 0;	
	
	if (!fields_changed ) return;
	
	scheduler_reset(); //removes all tasks
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,LED_BLIP_TASK_ID, 0, &task_led_blip_on);
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
	
		
	#ifdef REPORT_TYPE_CO2
		if ( fields_to_report & (REPORT_TYPE_CO2)) {
			k20_init();
			#ifdef USE_SOFTSERIAL
			k20_setup_reporting_schedule(250);
			#else
			k20_setup_reporting_schedule(100);
			#endif
		} 
	#endif
	
	#ifdef USE_DOOR_SENSORS
		if ( fields_to_report & (REPORT_TYPE_DOOR_SENSORS)) {
			door_sensors_setup_interrupt_schedule(1);
			door_sensors_setup_reporting_schedule(1);
		}
		else {
			scheduler_remove_tasks(SCHEDULER_MONITOR_LIST,DOOR_SENSOR_TASK_ID);
			scheduler_remove_tasks(SCHEDULER_PERIODIC_SAMPLE_LIST,DOOR_SENSOR_TASK_ID);
		}
	#endif
	#ifdef USE_PN532
		if ( fields_to_report & REPORT_TYPE_RFID_COUNT)
			rfid_setup_report_schedule(1);
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
			datalink_setup_rssi_reporting(1);
	#endif
	
	#ifdef USE_PN532
		rfid_setup_interrupt_schedule(1, &rfid_detection_cb);
		using_monitor_list = 1;
	#endif
	
	#ifdef USE_MACHXO2_PMCO2
		machxo2_setup_reporting_schedule(1);
	#endif
	
	#ifndef USE_RECORDSTORE
		scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST, MOTE_TASK_ID, 0, &datalink_wake);
	#endif

	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,TASK_REPORTING, SCHEDULER_LAST_EVENTS, &task_print_report);
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,TASK_REPORTING, SCHEDULER_LAST_EVENTS, &task_send_report);
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,TASK_REPORTING, SCHEDULER_LAST_EVENTS, &report_poplast);
	#ifndef USE_RECORDSTORE
		scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,MOTE_TASK_ID, SCHEDULER_LAST_EVENTS, &datalink_sleep);
	#endif
	
	scheduler_add_task(SCHEDULER_MONITOR_LIST,TASK_REPORTING, SCHEDULER_LAST_EVENTS, &task_print_report);
	scheduler_add_task(SCHEDULER_MONITOR_LIST,TASK_REPORTING, SCHEDULER_LAST_EVENTS, &task_check_send_report);
	scheduler_add_task(SCHEDULER_MONITOR_LIST,TASK_REPORTING, SCHEDULER_LAST_EVENTS, &report_poplast);
	
	scheduler_add_task(SCHEDULER_PERIODIC_SAMPLE_LIST,LED_BLIP_TASK_ID, SCHEDULER_LAST_EVENTS, &task_led_blip_off);
}


void rtc_timer_cb(void) {
	uint32_t curtime = rtctimer_read();
	if ( curtime < last_periodic_report_taken || curtime - last_periodic_report_taken  >= requested_report_interval ) {
		scheduler_start(SCHEDULER_PERIODIC_SAMPLE_LIST);
		last_periodic_report_taken = curtime;
	}
	#ifdef USE_RECORDSTORE
	if ( curtime < last_recordstore_sent || curtime - last_recordstore_sent >= requested_recordstore_interval ) {
		datalink_wake();
		last_recordstore_sent = curtime;
		num_retries_remaining = ERROR_NUM_RETRIES;
		last_retry_tried = curtime;
	}
	#endif
	
	if (num_retries_remaining > 0 && curtime - last_retry_tried >= ERROR_RETRY_INTERVAL && requested_recordstore_interval > 2*ERROR_RETRY_INTERVAL) {
		printf_P(PSTR("Retrying transmission %d retries remaining.\n"),num_retries_remaining-1);
		datalink_wake();
		last_retry_tried = curtime;
		num_retries_remaining--;		
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
}

void task_check_send_report(void) {
	if ( report_current()->fields != 0) {
		uint8_t packetbuf[128];
		uint16_t len = _construct_report_packet(packetbuf);
	#ifdef USE_RECORDSTORE
		recordstore_insert(packetbuf,len);
	#else
		datalink_wake();
		_delay_ms(250);
		datalink_send_packet_to_host(packetbuf, len);
		datalink_sleep();
	#endif
	}
}

void _datalink_rdy_cb() {
	send_over_datalink();
	num_retries_remaining = 0;
}

void send_over_datalink(void) {
	uint16_t len;
	uint8_t len8;
	uint8_t * uidbuf;
	uint8_t pbuf[32];

	#ifdef USE_RECORDSTORE
		datalink_get_ID(&uidbuf,&len8);
		len = packet_construct_device_id_header(DEVID_TYPE_MAC_80211, pbuf);
		memcpy(pbuf+len,uidbuf,len8);
		recordstore_insert(pbuf,len8+len);
		
		len = packet_construct_current_configuration(report_fields_requested,requested_report_interval,requested_recordstore_interval, pbuf);
		recordstore_insert(pbuf,len);

		uint8_t * packet = recordstore_dump(&len);
		datalink_send_packet_to_host(packet,len);
		recordstore_clear();
	#endif
	len = 500;
	do {
		datalink_tick(); // take care of any packets sent by host
		_delay_ms(1);
	} while (len--);
	datalink_sleep();	
}


void task_send_report(void) {
	uint8_t packetbuf[128];
	uint16_t len = _construct_report_packet(packetbuf);
#ifdef USE_RECORDSTORE
	recordstore_insert(packetbuf,len);
#else
	datalink_send_packet_to_host(packetbuf, len);
#endif
}

uint16_t _construct_report_packet(uint8_t * buf) {
	uint16_t len = 0;
	report_t * report = report_current();
	len += packet_construct_sensor_data_header(report->time,report->fields,buf);
	len += report_populate_real_data(report,buf + len);
	return len;
}



void cmd_timesync_cb(uint32_t new_time) {
	printf_P(PSTR("Time is now: %ld\n"),new_time);
	last_periodic_report_taken = new_time;
#ifdef USE_RECORDSTORE
	last_recordstore_sent = new_time;
	recordstore_clear(); 
#endif
	rtctimer_write(new_time);
	datalink_latch_destination_address();
}


void cmd_actuate_cb(uint16_t fields_to_affect, uint8_t * actuation_data) {
	
}
#ifdef USE_PN532
void rfid_detection_cb(uint8_t * uid, uint8_t uidlen) {
	uint8_t buf[128];
	datalink_wake();
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
	#ifdef USE_RECORDSTORE
		recordstore_insert(buf,pos+uidlen);
	#else
		datalink_send_packet_to_host(buf,pos+uidlen);
		datalink_sleep();
	#endif
	
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
	#ifdef LED2
		LED2 = 1;
	#endif
}

void task_led_blip_off(void) {
	#ifdef LED2
		LED2 = 0;
	#endif
}