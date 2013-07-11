/*
 * sensor_packet.h
 *
 * Created: 7/10/2013 4:57:55 PM
 *  Author: kweekly
 */ 


#ifndef SENSOR_PACKET_H_
#define SENSOR_PACKET_H_

void packet_set_handlers(
	void (*timesync_cb)(uint32_t new_time),
	void (*configure_sensor_cb)(uint8_t mode, uint16_t fields_to_report, uint16_t sample_interval),
	void (*actuate_cb)(uint16_t fields_to_affect, uint8_t * actuation_data)
);

void packet_recieved(uint8_t * data,uint16_t packet_len);

uint16_t packet_construct_sensor_data_header(uint32_t timestamp, uint16_t fields, uint8_t * buffer_out);

#endif /* SENSOR_PACKET_H_ */