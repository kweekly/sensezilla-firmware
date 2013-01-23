/*
 * Copyright (c) 2010, Dust Networks.  All rights reserved.
 *
 * $HeadURL$
 * $Rev$
 * $Author$
 * $LastChangedDate$ 
 */

#ifndef _DN_API_COMMON_H
#define _DN_API_COMMON_H
#include "dn_typedef.h"

PACKED_START /** all structures that follow are packed */

/*********************************************************/
/* Local API commands & notifications                    */
/*********************************************************/
#define DN_API_LOC_CMD_SETPARAM                    0x01
#define DN_API_LOC_CMD_GETPARAM                    0x02
#define DN_API_LOC_CMD_RESERVED1                   0x03
#define DN_API_LOC_CMD_RESERVED2                   0x04
#define DN_API_LOC_CMD_RESERVED3                   0x05
#define DN_API_LOC_CMD_JOIN                        0x06
#define DN_API_LOC_CMD_DISCONNECT                  0x07
#define DN_API_LOC_CMD_RESET                       0x08
#define DN_API_LOC_CMD_LOWPWRSLEEP                 0x09
#define DN_API_LOC_CMD_RESERVED4                   0x0A
#define DN_API_LOC_CMD_TESTRADIOTX                 0x0B
#define DN_API_LOC_CMD_TESTRADIORX                 0x0C
#define DN_API_LOC_NOTIF_TIME                      0x0D
#define DN_API_LOC_NOTIF_EVENTS                    0x0F
#define DN_API_LOC_CMD_CLEARNV                     0x10
#define DN_API_LOC_CMD_SERVICE_REQUEST             0x11
#define DN_API_LOC_CMD_GET_SVC_INFO                0x12
#define DN_API_LOC_CMD_WRITE_FILE                  0x13
#define DN_API_LOC_CMD_READ_FILE                   0x14
#define DN_API_LOC_CMD_OPEN_SOCKET                 0x15
#define DN_API_LOC_CMD_CLOSE_SOCKET                0x16
#define DN_API_LOC_CMD_BIND_SOCKET                 0x17
#define DN_API_LOC_CMD_SENDTO                      0x18
#define DN_API_LOC_NOTIF_RECEIVED                  0x19
#define DN_API_LOC_CMD_TUNNEL                      0x21
#define DN_API_LOC_CMD_UNUSED                      0x22
#define DN_API_LOC_CMD_SENDTO_MAC                  0x23
#define DN_API_LOC_CMD_SEARCH                      0x24
#define DN_API_LOC_NOTIF_TXDONE                    0x25
#define DN_API_LOC_NOTIF_ADVRX                     0x26
#define DN_API_LOC_NOTIF_MACRX                     0x27






/*********************************************************/
/* Local Events  (see DN_API_LOC_NOTIF_EVENTS)               */
/*********************************************************/      
#define DN_API_LOC_EV_BOOT                         0x01     // The mote booted up
#define DN_API_LOC_EV_ALARMS_CHG                   0x02     // Value of alarms field changed
#define DN_API_LOC_EV_TIME_CHG                     0x04     // UTC time on the mote changed
#define DN_API_LOC_EV_JOINFAIL                     0x08     // Join operation failed
#define DN_API_LOC_EV_DISCON                       0x10     // The mote disconnected from the network
#define DN_API_LOC_EV_OPERATIONAL                  0x20     // Mote has connection to gateway to send data
#define DN_API_LOC_RESERVED                        0x40     // Reserved
#define DN_API_LOC_EV_SVC_CHG                      0x80     // Mote service changed
#define DN_API_LOC_EV_JOINSTART                    0x100    // Mote sent the 1st packet to initiate join sequence


