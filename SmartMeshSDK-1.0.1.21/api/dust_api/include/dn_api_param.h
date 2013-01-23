/*
 * Copyright (c) 2010, Dust Networks.  All rights reserved.
 *
 * $HeadURL: https://subversion/software/trunk/shared/include/6lowpan/dn_api_param.h $
 * $Rev: 41760 $
 * $Author: yzats $
 * $LastChangedDate: 2010-11-10 18:01:38 -0800 (Wed, 10 Nov 2010) $ 
 */

#ifndef _DN_API_PARAM_H
#define _DN_API_PARAM_H

#include "dn_typedef.h"
#include "dn_api_common.h"
#include "dn_mesh.h"

PACKED_START

//*************************************************************************************
//                               setParam, getParam                                  //
//*************************************************************************************

// setParam <DN_API_PARAM_MACADDR>
typedef struct {
   uint8_t         paramId;
   dn_macaddr_t  macAddr; 
}   dn_api_set_macaddr_t;
typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_macaddr_t;

// getParam <DN_API_PARAM_MACADDR>
typedef dn_api_get_hdr_t  dn_api_get_macaddr_t;
typedef struct {
   uint8_t     rc;
   uint8_t     paramId;
   dn_macaddr_t  macAddr;
}   dn_api_rsp_get_macaddr_t;


// setParam <DN_API_PARAM_JOINKEY>
typedef struct {
   uint8_t      paramId;
   dn_key_t   joinKey; 
}   dn_api_set_joinkey_t;
typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_joinkey_t;


// setParam <DN_API_PARAM_NETID>
typedef struct {
   uint8_t paramId;
   dn_netid_t netId; 
}   dn_api_set_netid_t;

typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_netid_t;


// getParam <DN_API_PARAM_NETID>
typedef dn_api_get_hdr_t dn_api_get_netid_t;

typedef struct {
   uint8_t rc;
   uint8_t paramId;
   dn_netid_t netId; 
}   dn_api_rsp_get_netid_t;


// setParam <DN_API_PARAM_TXPOWER>
typedef struct {
   uint8_t   paramId;
   INT8S   txPower;     //in dBm
}   dn_api_set_txpower_t;

typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_txpower_t;

// getParam <DN_API_PARAM_TXPOWER>
typedef dn_api_get_hdr_t dn_api_get_txpower_t;
typedef struct {
   uint8_t     rc;
   uint8_t     paramId;
   INT8S     txPower;     //in dBm
}   dn_api_rsp_get_txpower_t;

// setParam <DN_API_PARAM_EVENTMASK>
typedef struct {
   uint8_t   paramId;
   uint32_t  eventMask;
}   dn_api_set_eventmask_t;

typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_eventmask_t;

// getParam <DN_API_PARAM_EVENTMASK>
typedef dn_api_get_hdr_t dn_api_get_eventmask_t;
typedef struct {
   uint8_t   rc;
   uint8_t   paramId;
   uint32_t eventMask; 
}   dn_api_rsp_get_eventmask_t;


// getParam <DN_API_PARAM_MOTEINFO>
typedef dn_api_get_hdr_t dn_api_get_moteinfo_t;

typedef struct {
   uint8_t  rc;
   uint8_t  paramId;
   uint8_t  apiVersion;       /* this protocol version */
   uint8_t  serialNumber[DN_SERNUM_SIZE];  /* Dust IEEE addr in OTP==serial # */
   uint8_t  hwModel;          /* Hardware model (HW_MODEL_OSKI) */
   uint8_t  hwRev;            /* Hardware revision (maps to (cal.icVer | cal.sipVer)) */
   dn_api_swver_t swVer;      /* software version*/
   uint8_t  blSwVer;          /* bootloader: sw version */
}  dn_api_rsp_get_moteinfo_t;


// getParam <DN_API_PARAM_NETINFO>
typedef dn_api_get_hdr_t dn_api_get_netinfo_t;

typedef struct {
   uint8_t  rc;
   uint8_t  paramId;
   dn_macaddr_t      macAddr;  // mac addr used by mote
   dn_moteid_t       moteId;   // short address assigned to the mote
   dn_netid_t        netId;    // network id
   uint16_t            slotSize; // slot Size in usec
}  dn_api_rsp_get_netinfo_t;


// getParam <DN_API_PARAM_MOTESTATUS>
typedef dn_api_get_hdr_t dn_api_get_motestatus_t;

typedef struct {
   uint8_t    rc;
   uint8_t    paramId;
   uint8_t    state;
   uint8_t    stateReason;
   uint16_t   reserved_1;
   uint8_t    numParents;
   uint32_t   alarms;   //Bitmap of current alarms
   uint8_t    reserved_2;
}   dn_api_rsp_get_motestatus_t;

// getParam <DN_API_PARAM_TIME>
typedef dn_api_get_hdr_t dn_api_get_time_t;

