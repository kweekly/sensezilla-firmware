/*
 * sensor_packet.h
 *
 * Created: 7/10/2013 4:57:55 PM
 *  Author: kweekly
 */ 


#ifndef SENSOR_PACKET_H_
#define SENSOR_PACKET_H_


#define BT_HOST						0x00
#define BT_POWER_STRIP_MONITOR		0x01
#define BT_DUST_ENVIRONMENT_SENSOR	0x02
#define BT_PM_SENSOR				0x03
#define BT_MACHXO2_PMCO2			0x04

#define CONFIGURE_FIELDS_TO_REPORT 0x01
#define CONFIGURE_SAMPLE_INTERVAL 0x02
#define CONFIGURE_RECORDSTORE_INTERVAL 0x04

void packet_set_handlers(
	void (*timesync_cb)(uint32_t new_time),
	void (*configure_sensor_cb)(uint8_t mode, uint16_t fields_to_report, uint16_t sample_interval, uint16_t recordstore_interval),
	void (*actuate_cb)(uint16_t fields_to_affect, uint8_t * actuation_data)
);

uint8_t packet_recieved(uint8_t * data,uint16_t packet_len);

uint16_t packet_construct_sensor_data_header(uint32_t timestamp, uint16_t fields, uint8_t * buffer_out);
uint16_t packet_construct_sensor_data_header_bt(uint8_t bt, uint32_t timestamp, uint16_t fields, uint8_t * buffer_out);

#define DEVID_TYPE_BINARY		0x00
#define DEVID_TYPE_MAC_80211	0x01
#define DEVID_TYPE_ASCII		0x40
#define DEVID_TYPE_ASCII_TYPE_NUMBER	0x41
uint16_t packet_construct_device_id_header(uint8_t device_id_type, uint8_t * buffer_out);

uint16_t packet_construct_current_configuration(uint16_t fields_to_report, uint16_t sample_interval, uint16_t recordstore_interval, uint8_t * buffer_out);

#ifdef USE_RECORDSTORE
uint16_t packet_construct_recordstore_header(uint8_t * buffer_out);
#endif

#ifdef USE_PN532
uint16_t packet_construct_RFID_detected_header(uint32_t timestamp, uint8_t * buffer_out);
#endif

#endif /* SENSOR_PACKET_H_ */