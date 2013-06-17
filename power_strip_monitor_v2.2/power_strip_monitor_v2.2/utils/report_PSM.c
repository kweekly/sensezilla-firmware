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
	
	powermon_reading_t power;
} report_t;
*/


void report_print(report_t * rep) {
	printf_P(PSTR("t=%10ld"),rep->time);
	
		
	/* EXAMPLE
	if ( rep->fields & REPORT_TYPE_LIGHT)
		printf_P(PSTR(" light0=%04X light1=%04X"),rep->light.ch0,rep->light.ch1);
		*/

		
	printf_P(PSTR("\n"));
}

uint16_t report_populate_real(report_t * rep, uint8_t * buf) {
	uint8_t * oldbuf = buf;
	*(uint32_t *)buf = rep->time;
	*(uint16_t *)(buf += 4) = rep->fields;
	float * fltptr = (float*)(buf += 2);

	
	/* EXAMPLE
	if ( rep->fields & REPORT_TYPE_LIGHT)
		fltptr += light_convert_real(&rep->light,fltptr);
	*/
	
	if ( rep->fields & REPORT_TYPE_POWER ) 
		fltptr += powermon_convert_real(&rep->power,fltptr);
	
	return (uint16_t)((uint8_t *)fltptr - oldbuf);
}

void report_print_human(report_t * rep) {
	char buf[1024];
	printf_P(PSTR("t=%10ld"),rep->time);
	
	/* EXAMPLE
	if ( rep->fields & REPORT_TYPE_LIGHT) {
		light_fmt_reading(&(rep->light),sizeof(buf),buf);
		uart_putc(' ');
		uart_puts(buf);
	}	
	*/
	
	if ( rep->fields & REPORT_TYPE_POWER ) {
		powermon_fmt_reading(&(rep->power),sizeof(buf),buf);
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