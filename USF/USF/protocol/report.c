/*
 * report.c
 *
 * Created: 3/19/2013 4:13:53 PM
 *  Author: kweekly
 */ 
#include "devicedefs.h"

#include "avrincludes.h"
#include "protocol/report.h"
#include "protocol/datalink.h"
/*
typedef struct {
	uint32_t time;
	uint16_t fields;
	
	humid_reading_t temphumid;
	float  occupancy;
	light_reading_t light;
	accel_reading_t accel;
	gyro_reading_t gyro;
} report_t;
*/


uint16_t report_fields_requested;

uint16_t report_populate_real_data(report_t * rep, uint8_t * buf) {
	uint8_t * oldbuf = buf;
	float * fltptr = (float*)(buf);
	
	/**************** ENV SENSOR *******************/
	#ifdef REPORT_TYPE_TEMP
	if ( rep->fields & REPORT_TYPE_TEMP)  // also should have humidity
		fltptr += humid_convert_real(&rep->temphumid,fltptr);	
	#endif
	
	#ifdef REPORT_TYPE_OCCUPANCY	
	if ( rep->fields & REPORT_TYPE_OCCUPANCY) 
		*(fltptr++) = rep->occupancy * 100.0;
	#endif
	
	#ifdef REPORT_TYPE_LIGHT
	if ( rep->fields & REPORT_TYPE_LIGHT)
		fltptr += light_convert_real(&rep->light,fltptr);
	#endif
	
	#ifdef REPORT_TYPE_ACCEL
	if ( rep->fields & REPORT_TYPE_ACCEL)
		fltptr += accel_convert_real(&rep->accel,fltptr);
	#endif
	
	#ifdef REPORT_TYPE_GYRO
	if ( rep->fields & REPORT_TYPE_GYRO)
		fltptr += gyro_convert_real(&rep->gyro,fltptr);
	#endif
	
	
	/**************** POWERMON *******************/
	#ifdef REPORT_TYPE_POWER_CH0
	if ( rep->fields & REPORT_TYPE_POWER_CH0 )
		fltptr += powermon_convert_real(rep->power,fltptr);
	#endif
	
	
	/**************** COMMON *******************/
	
	#ifdef REPORT_TYPE_RSSI	
	if ( rep->fields & REPORT_TYPE_RSSI ) {
		*((int8_t *)fltptr) = rep->rssi;
		fltptr = (float*)(((int8_t *)fltptr) + 1);
	}		
	#endif
	
	#ifdef REPORT_TYPE_OCCUPANCY_CHANGED
	if ( rep->fields & REPORT_TYPE_OCCUPANCY_CHANGED ) {
		*((int8_t *)fltptr) = rep->occupancy_state;
		fltptr = (float*)(((int8_t *)fltptr) + 1);
	}		
	#endif
	
	#ifdef REPORT_TYPE_ORIENTATION_CHANGED
	if ( rep->fields & REPORT_TYPE_ORIENTATION_CHANGED ) {
		*((int8_t *)fltptr) = rep->orientation;
		fltptr = (float*)(((int8_t *)fltptr) + 1);
	}		
	#endif
	
	#ifdef REPORT_TYPE_LIGHT_CHANGED
	if ( rep->fields & REPORT_TYPE_LIGHT_CHANGED ) {
		*((int8_t *)fltptr) = rep->light_level_state;
		fltptr = (float*)(((int8_t *)fltptr) + 1);
	}		
	#endif
	
	#ifdef REPORT_TYPE_DOOR_SENSORS
	if ( rep->fields & REPORT_TYPE_DOOR_SENSORS ) {
		*((int8_t *)fltptr) = rep->door_sensor_state;
		fltptr = (float*)(((int8_t *)fltptr) + 1);
	}
	#endif
	
	#ifdef REPORT_TYPE_RFID_COUNT
	if ( rep->fields & REPORT_TYPE_RFID_COUNT ) {
		*((int8_t *)fltptr) = rep->rfid_count;
		fltptr = (float*)(((int8_t *)fltptr) + 1);
	}
	#endif
	
	return (uint16_t)((uint8_t *)fltptr - oldbuf);
}

