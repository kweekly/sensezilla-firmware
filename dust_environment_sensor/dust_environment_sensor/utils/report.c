/*
 * report.c
 *
 * Created: 3/19/2013 4:13:53 PM
 *  Author: kweekly
 */ 

#include "all.h"

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


void report_print(report_t * rep) {
	printf_P(PSTR("t=%8d"),rep->time);
	
	if ( rep->fields & REPORT_TYPE_TEMP)
		printf_P(PSTR(" temp=%04X"),rep->temphumid.temperature);
		
	if ( rep->fields & REPORT_TYPE_HUMID)
		printf_P(PSTR(" humid=%04X"),rep->temphumid.humidity);
		
	if ( rep->fields & REPORT_TYPE_OCCUPANCY)
		printf_P(PSTR(" occupancy=%08X"),*((uint16_t *)(&rep->occupancy)));
		
	if ( rep->fields & REPORT_TYPE_LIGHT)
		printf_P(PSTR(" light0=%04X light1=%04X"),rep->light.ch0,rep->light.ch1);
			
	if ( rep->fields & REPORT_TYPE_ACCEL)
		printf_P(PSTR(" ax=%04d ay=%04d az=%04d"),rep->accel.X,rep->accel.Y,rep->accel.Z);
		
	if ( rep->fields & REPORT_TYPE_GYRO)
		printf_P(PSTR(" gx=%04X gy=%04X gz=%04X"),rep->gyro.X,rep->gyro.Y,rep->gyro.Z);
		
	printf_P(PSTR("\n"));
}

void report_print_human(report_t * rep) {
	char buf[64];
	printf_P(PSTR("t=%8d"),rep->time);
	
	if ( rep->fields & REPORT_TYPE_TEMP) {
		humid_fmt_reading(&(rep->temphumid),sizeof(buf),buf);
		uart_putc(' ');
		uart_puts(buf);
	}		
	
	if ( rep->fields & REPORT_TYPE_OCCUPANCY) {
		pir_fmt_reading(&(rep->occupancy),sizeof(buf),buf);
		uart_putc(' ');
		uart_puts(buf);	
	}
	
	if ( rep->fields & REPORT_TYPE_LIGHT) {
		light_fmt_reading(&(rep->light),sizeof(buf),buf);
		uart_putc(' ');
		uart_puts(buf);
	}	
	
	if ( rep->fields & REPORT_TYPE_ACCEL){
		accel_fmt_reading(&(rep->accel),sizeof(buf),buf);
		uart_putc(' ');
		uart_puts(buf);
	}
	
	if ( rep->fields & REPORT_TYPE_GYRO){
		gyro_fmt_reading(&(rep->gyro),sizeof(buf),buf);
		uart_putc(' ');
		uart_puts(buf);
	}
	
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


