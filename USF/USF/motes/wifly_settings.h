/*
 * wifly_settings.h
 *
 * Created: 9/25/2013 1:51:21 PM
 *  Author: kweekly
 */ 


#ifndef WIFLY_SETTINGS_H_
#define WIFLY_SETTINGS_H_

#if MOTE_TYPE == MOTE_WIFLY

const unsigned char WIFLY_SETTINGS_STR[] PROGMEM =
	"set s p 0\n" // printlevel 0 ( no messages to interfere with data )
	"set c t 10\n" // set comm timer to 10ms
#ifdef LOW_POWER
	"set s a 10\n" //automatically sleep 100ms after UDP transmission
#else
	"set s a 0\n" // turn off sleep timer
#endif	
	"set s t 2\n" // trigger wake on CTS (RTS on our board)
	"set i d 3\n"	// use dhcp with cache
	"set i f 78\n" // set udp autopairing, arp and dns caching, udp retry
	"set i p 0\n" // UDP mode
	"set w s crest-406\n"
	"set w p crest406bubble\n"
	"set w t 1\n" // set to 1dBm (out of 1-12)
	;
#endif
#endif /* WIFLY_SETTINGS_H_ */