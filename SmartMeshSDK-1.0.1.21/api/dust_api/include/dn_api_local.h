/*
 * Copyright (c) 2009, Dust Networks.  All rights reserved.
 *
 * $HeadURL: https://subversion/software/trunk/shared/include/6lowpan/dn_api_local.h $
 * $Rev: 42322 $
 * $Author: yzats $
 * $LastChangedDate: 2010-12-23 16:45:15 -0800 (Thu, 23 Dec 2010) $ 
 */
#ifndef _DN_API_LOCAL_H
#define _DN_API_LOCAL_H

#include "dn_typedef.h"
#include "dn_api_common.h"
#include "dn_mesh.h"

#ifdef WIN32
   #pragma warning( disable : 4200) /* allow 0-sized arrays in struct */
#endif

PACKED_START

// DN_API_LOC_CMD_SETPARAM
typedef struct {
   uint8_t paramId;
   uint8_t payload[]; // param-specific payload
}  dn_api_loc_setparam_t;

typedef struct {
   uint8_t rc;
   uint8_t paramId;
   uint8_t payload[];
}  dn_api_loc_rsp_setparam_t;


// DN_API_LOC_CMD_GETPARAM
typedef struct {
   uint8_t paramId;
   uint8_t payload[]; // param-specific payload
}  dn_api_loc_getparam_t;

typedef struct {
   uint8_t rc;
   uint8_t paramId;
   uint8_t payload[];
}  dn_api_loc_rsp_getparam_t;

// DN_API_LOC_CMD_SETNVPARAM
typedef struct {
   uint32_t magic;
   uint8_t  paramId;
   uint8_t  payload[]; // param-specific payload
}  dn_api_loc_setnvparam_t;

typedef struct {
   uint8_t rc;
   uint8_t paramId;
}  dn_api_loc_rsp_setnvparam_t;


// MOTEAPI_CMD_JOIN 
// cmd has no payload
typedef dn_api_rc_rsp_t dn_api_loc_rsp_join_t;

// DN_API_LOC_CMD_DISCONNECT
// cmd has no payload
typedef dn_api_rc_rsp_t dn_api_loc_rsp_discon_t;

// DN_API_LOC_CMD_RESET 
// cmd has no payload
typedef dn_api_rc_rsp_t dn_api_loc_rsp_reset_t;

// DN_API_LOC_CMD_LOWPWRSLEEP
// cmd has no payload
typedef dn_api_rc_rsp_t dn_api_loc_rsp_lpsleep_t;

// DN_API_LOC_CMD_TESTRADIOTX
typedef struct {
   uint8_t  type;       /* Test type. See DN_LOC_RADIOTX_TYPE_xxx */
   uint16_t mask;       /* mask enabled channels (for CW, CM test one channel only) */
   uint16_t numPkts;    /* number of packets to transmit */
   uint8_t  pktLen;     /* length of packet (0 - default max length) */
   INT8S  txPower;    /* transmit power: -2=min (PA off), 8=max (PA on) */
} dn_api_loc_testrftx_t;

typedef dn_api_rc_rsp_t dn_api_loc_rsp_testrftx_t;

// DN_API_LOC_CMD_TESTRADIORX
typedef struct {
   uint8_t   channel;       /* rf channel 0-15 to use for the test */
   uint16_t  timeSeconds;   /* duration of the rx test, in seconds */
} dn_api_loc_testrfrx_t;

typedef dn_api_rc_rsp_t dn_api_loc_rsp_testrfrx_t;

// DN_API_LOC_CMD_CLEARNV 
// cmd has no payload
typedef dn_api_rc_rsp_t dn_api_loc_rsp_clearnv_t;

// DN_API_LOC_CMD_SERVICE_REQUEST
typedef struct {   
   dn_moteid_t  dest;
   uint8_t        type;        // See SRV_TYPE_xxx
   uint32_t       value;                                           
}  dn_api_loc_svcrequest_t;

typedef dn_api_rc_rsp_t dn_api_loc_rsp_svcrequest_t;


// DN_API_LOC_CMD_GET_SVC_INFO
typedef struct {
   dn_moteid_t    dest;       /* address  of in-mesh  destination of service */
   uint8_t          type;       /* service  type */
}  dn_api_loc_get_service_t;

typedef struct {
   uint8_t          rc;
   dn_moteid_t    dest;       /* address  of in-mesh  destination of service */
   uint8_t          type;        // See SRV_TYPE_xxx
   uint8_t          status;      // state of the indicated service
   uint32_t         value;       // currently assigned bw
}  dn_api_loc_rsp_get_service_t;


// DN_API_LOC_CMD_WRITE_FILE
typedef struct {
   uint8_t   fileId;
   uint32_t  offset;
   uint32_t  len;
   uint8_t   data[];
} dn_api_loc_write_file_t;

typedef dn_api_rc_rsp_t dn_api_loc_rsp_write_file_t;


// DN_API_LOC_CMD_READ_FILE 
typedef struct {
   uint8_t	  fd;
   uint16_t  offset;
   uint8_t   len;
}  dn_api_loc_read_file_t;

typedef struct{
   uint8_t   rc; /* result code */
   uint32_t  len;
   uint8_t   data[1];
} dn_api_loc_rsp_read_file_t;


// DN_API_LOC_CMD_OPEN_SOCKET 
typedef struct {
   uint8_t   protocol;      // 0 - udp
}  dn_api_loc_open_socket_t;

typedef struct {
   uint8_t   rc; /* result code */
   uint8_t   socketId;   
}  dn_api_loc_rsp_open_socket_t;


// DN_API_LOC_CMD_CLOSE_SOCKET   
typedef struct {
   uint8_t   socketId;
}  dn_api_loc_close_socket_t;

