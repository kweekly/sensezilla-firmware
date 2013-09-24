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
	",D7\x1" // enable flow control CTS
#else
	",D7\x0"
#endif
",D5\x0" // associated indicator off
",CH\xC"
",MY\xFF\xFF"
",MM\x0"
#ifdef XBEE_MAX_RETRIES
",RR\x6"
#else
",RR\x2"
#endif
",RN\x0"
",CE\x0"
",EE\x0"
#if defined(XBEE_PINSLEEP_ENABLED) && defined(LOW_POWER)
	",SM\x1" // pin hibernate
	//",A1\xA" // poll coordinator for data, can reassign channel, no attempt to associate
	",A1\xE" // poll coordinator for data, device attempts association
#else
	",SM\x0"
	",A1\xE" // poll coordinator for data, device attempts association
#endif
",SP\x01\x90" // 20s before host discards message
",ST\x1F8" //500ms before sleep
",SO\x2" // supress IO samples
",CA\x32" // CCA threshold -50dBm
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
	",D7\x1" // enable flow control CTS
#else
	",D7\x0"
#endif
",D5\x0" // associated indicator off
",CH\xC"
",MY\xFF\xFF"
",MM\x0"
#ifdef XBEE_MAX_RETRIES
",RR\x6"
#else
",RR\x2"
#endif
",RN\x0"
",CE\x0" 
",EE\x0"
#if defined(XBEE_PINSLEEP_ENABLED) && defined(LOW_POWER)
	",SM\x1" // pin hibernate
	",A1\xC" // poll coordinator for data, device attempts association
#else
	",SM\x0"
	",A1\x8" // only check for data, do not attempt to associate
#endif
",ST\xFA" // 250ms wake time
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
#if defined(XBEE_PINSLEEP_ENABLED) && defined(LOW_POWER)
	",SM\x1" // pin hibernate
#else
	",SM\x0"
#endif
",ST\xFA" // 250ms wake time
",XX"
;

#endif /* XBEE_AT_SETTINGS_H_ */