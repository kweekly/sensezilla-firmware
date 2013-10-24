/*
 * wifly.h
 *
 * Created: 9/25/2013 12:47:17 PM
 *  Author: kweekly
 */ 


#ifndef WIFLY_H_
#define WIFLY_H_


void wifly_init();
void wifly_wake();
void wifly_sleep();
void wifly_tick();
void wifly_setup_reporting_schedule(uint16_t starttime);
void wifly_fmt_reading(int8_t * reading,size_t bufsize,char * buf);
void wifly_flush();
void wifly_send_packet(uint8_t * buf, uint16_t len);
void wifly_set_rx_callback(void (*rx_cb)(uint8_t * buf, uint16_t len));
void wifly_set_rdy_callback(void (*rdy_cb)(void));

void wifly_get_ID(uint8_t ** uid_buf, uint8_t * uid_len);

void _wifly_TCP_connect();
void _wifly_get_MAC();

uint8_t _wifly_readline();
void _wifly_pcint_cb();

uint8_t wifly_exec_command(uint8_t * cmd, uint16_t len);

#endif /* WIFLY_H_ */