typedef dn_api_rc_rsp_t dn_api_loc_rsp_close_socket_t;


// DN_API_LOC_CMD_BIND_SOCKET 
typedef struct {   
   uint8_t   socketId;   
   uint16_t  port;
}  dn_api_loc_bind_socket_t;

typedef dn_api_rc_rsp_t dn_api_loc_rsp_bind_socket_t;


// DN_API_LOC_CMD_SENDTO   
typedef struct {
   uint8_t   socketId;       
   dn_ipv6_addr_t   destAddr;       /* destination address */
   uint16_t  destPort;
   uint8_t   serviceType;
   uint8_t   priority;        /* 0=low, 1=med, 2=high */
   uint16_t  packetId;        /* for tx done notification; FFFF=no notif */
   uint8_t   payload[];      /* payload */
}   dn_api_loc_sendto_t;

typedef dn_api_rc_rsp_t dn_api_loc_rsp_sendto_t;

#define LOC_SENDTO_DATA_OFFSET ((((dn_api_loc_sendto_t*)0)->payload) - ((uint8_t *)(dn_api_loc_sendto_t*)0)) 

// MOTEAPI_CMD_TUNNEL
//typedef struct{
//   uint8_t   data[1];
//} dn_api_loc_tun_t;

/* data tunnel response command header */
typedef struct{
   uint8_t   rc; /* result code */
   uint8_t   data[];
} dn_api_loc_rsp_tun_t;


// control local sendtomac command
typedef struct{
#ifdef L_ENDIAN
   uint8_t   priority:2;      //00=low, 01=med, 10=high, 11=ctrl
   uint8_t   linkCtrl:1;      /*0=default link selection. 
                                    1=use frame/slot/offset*/
   uint8_t   txDoneNotif:1;   /* 1=txDone notification required */
   uint8_t   reserved:4;
#else
   uint8_t   reserved:4;
   uint8_t   txDoneNotif:1;   /* 1=txDone notification required */
   uint8_t   linkCtrl:1;      /*0=default link selection. 
                                    1=use frame/slot/offset*/
   uint8_t   priority:2;      //00=low, 01=med, 10=high, 11=ctrl
#endif   

   uint16_t  packetId;  // Packet ID (used by txDone notification); 0xFFFF=no notif
   uint8_t   frameId;   // Output link definition
   uint32_t  timeslot;
   uint8_t   channel;
   uint16_t  dest;   
}  dn_api_loc_sendtomac_ctrl_t;


// DN_API_LOC_CMD_SENDTO_MAC
typedef struct {
   dn_api_loc_sendtomac_ctrl_t control;   // control
   uint8_t   payload[DN_MAX_MAC_PAYLOAD];   // max payload (includes mesh hdr, 6lowpan, user payload)
}  dn_api_loc_sendtomac_t;

typedef dn_api_rc_rsp_t dn_api_loc_rsp_sendtomac_t;

// DN_API_LOC_CMD_SEARCH 
// cmd has no payload
typedef dn_api_rc_rsp_t dn_api_loc_rsp_search_t;


/************************** NOTIFICATIONS *****************************/


// dn_api_loc_notif__EVENTS 
typedef struct {
   uint32_t  events;      /* new events */
   uint8_t   state;       /* current state, same as in networkInfo */
   uint32_t  alarms;      /* current alarms */ 
}  dn_api_loc_notif_events_t;


// dn_api_loc_notif__RECEIVED 
typedef struct {
   uint8_t   socketId;    /* socket on which the packet was received */
   dn_ipv6_addr_t sourceAddr; /* source of the received packet */
   uint16_t  sourcePort;
   uint8_t   data[];      /* packet payload */
}    dn_api_loc_notif_received_t;

//define dn_api_loc_notif__MACRX   
//typedef struct {
//   uint8_t   data[1];     /* packet payload */
//}  dn_api_loc_macrx_t; 

//define dn_api_loc_notif__TXDONE
typedef struct {
   uint16_t   packetId;
   uint8_t	   status;     // see Transmit Status
}  dn_api_loc_notif_txdone_t; 

// DN_API_LOC_NOTIF_TIME
typedef struct {
   uint32_t         upTime;      // time (sec) that the packet was generated (as uptime)
   dn_utc_time_t  utcTime;     // utc time 
   dn_asn_t       asn;         // ASN
   uint16_t         offset;      // ASN offset by micro-seconds
}  dn_api_loc_notif_time_t; 

// DN_API_LOC_NOTIF_ADV_RX
typedef struct {
   uint16_t netId;     /* network id */
   uint16_t moteId;    /* mote id */
   INT8S  rssi;      /* rssi */
   uint8_t  joinPri;   /* join priority (aka hop depth) */
}  dn_api_loc_notif_adv_t;

//////////////////// maximum payload definitions for local interface /////////////

// the largest message on local interface is sendtomac  
#define DN_API_LOC_MAXMSG_SIZE               (sizeof(dn_api_cmd_hdr_t) + sizeof(dn_api_loc_sendtomac_t))

// applications should use these defines for max message size
#define DN_API_LOC_MAX_REQ_SIZE              DN_API_LOC_MAXMSG_SIZE   // max request on ctrl chan
#define DN_API_LOC_MAX_RESP_SIZE             DN_API_LOC_MAXMSG_SIZE   // max response on ctrl chan
#define DN_API_LOC_MAX_NOTIF_SIZE            DN_API_LOC_MAXMSG_SIZE   // max notif

// max size of payload in sendto and receive payloads
#define DN_API_LOC_MAX_USERPAYL_SIZE         80  

PACKED_STOP


#endif /* _MOTEAPI_LOCAL_H */
