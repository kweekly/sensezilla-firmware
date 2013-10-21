/*
 * sensor_packet.c
 *
 * Created: 7/10/2013 4:58:05 PM
 *  Author: kweekly
 */ 

#include "devicedefs.h"

#include "avrincludes.h"
#include "protocol/sensor_packet.h"


#define MT_TIMESYNC					0x00
#define MT_SENSOR_DATA				0x01
#define MT_CONFIGURE_SENSOR			0x02
#define MT_ACTUATE					0x03
#define MT_RFID_DETECTION			0x04
#define MT_RECORDSTORE				0x05
#define MT_DEVICE_ID				0x06

#if defined ENVIRONMENT_SENSOR
	#define MY_BT BT_DUST_ENVIRONMENT_SENSOR
#elif defined POWER_STRIP_MONITOR
	#define MY_BT BT_POWER_STRIP_MONITOR
#endif

void (*timesync_cb)(uint32_t new_time);
void (*configure_sensor_cb)(uint8_t mode, uint16_t fields_to_report, uint16_t sample_interval);
void (*actuate_cb)(uint16_t fields_to_affect, uint8_t * actuation_data);


void packet_set_handlers(
void (*a)(uint32_t new_time),
void (*b)(uint8_t mode, uint16_t fields_to_report, uint16_t sample_interval),
void (*c)(uint16_t fields_to_affect, uint8_t * actuation_data)
) {
	timesync_cb = a;
	configure_sensor_cb = b;
	actuate_cb = c;
}

void packet_recieved(uint8_t * data,uint16_t packet_len) {
	uint8_t BT = data[0]; // board type
	uint8_t MT = data[1]; // message type
	if ( BT != BT_HOST ) return; // don't take anything from non-host soruce
	switch(MT) {
		case MT_TIMESYNC:
			if (timesync_cb)
				timesync_cb(*(uint32_t *)(&(data[2])));
			break;
		case MT_CONFIGURE_SENSOR:
			if (timesync_cb)
				configure_sensor_cb(data[2], *(uint16_t*)(data+3), *(uint16_t*)(data+5));
			break;
		case MT_ACTUATE:
			if (timesync_cb)
				actuate_cb(*(uint16_t*)(data+2),data+4);
			break;		
	}
}

uint16_t packet_construct_sensor_data_header_bt(uint8_t bt, uint32_t timestamp, uint16_t fields, uint8_t * buffer_out) {
	buffer_out[0] = MY_BT;
	buffer_out[1] = MT_SENSOR_DATA;
	*(uint32_t*)(buffer_out+2) = timestamp;
	*(uint16_t*)(buffer_out+6) = fields;
	return 8;
}

uint16_t packet_construct_sensor_data_header(uint32_t timestamp, uint16_t fields, uint8_t * buffer_out) {
	return packet_construct_sensor_data_header_bt(MY_BT, timestamp, fields, buffer_out);
}

uint16_t packet_construct_device_id_header(uint8_t device_id_type, uint8_t * buffer_out) {
	buffer_out[0] = MY_BT;
	buffer_out[1] = MT_DEVICE_ID;
	buffer_out[2] = device_id_type;
	return 3;
}

#ifdef USE_PN532
uint16_t packet_construct_RFID_detected_header(uint32_t timestamp, uint8_t * buffer_out) {
	buffer_out[0] = MY_BT;
	buffer_out[1] = MT_RFID_DETECTION;
	*(uint32_t*)(buffer_out+2) = timestamp;
	return 6;
}
#endif


#ifdef USE_RECORDSTORE
uint16_t packet_construct_recordstore_header(uint8_t * buffer_out) {
	buffer_out[0] = MY_BT;
	buffer_out[1] = MT_RECORDSTORE;
	return 2;
}
#endif