/*********************************************************/
/* Network API Commands                                  */
/*********************************************************/
#define DN_API_NET_CMD_SET_PARAM                   0x01
#define DN_API_NET_CMD_GET_PARAM                   0x02
#define DN_API_NET_CMD_RESERVED1                   0x03
#define DN_API_NET_CMD_RESERVED2                   0x04
#define DN_API_NET_CMD_ADD_FRAME        	         0x05
#define DN_API_NET_CMD_DEL_FRAME        	         0x06
#define DN_API_NET_CMD_CH_SIZE_FRAME    	         0x07
#define DN_API_NET_CMD_CTRL_FRAME        	         0x08
#define DN_API_NET_CMD_ADD_LINK         	         0x09
#define DN_API_NET_CMD_DEL_LINK         	         0x0A
#define DN_API_NET_CMD_ADD_MUL_LINK     	         0x0B
#define DN_API_NET_CMD_DEL_MUL_LINK     	         0x0C
#define DN_API_NET_CMD_ADD_SESSION      	         0x0D
#define DN_API_NET_CMD_DEL_SESSION      	         0x0E
#define DN_API_NET_CMD_ADD_CONN         	         0x0F
#define DN_API_NET_CMD_DEL_CONN         	         0x10
#define DN_API_NET_CMD_ACTIVATE         	         0x11
#define DN_API_NET_CMD_SET_NBR_PROP     	         0x12
#define DN_API_NET_CMD_ADD_GR_ROUTE     	         0x13
#define DN_API_NET_CMD_DEL_GR_ROUTE     	         0x14
#define DN_API_NET_CMD_ECHO_REQ         	         0x15
#define DN_API_NET_CMD_OTAP_HANDSHAKE   	         0x16
#define DN_API_NET_CMD_OTAP_DATA        	         0x17
#define DN_API_NET_CMD_OTAP_STATUS                   0x18
#define DN_API_NET_CMD_OTAP_COMMIT      	         0x19
#define DN_API_NET_CMD_GET_LOG          	         0x1A
#define DN_API_NET_CMD_GET_REQ_SRV      	         0x1B
#define DN_API_NET_CMD_ASSIGN_SRV       	         0x1C
#define DN_API_NET_CMD_CH_NET_KEY       	         0x1D
#define DN_API_NET_CMD_DISCONNECT                    0x1E
#define DN_API_NET_CMD_TUN_LOC_CMD      	         0x1F
#define DN_API_NET_CMD_REFRESH_ADV    	             0x20
#define DN_API_NET_CMD_READ_FRAME      	             0x21
#define DN_API_NET_CMD_READ_LINK       	             0x22
#define DN_API_NET_CMD_READ_SESSION     	         0x23
#define DN_API_NET_CMD_READ_GR_ROUTE     	         0x24



/*********************************************************/
/* Network API notifications                             */
/*********************************************************/
#define DN_API_NET_NOTIF_DEV_HR                    0x80
#define DN_API_NET_NOTIF_NBR_HR                    0x81
#define DN_API_NET_NOTIF_DSCV_NBR                  0x82
#define DN_API_NET_NOTIF_PATH_ALARM                0x83
#define DN_API_NET_NOTIF_SR_FAIL_ALARM             0x84
#define DN_API_NET_NOTIF_ECHO_RESP                 0x85
#define DN_API_NET_NOTIF_REQ_SERVICE               0x86
#define DN_API_NET_NOTIF_ACCEPT_CONTEXT            0x87
#define DN_API_NET_NOTIF_TRACE                     0x88
#define DN_API_NET_NOTIF_MOTE_JOIN                 0x89
#define DN_API_NET_NOTIF_AP_JOIN                   0x90

#define DN_API_NET_NOTIF_FIRST                     DN_API_NET_NOTIF_DEV_HR
#define DN_API_NET_NOTIF_LAST                      DN_API_NET_NOTIF_AP_JOIN


