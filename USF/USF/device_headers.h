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
	#if HW_VERSION==1 || HW_VERSION==2
		#include "devices/si7005.h"
	#elif HW_VERSION==3
		#include "devices/htu21d.h"
	#endif
	#include "devices/tsl2560.h"
	#include "devices/door_sensors.h"
#elif defined(POWER_STRIP_MONITOR)
	#include "devices/CS5467.h"
	#include "devices/triacs.h"
#endif

#ifdef USE_PN532
	#include "devices/PN532.h"
#endif

#include "devices/k20.h"

#ifdef USE_MACHXO2_PMCO2
	#include "devices/MACHXO2_PMCO2.h"
#endif

#endif /* DEVICE_HEADERS_H_ */