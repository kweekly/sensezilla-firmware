/*
 * device_headers.h
 *
 * Created: 7/3/2013 4:05:09 PM
 *  Author: kweekly
 */ 


#ifndef DEVICE_HEADERS_H_
#define DEVICE_HEADERS_H_

#include "devicedefs.h"

#if defined(ENVIRONMENT_SENSOR)
	#include "devices/amn41121.h" 
	#include "devices/l3gd20.h"
	#include "devices/lis3dh.h"
	#include "devices/si7005.h"
	#include "devices/tsl2560.h"
#elif defined(POWER_STRIP_MONITOR)
	#include "devices/CS5467.h"
#endif

#ifdef USE_PN532
	#include "devices/PN532.h"
#endif
#endif /* DEVICE_HEADERS_H_ */