/*********************************************************/
/* set/get parameters                                    */
/*********************************************************/           
#define DN_API_PARAM_MACADDR                       0x01
#define DN_API_PARAM_JOINKEY                       0x02
#define DN_API_PARAM_NETID                         0x03
#define DN_API_PARAM_TXPOWER                       0x04
#define DN_API_PARAM_RSVD1                         0x05
#define DN_API_PARAM_JOINDUTYCYCLE                 0x06
#define DN_API_PARAM_RSVD2                         0x07
#define DN_API_PARAM_RSVD3                         0x08
#define DN_API_PARAM_RSVD4                         0x09
#define DN_API_PARAM_RSVD5                         0x0A
#define DN_API_PARAM_EVENTMASK                     0x0B
#define DN_API_PARAM_MOTEINFO                      0x0C
#define DN_API_PARAM_NETINFO                       0x0D
#define DN_API_PARAM_MOTESTATUS                    0x0E
#define DN_API_PARAM_TIME                          0x0F
#define DN_API_PARAM_CHARGE                        0x10
#define DN_API_PARAM_TESTRADIORXSTATS              0x11
#define DN_API_PARAM_RSVD6                         0x12
#define DN_API_PARAM_RSVD7                         0x13
#define DN_API_PARAM_RSVD8                         0x14
#define DN_API_PARAM_OTAP_LOCKOUT                  0x15
#define DN_API_PARAM_MACMICKEY                     0x16
#define DN_API_PARAM_SHORTADDR                     0x17
#define DN_API_PARAM_IP6ADDR                       0x18
#define DN_API_PARAM_CCAMODE                       0x19
#define DN_API_PARAM_CHANNELS                      0x1A
#define DN_API_PARAM_ADVGRAPH                      0x1B
#define DN_API_PARAM_HRTMR                         0x1C
#define DN_API_PARAM_ROUTINGMODE                   0x1D
#define DN_API_PARAM_APPINFO                       0x1E
#define DN_API_PARAM_PWRSRCINFO                    0x1F
#define DN_API_PARAM_PWRCOSTINFO                   0x20
#define DN_API_PARAM_MOBILITY                      0x21
#define DN_API_PARAM_ADVKEY                        0x22
#define DN_API_PARAM_SIZEINFO                      0x23
#define DN_API_PARAM_AUTOJOIN                      0x24
#define DN_API_PARAM_PATHALARMGEN                   0x25

/*********************************************************/
/* Common return codes                                   */
/*********************************************************/				

typedef enum {
   DN_API_RC_OK =                                  0x00, 
   DN_API_RC_ERROR =                               0x01,     // generic error
   DN_API_RC_RSVD1 =                               0x02,     // reserved
   DN_API_RC_BUSY =                                0x03,
   DN_API_RC_INVALID_LEN =                         0x04,
   DN_API_RC_INV_STATE =                           0x05,
   DN_API_RC_UNSUPPORTED =                         0x06,
   DN_API_RC_UNKNOWN_PARAM =                       0x07,
   DN_API_RC_UNKNOWN_CMD =                         0x08,
   DN_API_RC_WRITE_FAIL =                          0x09,
   DN_API_RC_READ_FAIL =                           0x0A,
   DN_API_RC_LOW_VOLTAGE =                         0x0B,
   DN_API_RC_NO_RESOURCES =                        0x0C,
   DN_API_RC_INCOMPLETE_JOIN_INFO =                0x0D,
   DN_API_RC_NOT_FOUND =                           0x0E,
   DN_API_RC_INVALID_VALUE =                       0x0F,
   DN_API_RC_ACCESS_DENIED =                       0x10,    // access to this command/variable is denied
   DN_API_RC_DUPLICATE =                           0x11,
//------------------------------------------------------
   DN_API_RC_MAX =                                 0x12     // should be always the last
} dn_api_rc_t;

// OTAP retun code
typedef enum {
   DN_API_OTAP_RC_OK                 = 0,
   DN_API_OTAP_RC_LOWBATT            = 1,  /* too low battery voltage */
   DN_API_OTAP_RC_FILE               = 2,  /* file size, block size, start address, execute size is wrong */ 
   DN_API_OTAP_RC_INVALID_PARTITION  = 3,  /* Invalid partition information (ID, file size) */
   DN_API_OTAP_RC_INVALID_APP_ID     = 4,  /* AppId is not correct. */
   DN_API_OTAP_RC_INVALID_VER        = 5,  /* SW vesrsions are not compatible for OTAP */
   DN_API_OTAP_RC_INVALID_VENDOR_ID  = 6,  /* invalid vendor ID */
   DN_API_OTAP_RC_RCV_ERROR          = 7,
   DN_API_OTAP_RC_FLASH              = 8,
   DN_API_OTAP_RC_MIC                = 9,  /* MIC failed on uploaded data */
   DN_API_OTAP_RC_NOT_IN_OTAP        = 10, /* No OTAP handshake is initiated */
   DN_API_OTAP_RC_IOERR              = 11, /* IO error */
   DN_API_OTAP_RC_CREATE             = 12, /* Can not create OTAP file */
   DN_API_OTAP_RC_INVALID_EXEPAR_HDR = 13, /* Wrong value for exe-partition heder fieds 'signature' or 'upgrade' */
   DN_API_OTAP_RC_RAM                = 14, /* Can not allocate memory */
   DN_API_OTAP_RC_UNCOMPRESS         = 15, /* Uncompress error */
   DN_API_OTAP_IN_PROGRESS           = 16, /* OTAP in progress */
   DN_API_OTAP_LOCK                  = 17, /* OTAP lock */
} dn_api_otap_rc_t;

