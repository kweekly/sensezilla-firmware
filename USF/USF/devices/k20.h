/*
 * k20.h
 *
 * Created: 10/23/2013 4:39:18 PM
 *  Author: kweekly
 */ 


#ifndef K20_H_
#define K20_H_

void k20_init();

void k20_setup_reporting_schedule(uint16_t starttime);
void _k20_reporting_startread();
void _k20_reporting_checkread();

#endif /* K20_H_ */