typedef struct {
   uint8_t    rc;
   uint8_t    paramId;
   uint32_t         upTime;
   dn_utc_time_t  utcTime;             
   dn_asn_t       asn;
   uint16_t         offset;    //ASN offset by micro-seconds
}   dn_api_rsp_get_time_t;


//getParam <DN_API_PARAM_CHARGE>
typedef dn_api_get_hdr_t dn_api_get_charge_t;

typedef struct {
   uint8_t    rc;
   uint8_t    paramId;
   uint32_t   qTotal;    //charge in milicoulombs since last reset
   uint32_t   upTime;    //mote up time since the last reset (in seconds)
   INT8S    tempInt;   //the integral part in temperature (in Celsius)
   uint8_t    tempFrac;  //the fractional part in temperature (in 1/255 of C)
}   dn_api_rsp_get_charge_t;


//getParam <DN_API_PARAM_TESTRADIORXSTATS>
typedef dn_api_get_hdr_t dn_api_get_rfrxstats_t;
typedef struct {
   uint8_t    rc;
   uint8_t    paramId;
   uint16_t   rxOkCnt;      //# of packets received OK
   uint16_t   rxFailCnt;    //# of failed packet reeceptions
}  dn_api_rsp_get_rfrxstats_t;


// setParam <DN_API_PARAM_MACMICKEY>
typedef struct {
   uint8_t      paramId;   
   dn_key_t  key;      // MAC MIC key
}   dn_api_set_macmickey_t;

typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_macmickey_t;



// setParam <DN_API_PARAM_SHORTADDR>
typedef struct {
   uint8_t        paramId;
   dn_moteid_t shortAddr;     //0x0001~0xFFFE
}   dn_api_set_shortaddr_t;

typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_shortaddr_t;

// getParam <DN_API_PARAM_SHORTADDR>
typedef dn_api_get_hdr_t dn_api_get_shortaddr_t;

typedef struct {
   uint8_t    rc;
   uint8_t    paramId;
   dn_moteid_t shortAddr;     //0x0001~0xFFFE
}   dn_api_rsp_get_shortaddr_t;

// setParam <DN_API_PARAM_IP6ADDR>
typedef struct {
   uint8_t            paramId;
   dn_ipv6_addr_t  ipAddr;
}   dn_api_set_ip6addr_t;

typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_ip6addr_t;

// getParam <DN_API_PARAM_IP6ADDR>
typedef dn_api_get_hdr_t dn_api_get_ip6addr_t;

typedef struct {
   uint8_t    rc;
   uint8_t    paramId;
   dn_ipv6_addr_t ipAddr;
}   dn_api_rsp_get_ip6addr_t;


// setParam <DN_API_PARAM_CCAMODE>
typedef struct {
   uint8_t   paramId;
   uint8_t   mode;   
}   dn_api_set_ccamode_t;

typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_ccamode_t;

// setParam <DN_API_PARAM_PATHALARMGEN>
typedef struct {
   uint8_t   paramId;
   uint8_t   isGenerate; // !=0 generate path alarm, otherwise don't generate
}   dn_api_set_pathalarmgen_t;

typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_pathalarmgen_t;

// setParam <DN_API_PARAM_CHANNELS>
typedef struct {
   uint8_t   paramId;
   uint16_t  bitmap; /* Bit0=channel 0, Bit 15= channel 15;
                      0=do not use; 1=use channel */
}   dn_api_set_channels_t;

typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_channels_t;


// setParam <DN_API_PARAM_ADVGRAPH>
typedef struct {
   uint8_t   paramId;
   uint8_t   advGraph;  //Graph id to advertise
}   dn_api_set_advgraph_t;

typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_advgraph_t;


// setParam <DN_API_PARAM_HRTMR>
typedef struct {
   uint8_t   paramId;
   uint16_t  hrTimer;  //in seconds; 0=disabled
}   dn_api_set_hrtimer_t;

typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_hrtimer_t;


// setParam <DN_API_PARAM_APPINFO>
typedef struct {
   uint8_t        paramId;  
   uint16_t       vendorId;
   uint16_t        appId;
   dn_api_swver_t appVer;   //App Version   
}   dn_api_set_appinfo_t;

typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_appinfo_t;

// getParam <DN_API_PARAM_HRTMR>
typedef dn_api_get_hdr_t dn_api_get_hrTimer_t;

typedef struct {
   uint8_t rc;
   uint8_t paramId;
   uint16_t  hrTimer;  //in seconds; 0=disabled
}   dn_api_rsp_get_hrTimer_t;


// getParam <DN_API_PARAM_APPINFO>
typedef dn_api_get_hdr_t dn_api_get_appinfo_t;

typedef struct {
   uint8_t rc;
   uint8_t paramId;
   uint16_t       vendorId;
   uint16_t       appId;
   dn_api_swver_t appVer;   //App Version   
}   dn_api_rsp_get_appinfo_t;


