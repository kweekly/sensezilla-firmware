/*
 * configurations.h
 *
 * Created: 9/18/2013 11:39:02 AM
 *  Author: kweekly
 */ 


#ifndef CONFIGURATIONS_H_
#define CONFIGURATIONS_H_


//#define CONFIG_DOORWAY_RFID_READER
//#define CONFIG_SERIAL_RFID_READER
#define CONFIG_BATTERY_SENSOR_DEVEL
//#define CONFIG_BATTERY_SENSOR_DEPLOY
//#define CONFIG_DOOR_OPEN_SENSOR
//#define CONFIG_CO2_SENSOR

#define MOTE_TYPE MOTE_WIFLY
/***************  ENVIRONMENT SENSOR ***********************/ 
/*
BODLEVEL = DISABLED
OCDEN = [ ]
JTAGEN = [X]
SPIEN = [X]
WDTON = [ ]
EESAVE = [ ]
BOOTSZ = 4096W_F000
BOOTRST = [ ]
CKDIV8 = [ ]
CKOUT = [ ]
SUT_CKSEL = INTRCOSC_6CK_0MS

EXTENDED = 0xFF (valid)
HIGH = 0x99 (valid)
LOW = 0xC2 (valid)

 */ 
// Uncomment this line to activate this board
#if defined(CONFIG_DOORWAY_RFID_READER) || defined(CONFIG_SERIAL_RFID_READER) || defined(CONFIG_BATTERY_SENSOR_DEVEL) || defined(CONFIG_BATTERY_SENSOR_DEPLOY) || defined(CONFIG_DOOR_OPEN_SENSOR) || defined(CONFIG_CO2_SENSOR)
	#define ENVIRONMENT_SENSOR
	#define HW_VERSION 2
#endif

#if defined(CONFIG_DOORWAY_RFID_READER) || defined(CONFIG_SERIAL_RFID_READER)
	#define USE_PN532
#endif

#if defined(CONFIG_CO2_SENSOR)
	#define USE_K20
	#define USE_SOFTSERIAL
#endif

#if defined(CONFIG_DOOR_OPEN_SENSOR)
	#define USE_DOOR_SENSORS
#endif

//#define USE_MACHXO2_PMCO2
/***************  POWER STRIP MONITOR (v2) *******************/ 
/* Fuse Settings:
Extended: FF
High : 91
Low D2
*/
//#define POWER_STRIP_MONITOR


// reporting
#if defined(CONFIG_DOORWAY_RFID_READER) || defined(CONFIG_BATTERY_SENSOR_DEVEL) || defined(CONFIG_BATTERY_SENSOR_DEPLOY) 
	#define DEFAULT_FIELDS_TO_REPORT 0x31F // all but gyro, light level change, RSSI
#elif defined(CONFIG_CO2_SENSOR)
	#define DEFAULT_FIELDS_TO_REPORT 0x31F // all but gyro, light level change, RSSI
#elif defined(CONFIG_DOOR_OPEN_SENSOR)
	#define DEFAULT_FIELDS_TO_REPORT 0xB1F // all but gyro, light level change, RSSI
#elif defined(CONFIG_SERIAL_RFID_READER)
	#define DEFAULT_FIELDS_TO_REPORT 0
#endif

// sample interval
#if defined(CONFIG_BATTERY_SENSOR_DEVEL)
#define DEFAULT_SAMPLE_INTERVAL 1
#define DEFAULT_RECORDSTORE_INTERVAL 12
#define USE_RECORDSTORE
#elif defined(CONFIG_BATTERY_SENSOR_DEPLOY) || defined(CONFIG_DOOR_OPEN_SENSOR)
#define DEFAULT_SAMPLE_INTERVAL 5
#define DEFAULT_RECORDSTORE_INTERVAL 150
#define USE_RECORDSTORE
#elif defined(CONFIG_DOORWAY_RFID_READER) || defined(CONFIG_CO2_SENSOR)
#define DEFAULT_SAMPLE_INTERVAL 5
#define DEFAULT_RECORDSTORE_INTERVAL 20
#define USE_RECORDSTORE
#elif defined(CONFIG_SERIAL_RFID_READER)
#define DEFAULT_SAMPLE_INTERVAL 65535
#endif

#if defined(CONFIG_DOORWAY_RFID_READER)
#define XBEE_MAX_RETRIES
#endif


#endif /* CONFIGURATIONS_H_ */