// values for protocol in DN_API_LOC_CMD_OPEN_SOCKET
#define DN_API_PROTO_UDP                           0      

// values for DN_API_PARAM_ROUTINGMODE
#define DN_API_ROUTING_ENABLED                     0
#define DN_API_ROUTING_DISABLED                    1

// values for DN_API_PARAM_MOBILITY  
#define DN_API_LOCATION_UNUSED                     0
#define DN_API_LOCATION_KNOWN                      1
#define DN_API_LOCATION_UNKNOWN                    2
#define DN_API_LOCATION_MOBILE                     3

// service type
#define DN_API_SERVICE_TYPE_BW                     0
#define DN_API_SERVICE_TYPE_LATENCY                1

// service state
#define DN_API_SERVICE_STATE_REQ_COMPLETED         0
#define DN_API_SERVICE_STATE_REQ_PENDING           1

// Packet priority in DN_API_LOC_CMD_SENDTO 
#define DN_API_PRIORITY_LOW                        0
#define DN_API_PRIORITY_MED                        1
#define DN_API_PRIORITY_HIGH                       2
#define DN_API_PRIORITY_CTRL                       3

// mote state in DN_API_PARAM_MOTESTATUS 
#define DN_API_ST_INIT                             0        // Initializing
#define DN_API_ST_IDLE                             1        // Idle, ready to be configured or join
#define DN_API_ST_SEARCHING                        2        // Received join command, searching for network
#define DN_API_ST_NEGO                             3        // Sent join request
#define DN_API_ST_CONNECTED                        4        // Received at least one packet from the controller
#define DN_API_ST_OPERATIONAL                      5        // Configured by controller and ready to send data
#define DN_API_ST_DISCONNECTED                     6        // Disconnected from the network
#define DN_API_ST_RADIOTEST                        7        // Radio test
#define DN_API_ST_PROMISC_LISTEN                   8        // Promisc listen

// power limit table in DN_API_PARAM_PWRSRCINFO
#define DN_API_PWR_LIMIT_CNT                       3        // # of limits in pwrsrc

#define DN_API_SETNV_MAGIC_NUMBER 0

// power cost defs for DN_API_PARAM_PWRCOSTINFO
#define DN_API_PWRCOSTINFO_MAX_UNLIM 0xFFFF
#define DN_API_PWRCOSTINFO_MIN_UNLIM 0

// values for DN_API_PARAM_CCAMODE
#define DN_API_CCA_MODE_OFF                        0
#define DN_API_CCA_MODE_ON                         1

// values for DN_API_PARAM_TXPOWER
#define DN_API_TXPOWER_PA_OFF                      0
#define DN_API_TXPOWER_PA_ON                       8

// values for status in DN_API_LOC_NOTIF_TXDONE 
#define DN_API_TXSTATUS_OK                         0
#define DN_API_TXSTATUS_FAIL                       1

// frame mode for DN_API_NET_CMD_ADD_FRAME
#define DN_API_FRAME_MODE_INACTIVE                 0        // Inactive
#define DN_API_FRAME_MODE_ACTIVE                   1        // Active

// link type in DN_API_NET_CMD_XXX_LINK
#define DN_API_LINK_TYPE_NORMAL                    0        // Normal
#define DN_API_LINK_TYPE_JOIN                      1        // Join 
#define DN_API_LINK_TYPE_DSCV                      2        // Discovery
#define DN_API_LINK_TYPE_ADV                       3        // Advertisement

// link option flag in DN_API_NET_CMD_XXX_LINK
#define DN_API_LINK_OPT_TX                         0x01     // Transmit link
#define DN_API_LINK_OPT_RX                         0x02     // Receive link
#define DN_API_LINK_OPT_SHARED                     0x04     // Shared link               
#define DN_API_LINK_OPT_TOF                        0x08     // TOF ranging
#define DN_API_LINK_OPT_NO_PF                      0x10     // No path failure monitoring on this link

