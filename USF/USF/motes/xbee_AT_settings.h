/*
 * xbee_AT_settings.h
 *
 * THIS FILE IS PRIVATE AND SHOULD NOT BE ENTERED INTO SOURCE CONTROL
 *
 * Created: 4/2/2013 3:03:06 PM
 *  Author: kweekly
 */ 


#ifndef XBEE_AT_SETTINGS_H_
#define XBEE_AT_SETTINGS_H_

const unsigned char XBEE_AT_SETTING_STR_S1[] PROGMEM =  
 "ID\x12\x34"
#ifdef XBEE_RTS_ENABLED
",D6\x1" // enable flow control RTS
#else
",D6\x0"
#endif
#ifdef XBEE_CTS_ENABLED
",D7\x1" // enable flow control RTS
#else
",D7\x0"
#endif
",CH\xC"
",MY\xFF\xFF"
",MM\x0"
",RR\x2"
",RN\x3"
",CE\x0"
",EE\x0"
",SM\x1" // pin hibernate
",SO\x2" // supress IO samples
",AP\x2"
",XX"
;

const unsigned char XBEE_AT_SETTING_STR_S2[] PROGMEM =
"ID\x12\x34"
#ifdef XBEE_RTS_ENABLED
",D6\x1" // enable flow control RTS
#else
",D6\x0"
#endif
#ifdef XBEE_CTS_ENABLED
",D7\x1" // enable flow control RTS
#else
",D7\x0"
#endif
",CH\xC"
",MY\xFF\xFF"
",MM\x0"
",RR\x2"
",RN\x3"
",CE\x0"
",EE\x0"
",SM\x1" // pin hibernate
",SO\x2" // supress IO samples
",AP\x2"
",XX"
; 

const unsigned char XBEE_AT_SETTING_STR_S6[] PROGMEM =
"PL\x0" // lowest power
",C0\x1D\x51" // Source port 7505
/*
",IDcrest-406" // SSID
",AH\x2" // Infrastructure mode
",IP\x0" // UDP
",MA\x0" // DHCP
",EE\x2" // WPA2 Encryption
",PKcrest406bubble"
*/
",IDAirBears"
",AH\x2"
",IP\x0"
",MA\x0"
",EE\x0"

",AO\x2" // RX64 (802.15.4-compatible RX packets)
#ifdef XBEE_RTS_ENABLED
",D6\x1" // enable flow control RTS
#else
",D6\x0"
#endif
#ifdef XBEE_CTS_ENABLED
",D7\x1" // enable flow control RTS
#else
",D7\x0"
#endif
",SM\x1" // pin hibernate
",XX"
;

#endif /* XBEE_AT_SETTINGS_H_ */