void report_print_human(report_t * rep) {
	char buf[200];
	printf_P(PSTR("t=%10ld"),rep->time);
	
	/**************** ENV SENSOR *******************/
	#ifdef REPORT_TYPE_TEMP
	if ( rep->fields & REPORT_TYPE_TEMP) {
		humid_fmt_reading(&(rep->temphumid),sizeof(buf),buf);
		uart_puts_P("; ");
		uart_puts(buf);
	}		
	#endif
	
	#ifdef REPORT_TYPE_OCCUPANCY
	if ( rep->fields & REPORT_TYPE_OCCUPANCY) {
		pir_fmt_reading(&(rep->occupancy),sizeof(buf),buf);
		uart_puts_P("; ");
		uart_puts(buf);	
	}
	#endif
	
	#ifdef REPORT_TYPE_LIGHT
	if ( rep->fields & REPORT_TYPE_LIGHT) {
		light_fmt_reading(&(rep->light),sizeof(buf),buf);
		uart_puts_P("; ");
		uart_puts(buf);
	}	
	#endif
	
	#ifdef REPORT_TYPE_ACCEL
	if ( rep->fields & REPORT_TYPE_ACCEL){
		accel_fmt_reading(&(rep->accel),sizeof(buf),buf);
		uart_puts_P("; ");
		uart_puts(buf);
	}
	#endif
	
	#ifdef REPORT_TYPE_GYRO
	if ( rep->fields & REPORT_TYPE_GYRO){
		gyro_fmt_reading(&(rep->gyro),sizeof(buf),buf);
		uart_puts_P("; ");
		uart_puts(buf);
	}
	#endif
	
	/**************** POWERMON *******************/
	#ifdef REPORT_TYPE_POWER_CH0
	if ( rep->fields & REPORT_TYPE_POWER_CH0 )
		powermon_fmt_reading(rep->power,sizeof(buf),buf);
		uart_puts_P("; ");
		uart_puts(buf);
	#endif
		
	/**************** COMMON *******************/
	#ifdef REPORT_TYPE_RSSI	
	if ( rep->fields & REPORT_TYPE_RSSI) {
		datalink_fmt_reading(&(rep->rssi),sizeof(buf),buf);
		uart_puts_P("; ");
		uart_puts(buf);
	}
	#endif
	
	#ifdef REPORT_TYPE_OCCUPANCY_CHANGED
	if ( rep->fields & REPORT_TYPE_OCCUPANCY_CHANGED ) {
		printf_P(PSTR("; Occupancy=%d"),rep->occupancy_state);
	}
	#endif
	
	#ifdef REPORT_TYPE_ORIENTATION_CHANGED
	if ( rep->fields & REPORT_TYPE_ORIENTATION_CHANGED ) {
		printf_P(PSTR("; Orientation=%02X"),rep->orientation);
	}
	#endif
		
	#ifdef REPORT_TYPE_LIGHT_CHANGED
	if ( rep->fields & REPORT_TYPE_LIGHT_CHANGED ) {
		printf_P(PSTR("; Light detect=%d"),rep->light_level_state);
	}
	#endif
	
	#ifdef REPORT_TYPE_DOOR_SENSORS
	if ( rep->fields & REPORT_TYPE_DOOR_SENSORS ) {
		door_sensors_fmt_reading(rep->door_sensor_state,sizeof(buf),buf);
		uart_puts_P("; ");
		uart_puts(buf);
	}
	#endif
	
	#ifdef REPORT_TYPE_RFID_COUNT
		if ( rep->fields & REPORT_TYPE_RFID_COUNT ) {
			printf_P(PSTR("; RFID count=%d"),rep->rfid_count);
		}
	#endif
	
	printf_P(PSTR("\n"));
}

#define MAX_REPORTS 32

report_t report_list[MAX_REPORTS];
uint8_t report_list_insert;
uint8_t report_list_remove;
int8_t report_current_index;

void report_init() {
	report_list_insert = report_list_remove = 0;
	report_current_index = -1;
}

void report_new(uint32_t time) {
	uint8_t next_idx = (report_list_insert + 1)%MAX_REPORTS;
	if ( next_idx == report_list_remove ) {
		kputs("Reports overflow! Removing last report.\n");
		report_poplast();
	}
	report_list[report_list_insert].time = time;
	report_list[report_list_insert].fields = 0;
	report_current_index = report_list_insert;
	report_list_insert = next_idx;
}


report_t * report_current() {
	if  (report_current_index < 0) {
		//kputs("Error: tried to get current report, but there is none.\n")
		return NULL;
	}
	return &(report_list[report_current_index]);
}

report_t * report_last() {
	if ( report_list_remove == report_list_insert ) {
		return NULL;
	}
	return &(report_list[report_list_remove]);
}

void report_poplast() {
	if ( report_list_remove == report_list_insert ) {
		kputs("Reports underflow!\n");
		return;
	}
	if (report_list_remove == report_current_index)
		report_current_index = -1;
		
	report_list_remove = (report_list_remove + 1)%MAX_REPORTS;
}