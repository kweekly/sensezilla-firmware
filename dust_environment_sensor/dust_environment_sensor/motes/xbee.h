/*
 * xbee.h
 *
 * Created: 4/1/2013 12:52:13 PM
 *  Author: kweekly
 */ 


#ifndef XBEE_H_
#define XBEE_H_

// assume serial port is already initialized
void xbee_init();

void xbee_AT_set(char cmd[2], uint8_t nBytes,uint8_t * val);
uint8_t xbee_AT_get(char cmd[2], uint8_t * buf);




#endif /* XBEE_H_ */