// getParam <DN_API_PARAM_PWRCOSTINFO>
typedef dn_api_get_hdr_t dn_api_get_pwrcostinfo_t;

typedef struct {
   uint8_t   rc;
   uint8_t   paramId;
   dn_api_pwrcostinfo_t info;
}   dn_api_rsp_get_pwrcostinfo_t;


// getParam <DN_API_PARAM_SIZEINFO>
typedef dn_api_get_hdr_t dn_api_get_sizeinfo_t;

typedef struct {   
   uint8_t     rc;
   uint8_t     paramId;
   uint8_t     maxFrames;
   uint16_t    maxLinks;
   uint8_t     maxNbrs;
   uint8_t     maxRoutes;
   uint8_t     maxGraphs;
   uint8_t     maxQSize;
}   dn_api_rsp_get_sizeinfo_t;

// setParam <DN_API_PARAM_JOINDUTYCYCLE>
typedef struct {
   uint8_t   paramId;
   uint8_t   dutyCycle;  //in 1/255th of 100% 1-255
}   dn_api_set_dutycycle_t;
typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_dutycycle_t;

// getParam <DN_API_PARAM_JOINDUTYCYCLE>
typedef dn_api_get_hdr_t dn_api_get_dutycycle_t;
typedef struct {
   uint8_t     rc;
   uint8_t     paramId;
   uint8_t     dutyCycle;
}   dn_api_rsp_get_dutycycle_t;


// setParam <DN_API_PARAM_OTAPLOCKOUT>
typedef struct {
   uint8_t   paramId;
   uint8_t   mode;       // DN_API_OTAP_LOCKOUT_MODE_xxx
}   dn_api_set_otaplockout_t;
typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_otaplockout_t;

// getParam <DN_API_PARAM_OTAPLOCKOUT>
typedef dn_api_get_hdr_t dn_api_get_otaplockout_t;
typedef struct {
   uint8_t     rc;
   uint8_t     paramId;
   uint8_t     mode;       // DN_API_OTAP_LOCKOUT_MODE_xxx
}   dn_api_rsp_get_otaplockout_t;



// setParam <DN_API_PARAM_ROUTINGMODE>
typedef struct {
   uint8_t   paramId;
   uint8_t   mode;  // DN_API_ROUTING_MODE_xxx 
}   dn_api_set_rtmode_t;
typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_rtmode_t;

// getParam <DN_API_PARAM_ROUTINGMODE>
typedef dn_api_get_hdr_t dn_api_get_rtmode_t;
typedef struct {
   uint8_t   rc;
   uint8_t   paramId;
   uint8_t   mode;  // DN_API_ROUTING_MODE_xxx
}   dn_api_rsp_get_rtmode_t;


// setParam <DN_API_PARAM_PWRSRCINFO>
typedef struct {
   uint8_t   paramId;
   dn_api_pwrsrcinfo_t info; 
}   dn_api_set_pwrsrcinfo_t;
typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_pwrsrcinfo_t;

// getParam <DN_API_PARAM_PWRSRCINFO>
typedef dn_api_get_hdr_t dn_api_get_pwrsrcinfo_t;
typedef struct {
   uint8_t   rc;
   uint8_t   paramId;
   dn_api_pwrsrcinfo_t info;
}   dn_api_rsp_get_pwrsrcinfo_t;


// setParam <DN_API_PARAM_MOBILITY>
typedef struct {
   uint8_t   paramId;
   uint8_t   type;  //Mobility type 
}   dn_api_set_mobility_t;
typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_mobility_t;

// getParam <DN_API_PARAM_MOBILITY>
typedef dn_api_get_hdr_t dn_api_get_mobility_t;
typedef struct {
   uint8_t   rc;
   uint8_t   paramId;
   uint8_t   type;  //Mobility type
}   dn_api_rsp_get_mobility_t;


// setParam <DN_API_PARAM_ADVKEY>
typedef struct {
   uint8_t     paramId;
   dn_key_t  key;
}   dn_api_set_advkey_t;
typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_advkey_t;

// getParam <DN_API_PARAM_ADVKEY>
typedef dn_api_get_hdr_t dn_api_get_advkey_t;
typedef struct {
   uint8_t       rc;
   uint8_t       paramId;
   dn_key_t    key;
}   dn_api_rsp_get_advkey_t;


// setParam <DN_API_PARAM_AUTOJOIN>
typedef struct {
   uint8_t   paramId;
   uint8_t   mode; 
}   dn_api_set_autojoin_t;
typedef dn_api_rsp_set_hdr_t dn_api_rsp_set_autojoin_t;

// getParam <DN_API_PARAM_AUTOJOIN>
typedef dn_api_get_hdr_t dn_api_get_autojoin_t;
typedef struct {
   uint8_t   rc;
   uint8_t   paramId;
   uint8_t   mode; 
}   dn_api_rsp_get_autojoin_t;

PACKED_STOP
#endif /* _DN_API_PARAM_H */
