/*
 * report.h
 *
 * Created: 3/19/2013 4:14:01 PM
 *  Author: kweekly
 */
#ifndef _REPORT_H
#define _REPORT_H

#include "all.h"

#define REPORT_NUM_TYPES		1

#define REPORT_TYPE_POWER		0x01

typedef struct {
	uint32_t time;
	uint16_t fields;
	
	powermon_reading_t power;
} report_t;


void report_init();
void report_new(uint32_t time);
void report_print(report_t * rep);
void report_print_human(report_t * rep);
uint16_t report_populate_real(report_t * rep, uint8_t * buf);
report_t * report_current();
void report_poplast();

#endif