// flags for DN_API_NET_CMD_ACTIVATE
#define DN_API_ACTFL_CONRDY                        0x1      // Connection is ready 
#define DN_API_ACTFL_ADVRDY                        0x2      // Advertisment links is creatd 
#define DN_API_ACTFL_OPER                          0x4      // Mote ready 

// session type for DN_API_NET_CMD_XXX_SESSION
#define DN_API_SESSION_TYPE_UCAST                  0        // Unicast
#define DN_API_SESSION_TYPE_BCAST                  1        // Broadcast

// values for DN_API_NET_CMD_SET_NBR_PROP
#define DN_API_NBR_FLAG_TIME_SOURCE                0x01     // Time Source

// OTAP constatnts
#define  DN_API_OTAP_DATA_SIZE                     84
#define  DN_API_OTAP_MAXLOSTBLOCKS_SIZE            40

// DN_API_NET_NOTIF_TRACE types
#define  DN_API_NET_NOTIF_TRACETYPE_CLIPRINT       0
#define  DN_API_NET_NOTIF_TRACETYPE_LOGTRACE       1
// DN_API_NET_NOTIF_TRACE max size of trace string
#define  DN_API_NET_NOTIF_TRACE_MAXSTRINGSIZE      80

// Default route for all services
#define  DN_API_ROUTE_ALLSERVICES                   0xFF

// Alarms 
#define DN_API_ALARM_NVERR                   0x1
#define DN_API_ALARM_LOWVOLTAGE              0x2
#define DN_API_ALARM_OTPERR                  0x4
#define DN_API_ALARM_SEND_NOTREADY           0x8


// radio-test. type of transmissions test
#define DN_API_RADIOTX_TYPE_PKT           0
#define DN_API_RADIOTX_TYPE_CM            1
#define DN_API_RADIOTX_TYPE_CW            2

//  Common typedefs


/* request command header */
typedef struct {
   uint8_t    cmdId; /* command id */
   uint8_t    len;   /* length of request, excluding this header */
}  dn_api_cmd_hdr_t;

/* empty reply */
typedef struct {
   dn_api_cmd_hdr_t hdr;
   uint8_t         rc;
}  dn_api_emptyrpl_t;

/* payload of reply with rc */
typedef struct {
  uint8_t rc; 
}  dn_api_rc_rsp_t;

// basic getParam payload
typedef struct {
   uint8_t paramId;
}  dn_api_get_hdr_t;

/* getParameter response */
typedef struct {
   uint8_t    rc;    /* result code */
   uint8_t    paramId;
}  dn_api_rsp_get_hdr_t;

// basic setParam payload
typedef struct {
   uint8_t paramId;
}  dn_api_set_hdr_t;

/* setParameter response */
typedef struct {
   uint8_t    rc;    /* result code */
   uint8_t    paramId;
}  dn_api_rsp_set_hdr_t;

// sw version information
typedef struct {
   uint8_t   major;
   uint8_t   minor;
   uint8_t   patch;
   uint16_t  build;
}  dn_api_swver_t;

// power limit info
typedef struct {
   uint16_t  currentLimit;      // current limit in uA. 0=limit entry empty
   uint16_t  dischargePeriod;   // in seconds
   uint16_t  rechargePeriod;    // in seconds
}  dn_api_pwrlim_t;

// power source info
typedef struct {
   uint16_t  maxStCurrent; //Max steady-state current; FFFF=no limit
   uint8_t   minLifeTime;  //Min lifetime, in months; 0=no limit
   dn_api_pwrlim_t limit[DN_API_PWR_LIMIT_CNT];   //temporary current limits
}   dn_api_pwrsrcinfo_t;

// power cost info
typedef struct {
   uint8_t   maxTxCost;    //cost of max PA TX in uC, in 7.1 format
   uint8_t   maxRxCost;    //cost of max PA RX in uC, in 7.1 format
   uint8_t   minTxCost;    //cost of min PA TX in uC, in 7.1 format
   uint8_t   minRxCost;    //cost of min PA RX in uC, in 7.1 format
}  dn_api_pwrcostinfo_t;

PACKED_STOP /** back to default packing */

#endif // DN_API_COMMON_H
