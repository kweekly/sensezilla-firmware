/*
 * logic.h
 *
 * Created: 7/10/2013 5:30:47 PM
 *  Author: kweekly
 */ 


#ifndef LOGIC_H_
#define LOGIC_H_
#include "motes/xbee.h"

void logic_init();
uint16_t report_interval_needed();

void task_led_blip_on(void);
void task_led_blip_off(void);

#define TASK_REPORTING 0x02
void task_begin_report(void);
void task_print_report(void);
void task_check_send_report(void);
void task_send_report(void);
void _send_packet_to_host(uint8_t * packetbuf, uint16_t len);
uint16_t _construct_report_packet(uint8_t * buf);

void rtc_timer_cb(void);

uint8_t rx_packet_buf[128];
void mote_status_cb(uint8_t status);
void mote_packet_recieved_cb(xbee_16b_address addr_16b, xbee_64b_address addr_64b, uint8_t rssi, uint16_t nBytes);
void mote_tx_status_cb(uint8_t frame_id, uint8_t status);

#ifdef USE_LIS3DH
void accel_click_cb(unsigned char detect_mask);
void accel_orientation_cb(unsigned char orientation);
#endif

#ifdef USE_PN532
void rfid_detection_cb(uint8_t * uid, uint8_t uidlen);
#endif

#ifdef USE_AMN41121
void pir_detection_cb(uint8_t occupied);
#endif

#ifdef USE_TSL2560
void light_detection_cb(uint8_t level);
#endif

void cmd_timesync_cb(uint32_t new_time);
void cmd_configure_sensor_cb(uint8_t mode, uint16_t fields_to_report, uint16_t sample_interval);
void cmd_actuate_cb(uint16_t fields_to_affect, uint8_t * actuation_data);

#endif /* LOGIC_H_ */