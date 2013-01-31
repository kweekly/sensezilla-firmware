#ifndef ___DN_API_MGR_STRUCT_H
#define ___DN_API_MGR_STRUCT_H

#include "dn_typedef.h"

#define DN_API_PROTOCOL_VER   4

// control field
#define DN_API_CONTROL_ACK    0x01
#define DN_API_CONTROL_RLBL   0x02


#define DN_API_ASN_SIZE       5
#define DN_API_MAC_SIZE       8
#define DN_API_JOINKEY_SIZE   16 
#define DN_API_LICENSE_SIZE   13
#define DN_API_USRPWD_SIZE    16

#define DN_ENCRYPT_OFFSET_NONE 0xFF

// Commands IDs 
enum dn_api_appCommand {
   DN_API_NULL_PKT                  = 0,
   DN_API_HELLO                     = 1,
   DN_API_HELLO_RSP                 = 2,
   DN_API_MGR_HELLO                 = 3,

   // PICARD commands
   DN_API_NOTIF                     = 20, // notification 
   DN_API_RESET                     = 21, // resetCmdFunc
   DN_API_SUBSCRIBE                 = 22, // subscrCmdFunc
   DN_API_GET_TIME                  = 23, // getCurrentTimeCmdFunc
   DN_API_RESERV1                   = 24, // 
   DN_API_RESERV2                   = 25, // 
   DN_API_SET_NET_CFG               = 26, // setNetCfgCmdFunc
   DN_API_RESERV3                   = 27, // 
   DN_API_RESERV4                   = 28, // 
   DN_API_RESERV5                   = 29, // 
   DN_API_RESERV6                   = 30, // 
   DN_API_CLEAR_STATS               = 31, // clearStatsCmdFunc    
   DN_API_RESERV7                   = 32, // 
   DN_API_EXCH_JOINKEY              = 33, // exchJoinKeyCmdFunc   
   DN_API_EXCH_NETWORKID            = 34, // exchNetworkIdCmdFunc 
   DN_API_RESERV8                   = 35, // 
   DN_API_RADIOTEST_TX              = 36, // radiotestTxCmdFunc 
   DN_API_RADIOTEST_RX              = 37, // radiotestRxCmdFunc   
   DN_API_RADIOTEST_STATS           = 38, // radiotestStatsCmdFunc
   DN_API_SET_ACL_ENTRY             = 39, // setAclEntryCmdFunc
   DN_API_GET_NEXT_ACL_ENTRY        = 40, // getNextAclEntryCmdFunc
   DN_API_DEL_ACL_ENTRY             = 41, // delAclEntryCmdFunc
   DN_API_PING_MOTE                 = 42, // pingMoteCmdFunc
   DN_API_GET_LOG                   = 43, // getLogCmdFunc
   DN_API_SEND_DATA                 = 44, // sendDataCmdFunc
   DN_API_START_NETWORK             = 45, // startNetworkCmdFunc
   DN_API_GET_SYSTEM_INFO           = 46, // getSystemCmdFunc
   DN_API_GET_MOTE_CFG              = 47, // getMoteCmdFunc
   DN_API_GET_PATH                  = 48, // getPathCmdFunc
   DN_API_GET_NEXT_PATH             = 49, // getNextPathCmdFunc
   DN_API_SET_ADVERTISING           = 50, // setAdvertisingCmdFunc
   DN_API_SET_DFRAME_SIZE           = 51, // enableFrameMultCmdFunc
   DN_API_MEASURE_RSSI              = 52, // measureRssiCmdFunc
   DN_API_GET_MANAGER_INFO          = 53, // getManagerInfoCmdFunc
   DN_API_SET_TIME                  = 54, // setTimeCmdFunc
   DN_API_GET_LICENSE               = 55, // getLicenseCmdFunc
   DN_API_SET_LICENSE               = 56, // setLicenseCmdFunc
   DN_API_RESERV9                   = 57, // 
   DN_API_SET_CLI_USER              = 58, // setCliUserCmdFunc
   DN_API_SEND_IP                   = 59, // sendIpCmdFunc
   DN_API_START_LOCATION            = 60, // startLocationCmdFunc
   DN_API_RESTORE_FACTORY_DEFAULTS  = 61, // restoreFactoryDefaultsCmdFuns
   DN_API_GET_MOTE_INFO             = 62, // getMoteInfoCmdFunc
   DN_API_GET_NET_CFG               = 63, // getNetCfgCmdFunc
   DN_API_GET_NET_INFO              = 64, // getNetInfoCmdFunc
   DN_API_GET_MOTE_CFG_BY_ID        = 65, // getMoteCfgByIdCmdFunc
   DN_API_SET_COMMONJOINKEY         = 66, // set common join key
   DN_API_GET_IP_CFG                = 67, // get IP configuration
   DN_API_SET_IP_CFG                = 68, // set IP configuration
   DN_API_DEL_MOTE                  = 69, // Delete Mote
   DN_API_APP_CMD_TOTAL             = 70, // 
};

// Notification IDs
enum dn_api_notifType {
   DN_API_NOTIF_RESERV1             = 0, // 
   DN_API_NOTIF_EVENT               = 1, // event notification
   DN_API_NOTIF_LOG                 = 2, // log message
   DN_API_NOTIF_RESERV2             = 3, // 
   DN_API_NOTIF_DATA                = 4, // data payload
   DN_API_NOTIF_IP_DATA             = 5, // 6lowpan packet
   DN_API_NOTIF_HR                  = 6, // health report
};

// Event IDs
enum dn_api_eventType {
   DN_API_EV_MOTE_RESET             = 0, // eventData is mac address
   DN_API_EV_NET_RESET              = 1, // eventData is empty
   DN_API_EV_CMD_FINISH             = 2, // eventData is struct cmdFinish
   DN_API_EV_MOTE_JOIN              = 3, // eventData is mac address
   DN_API_EV_MOTE_OPER              = 4, // eventData is mac address
   DN_API_EV_MOTE_LOST              = 5, // eventData is mac address
   DN_API_EV_NET_TIME               = 6, // eventData is struct dn_api_timeEvent
   DN_API_EV_PING_RESP              = 7, // eventData is struct dn_api_pingResp
   DN_API_EV_RSV1                   = 8, // 
   DN_API_EV_MOTE_BANDWIDTH         = 9, // eventData is mac address
   DN_API_EV_PATH_CREATE            = 10,// eventData is struct path
   DN_API_EV_PATH_DELETE            = 11,// eventData is struct path
   DN_API_EV_PACKET_SENT            = 12,// eventData is struct cmdFinish
   DN_API_EV_MOTE_CREATE            = 13,// eventData is struct moteinfo
   DN_API_EV_MOTE_DELETE            = 14,// eventData is struct moteinfo
};

// Notificatio subscrib flags 
enum dn_api_subscribeFlags {
   DN_API_SUBSCR_EVENT              = 0x02,
   DN_API_SUBSCR_LOG                = 0x04,
   DN_API_SUBSCR_DATA               = 0x10,
   DN_API_SUBSCR_IP_DATA            = 0x20,
   DN_API_SUBSCR_HR                 = 0x40,
};

//[ Serial API Command and Response Structures -------------------------------
enum dn_api_responseCode {
   DN_API_RESP_OK                = 0,
   DN_API_RESP_INV_CMD           = 1,   // invalid command
   DN_API_RESP_INV_ARG           = 2,   // invalid argument
   DN_API_BOOT_RESP_CODE_FIRST   = 3,   // 3 up to 8 boot response codes

	DN_API_RESP_E_LIST            = 11,  // end of list (motes enumeration)
   DN_API_RESP_NO_ROOM           = 12,  // reached max.number of items (adding ACL entry) 
                                        // or we're out of memory buffers
	DN_API_RESP_IN_PROGRESS       = 13,  // oper. in progress (exchange and radioTest commands)
   DN_API_RESP_NAK               = 14,  // negative acknowledge
   DN_API_RESP_WRITE_ERROR       = 15,  // flash write failed
   DN_API_RESP_VALIDATION_ERROR  = 16,  // parameters validation error
};
//]------------------------------------------------------------------------

// [ Command specific enumeration -----------------------------------------
enum dn_api_HelloSuccCode {
   DN_API_HELLO_OK   = 0,
   DN_API_UNS_VER    = 1,
   DN_API_INV_MODE   = 2
};

enum dn_api_resetType {
   DN_API_RESET_SYSTEM  = 0,
   DN_API_RESET_NETWORK = 1,     // reserved
   DN_API_RESET_MOTE    = 2              
};  

enum  dn_api_frameProfile {
   MNGR_PROFILE_01    = 1,   // fast  build / medium operation
   MNGR_PROFILE_02    = 2,   // deprecated, maps to PICARD_PROFILE_01
   MNGR_PROFILE_03    = 3,   // deprecated, maps to PICARD_PROFILE_01
   MNGR_PROFILE_98    = 98,  // deprecated, maps to PICARD_PROFILE_01
   MNGR_PROFILE_99    = 99,  // deprecated, maps to PICARD_PROFILE_01
};

enum  dn_api_advState {
   DN_API_ADV_ON,
   DN_API_ADV_OFF,
};

enum  dn_api_multState {
   DN_API_MULT_NORMAL,
   DN_API_MULT_FAST
};

enum dn_api_loc_mode {
   DN_API_LOC_OFF,
   DN_API_LOC_ONDEMAND,
};

enum dn_api_bb_mode { // backbone frame mode
   DN_API_BB_MODE_OFF,
   DN_API_BB_MODE_UP,
   DN_API_BB_MODE_BIDIR
};

enum dn_api_moteState {
   DN_API_MOTE_STATE_LOST,         // mote is not currently part of the network
   DN_API_MOTE_STATE_NEGOT,        // mote is in the process of joining the  network
   DN_API_MOTE_STATE_RSV1,         // not returned, was DN_API_MOTE_STATE_NEGOT2
   DN_API_MOTE_STATE_RSV2,         // not returned, was DN_API_MOTE_STATE_CONNECTED
   DN_API_MOTE_STATE_OPERATIONAL,  // mote is operational
};

enum dn_api_pathFilter {
   DN_API_PATH_FILTER_ALL,
   DN_API_PATH_FILTER_UPSTREAM,
};

enum  dn_api_pathDirection {
   DN_API_PATH_DIR_NA,      // No path
   DN_API_PATH_DIR_NOTUSED, // Path is not used
   DN_API_PATH_DIR_UP,      // Upstream path
   DN_API_PATH_DIR_DOWN,    // Downstream path
};

enum dn_api_userRole {
  DN_API_USER_VIEWER = 0,
  DN_API_USER_ADMIN,
};

enum dn_api_netState {
    DN_API_NETSTATE_OPER    	  = 0,  // Network is operating normally
    DN_API_NETSTATE_RADIOTEST   = 1,  // Manager is in radiotest mode
    DN_API_NETSTATE_NOTSTARTED  = 2,  // Waiting for "startNetwork" API command
    DN_API_NETSTATE_ERR_UNEXP   = 3,  // Unexpected error occurred at startup
    DN_API_NETSTATE_ERR_CONFIG  = 4,  // Invalid or not licensed configuration found at startup
    DN_API_NETSTATE_ERR_LICENSE = 5,  // Invalid license file found at startup
};

// Type of TX radiotest: Packet / CM / CW
enum dn_api_radiotestType {
   DN_API_RADIOTESTTYPE_PKT   = 0,
   DN_API_RADIOTESTTYPE_CM    = 1,
   DN_API_RADIOTESTTYPE_CW    = 2,
};


PACKED_START   // Pack all structures
//[ Common Structures -----------------------------------------------------
typedef struct dn_api_utcTime {
   INT64S     utcSecs;     // unix time in seconds
   int32_t     utcUsecs;    // microseconds offset
} dn_api_utcTime_t;

typedef struct  dn_api_time {
   uint32_t           uptime;      // time (sec) that the packet was generated (as uptime)
   dn_api_utcTime_t utc;         // time that the packet was generated (as UTC)
   uint8_t            asn[5];      // absolute slot number
   uint16_t           offset;      // offset by micro-seconds
} dn_api_time_t;

//]-----------------------------------------------------------------------

// [ Packet Headers -------------------------------------------------------
typedef struct dn_api_request {
   uint8_t  control;    // DN_API_CONTROL_xxx
   uint8_t  type;       // dn_api_appCommand
   uint8_t  seqNo;
   uint8_t  length;     // length of payload 
} dn_api_request_t;

typedef struct dn_api_response {
   dn_api_request_t h;
   uint8_t  respCode;   // dn_api_responseCode
} dn_api_response_t;
//] ----------------------------------------------------------------------

//[ Payload Structure -----------------------------------------------------
// DN_API_MGR_HELLO - manager hello command
typedef struct dn_api_mgrHelloCmd {
   uint8_t  version;    // DN_API_PROTOCOL_VER
   uint8_t  mode;       // dn_api_MNGR_mode
} dn_api_mgrHelloCmd_t;

// DN_API_HELLO  - client hello command
typedef struct dn_api_helloCmd {
   uint8_t  version;    // protocol version (we accept DN_API_PROTOCOL_VER)
   uint8_t  cliSeqNo;   // client's seq.number
   uint8_t  mode;       // dn_api_MNGR_mode
} dn_api_helloCmd_t;

// DN_API_HELLO_RSP   - responce for client hello command
typedef struct dn_api_helloRspCmd {
	uint8_t  successCode;   // dn_api_HelloSuccCode
	uint8_t  version;       // DN_API_PROTOCOL_VER
	uint8_t  mgrSeqNo;      // tell client Manager's seq.num
	uint8_t  cliSeqNo;      // confirm client's seqno received in Hello 
	uint8_t  mode;          // dn_api_MNGR_mode
} dn_api_helloRspCmd_t;

// DN_API_RESET - reset resetCmdFunc
typedef struct dn_api_resetCmd {
   uint8_t  type;       // dn_api_resetType
   uint8_t  mac[DN_API_MAC_SIZE];
} dn_api_resetCmd_t;

typedef struct dn_api_resetRsp {
   uint8_t  mac[DN_API_MAC_SIZE];
} dn_api_resetRsp_t;

// DN_API_SUBSCRIBE - subscribe for notifications
typedef struct dn_api_subscribeCmdDeprecate {
   uint32_t filter;
} dn_api_subscribeCmdDeprecate_t;

typedef struct dn_api_subscribeCmd {
   uint32_t filter;
   uint32_t noRlbl;
} dn_api_subscribeCmd_t;

// DN_API_GET_TIME - Get Current Time - getCurrentTimeCmdFunc
// see struct dn_api_time 

// DN_API_GET_SYSTEM_INFO - Get System Info - getSystemCmdFunc
typedef struct  dn_api_getSysInfoRsp {
    uint8_t   mac[DN_API_MAC_SIZE];// Dust  IEEE address
    uint8_t   hwModel;          // Hardware model (maps to cal.marketingPartNumber)
    uint8_t   hwRev;            // Hardware revision (maps to cal.hardwareVer)
    uint8_t   swMajor;          // Software version, major
    uint8_t   swMinor;          // Software version, minor
    uint8_t   swPatch;          // Software version, patch
    uint16_t  swBuild;          // Software version, build
} dn_api_getSysInfoRsp_t;

// DN_API_GET_NET_CFG - Get Network Configuration (permanent data)
typedef struct  dn_api_getNetworkCfgRsp {
   uint16_t       networkId;
   uint8_t        apTxPower;       
   uint8_t        frameProfile;        // dn_api_frameProfile
   uint16_t       maxMotes;
   uint16_t       baseBandwidth;
   uint8_t        downFrameMultVal;    // power of 2
   uint8_t        numParents;
   uint8_t        enableCCA;           // boolean
   uint16_t       channelList;         // bitmap of available channels
   uint8_t        autoStartNetwork;    // boolean
   uint8_t        locMode;             // location mode (dn_api_loc_mode_t)
   uint8_t        bbMode;              // backbone frame mode (dn_api_bb_mode_t)
   uint8_t        bbSize;              // backbone frame size
   uint8_t        isRadioTest;         // TRUE if mode is radiotest
   uint16_t       bwMult;              // Bandwith overprovision multiplexer
   uint8_t        oneChannel;          // Number of channel in One Channel configuration. 
                                     // 0xFF - one channel configuration is off
} dn_api_getNetworkCfgRsp_t;

// DN_API_GET_NET_INFO - Get Network Information (volatile data)
typedef struct  dn_api_getNetworkInfoRsp {
   uint16_t  numMotes;
   uint16_t  asnSize;             // microseconds
   uint8_t   advertisementState;  // dn_api_advState
   uint8_t   downFrameState;       // dn_api_multState
   uint8_t   netReliability;      
   uint8_t   netPathStability;
   uint32_t  netLatency;          // avg latency, ms
   uint8_t   netState;            // current networ state (see dn_api_netState)
   uint8_t   ip6addr[16];         // IP6 address of manager
} dn_api_getNetworkInfoRsp_t;

// DN_API_SET_NET_CFG - Set Network Parameters (pernmanet data)
typedef struct dn_api_setNetCfgCmd {
   uint16_t       networkId;
   uint8_t        apTxPower;       // boolean
   uint8_t        frameProfile;        // dn_api_frameProfile
   uint16_t       maxMotes;
   uint16_t       baseBandwidth;
   uint8_t        downFrameMultVal;    // power of 2
   uint8_t        numParents;
   uint8_t        ccaEnabled;          // boolean
   uint16_t       channelList;    // bitmap of available channels
   uint8_t        autoStartNetwork;    // boolean
   uint8_t        locMode;             // location mode (dn_api_loc_mode_t)
   uint8_t        bbMode;              // backbone frame mode (dn_api_bb_mode_t)
   uint8_t        bbSize;              // backbone frame size
   uint8_t        isRadioTest;         // TRUE if mode is radiotest
   uint16_t       bwMult;              // Bandwith overprovision multiplexer
   uint8_t        oneChannel;          // channel number for One Channel mode
                                     // 0xFF if mode is off
} dn_api_setNetCfgCmd_t;

// DN_API_GET_MOTE_CFG_BY_ID Get Mote Cfg by MoteID
// Responce is dn_api_getMoteCfgRsp
typedef struct dn_api_getMoteCfgByIdCmd {
   uint16_t moteId;
} dn_api_getMoteCfgByIdCmd_t; 

// DN_API_GET_MOTE_CFG   Get Mote Cfg - getMoteCmdFunc
typedef struct dn_api_getMoteCfgCmd {
   uint8_t   mac[8];
   uint8_t   next;  // true if looking for next mote;
                  // false if looking for this mac
} dn_api_getMoteCfgCmd_t;

typedef struct dn_api_getMoteCfgRsp {
   uint8_t    mac[8];
   uint16_t   moteId;           // id is used in neighbor health reports
   uint8_t    isAP;             // boolean
   uint8_t    state;            // dn_api_moteState
   uint8_t    mobilityType;     // see DN_API_LOCATION_xxx
   uint8_t    isRouting;        // boolean
} dn_api_getMoteCfgRsp_t;

// DN_API_GET_MOTE_INFO Get Mote Information (volatile data)
typedef struct dn_api_getMoteInfoCmd {
   uint8_t   mac[DN_API_MAC_SIZE];
} dn_api_getMoteInfoCmd_t;

typedef struct dn_api_getMoteInfoRsp {
   uint8_t    mac[DN_API_MAC_SIZE];
   uint8_t    state;            // dn_api_moteState, also in getMoteCfg 
   uint8_t    numNbrs;          // # of neighbors discovered (connected or not)
   uint8_t    numGoodNbrs;      // # of good neighbors
   uint32_t   requestedBw;      // bw requested by mote, ms per packet
   uint32_t   totalNeededBw;    // total bw needed at the mote(includes requestedBw ),  ms per packet
   uint32_t   assignedBw;       // currently assigned bw, ms per packet
   uint32_t   packetsReceived;  // total packets received
   uint32_t   packetLost;       // total packets lost      
   uint32_t   avgLatency;       // avg latency in ms
} dn_api_getMoteInfoRsp_t;

// DN_API_GET_MANAGER_INFO
typedef struct dn_api_getManagerInfoRsp {
   // Low level stats:
   uint16_t serTxCnt;        // # of packets sent out on serial port - TODO: 16 bits seems small
   uint16_t serRxCnt;        // # of packets received on serial port - TODO: 16 bits  seems small
   uint16_t serRxCRCErr;     // # of CRC errors
   uint16_t serRxOverruns;   // # of overruns detected

   // Protocol-level stats:
   uint16_t apiEstabConn;    // # of established Serial API connections
   uint16_t apiDropppedConn; // # of dropped Serial API connections
   uint16_t apiTxOk;         // # of request packets sent on serial api for which ack-OK was received
   uint16_t apiTxErr;        // # of request packets sent on serial api for which ack-Error was received
   uint16_t apiTxFail;       // # of packets for which there was no ack
   uint16_t apiRxOk;         // # of request packets that were received and acked
   uint16_t apiRxProtErr;    // # of packets that were received and dropped due to invalid packet format
} dn_api_getManagerInfoRsp_t;

// DN_API_EXCH_JOINKEY - Excahnge Mote Join Key
typedef struct dn_api_exchMoteJoinKeyCmd {
   uint8_t mac[DN_API_MAC_SIZE];
   uint8_t key[DN_API_JOINKEY_SIZE];
} dn_api_exchMoteJoinKeyCmd_t;

typedef struct dn_api_exchMoteJoinKeyRsp {
   uint32_t callbackId;
} dn_api_exchMoteJoinKeyRsp_t;

// DN_API_EXCH_NETWORKID - Exchange Network ID
typedef struct dn_api_exchNetworkIdCmd {
   uint16_t id;
} dn_api_exchNetworkIdCmd_t;

typedef struct dn_api_exchNetworkIdRsp {
   uint32_t callbackId;
} dn_api_exchNetworkIdRsp_t;


// DN_API_RADIOTEST_TX - start radio test
typedef struct dn_api_radiotestTxCmd {
   uint8_t  type;       // Test type. See dn_api_radiotestType
   uint16_t mask;       // mask enabled channels (for CW, CM test one channel only)
   uint16_t numPkts;    // number of packets to transmit
   uint8_t  pktLen;     // length of packet (0 - default max length)
   INT8S  txPower;    // transmit power: -2=min (PA off), 8=max (PA on)
} dn_api_radiotestTxCmd_t;

// DN_API_RADIOTEST_RX
typedef struct dn_api_radiotestRxCmd {
   uint8_t  channel;    // RF channel to use for transmission (0-15)
   uint16_t duration;   // duration of test (in seconds)
} dn_api_radiotestRxCmd_t;

// DN_API_RADIOTEST_STATS
typedef struct dn_api_radiotestStatsRsp {
   uint16_t rxOk;    // number of packets received successfully
   uint16_t rxFail;  // number of packets not received
} dn_api_radiotestStatsRsp_t;


// DN_API_SET_ACL_ENTRY (add or update existing entry)
typedef struct dn_api_setAclEntryCmd {
   uint8_t   mac[DN_API_MAC_SIZE];
   uint8_t   joinKey[DN_API_JOINKEY_SIZE];
} dn_api_setAclEntryCmd_t;

// DN_API_GET_NEXT_ACL_ENTRY (00:00:00:00:00:00:00:00 to start search)
typedef struct dn_api_getNextAclEntryCmd {
   uint8_t   mac[DN_API_MAC_SIZE];
} dn_api_getNextAclEntryCmd_t;

typedef struct dn_api_getNextAclEntryRsp {
   uint8_t   mac[DN_API_MAC_SIZE];
   uint8_t   joinKey[DN_API_JOINKEY_SIZE];
} dn_api_getNextAclEntryRsp_t;

// DN_API_DEL_ACL_ENTRY (FF:FF:FF:FF:FF:FF:FF:FF to clear the whole ACL)
typedef struct dn_api_delAclEntryCmd {
   uint8_t   mac[DN_API_MAC_SIZE];
} dn_api_delAclEntryCmd_t;

// DN_API_PING_MOTE
typedef struct dn_api_pingMoteCmd {
   uint8_t   mac[DN_API_MAC_SIZE];
} dn_api_pingMoteCmd_t;

typedef struct dn_api_pingMoteRsp {
   uint32_t callbackId;
} dn_api_pingMoteRsp_t;

// DN_API_GET_LOG
typedef struct dn_api_getLogCmd {
   uint8_t   mac[DN_API_MAC_SIZE];
} dn_api_getLogCmd_t;

// DN_API_SEND_DATA
typedef struct dn_api_sendDataCmd {
   uint8_t   mac[DN_API_MAC_SIZE];
   uint8_t   priority; 
   uint16_t  srcPort;
   uint16_t  dstPort;
   uint8_t   options;
   // data[...] follows
} dn_api_sendDataCmd_t;

typedef struct dn_api_sendDataRsp {
   uint32_t callbackId;
} dn_api_sendDataRsp_t;

// DN_API_SEND_IP
typedef struct dn_api_sendIpCmd {
  uint8_t   mac[8];
  uint8_t   priority;               // dn_api_packetPriority see dn_api_pktPriority
  uint8_t   options;
  uint8_t   encryptedOffset;        // offset of encrypted data part. DN_ENCRYPT_OFFSET_NONE - no encription
  // data follows
} dn_api_sendIpCmd_t;

typedef struct dn_api_sendIpRsp {
  uint32_t callbackId;
} dn_api_sendIpRsp_t;


// DN_API_GET_PATH
typedef struct dn_api_getPathCmd {
   uint8_t   macSource[DN_API_MAC_SIZE];
   uint8_t   macDest[DN_API_MAC_SIZE];
} dn_api_getPathCmd_t;

typedef struct  dn_api_getPathRsp {
   uint8_t   source[DN_API_MAC_SIZE];   
   uint8_t   dest[DN_API_MAC_SIZE];     
   uint8_t   direction;   // dn_api_pathDirection
   uint8_t   numLinks;
   uint8_t   quality;
   INT8S   rssiSrcDest; // last rssi, source->dest
   INT8S   rssiDestSrc; // last rssi, dest->src
} dn_api_getPathRsp_t;

// DN_API_GET_NEXT_PATH
typedef struct dn_api_getNextPathCmd {
   uint8_t   mac[DN_API_MAC_SIZE];
   uint8_t   filter;  // dn_api_pathFilter
   uint16_t  pathId;
} dn_api_getNextPathCmd_t;

typedef struct  dn_api_getNextPathRsp {
   uint16_t  pathId;      
   uint8_t   source[8];   
   uint8_t   dest[8];     
   uint8_t   direction;   // dn_api_pathDirection
   uint8_t   numLinks;
   uint8_t   quality;
   INT8S   rssiSrcDest; // last rssi, source->dest
   INT8S   rssiDestSrc; // last rssi, dest->src
} dn_api_getNextPathRsp_t;

// DN_API_SET_ADVERTISING
typedef struct dn_api_setAdvertisingCmd {
   uint8_t  activated;    // See DN_API_ADV_xxx
} dn_api_setAdvertisingCmd_t;

typedef struct dn_api_setAdvertisingRsp {
   uint32_t callbackId;
} dn_api_setAdvertisingRsp_t;

// DN_API_SET_DFRAME_SIZE
typedef struct dn_api_setDownFrameSizeCmd {
   uint8_t  frameMode; // see dn_api_multState
} dn_api_setDownFrameSizeCmd_t;

typedef struct dn_api_setDownFrameRsp {
   uint32_t callbackId;
} dn_api_setDownFrameRsp_t;

// DN_API_MEASURE_RSSI
typedef struct dn_api_measureRssiCmd {
   uint16_t duration;
} dn_api_measureRssiCmd_t;

typedef struct dn_api_measureRssiRsp {
   uint32_t callbackId;
} dn_api_measureRssiRsp_t;

// DN_API_SET_TIME
typedef struct dn_api_setTimeCmd {
   uint8_t      useTrigger;  // (0 or 1) flag indicating whether to set time now or as of the previous trigger
   struct dn_api_utcTime  utcTime;     // time that the response was generated (as UTC microseconds)
} dn_api_setTimeCmd_t;

// DN_API_GET_LICENSE
typedef struct dn_api_getLicenseRsp {
   uint8_t   licenseKey[DN_API_LICENSE_SIZE];
} dn_api_getLicenseRsp_t;

// DN_API_SET_LICENSE
typedef struct dn_api_setLicenseCmd {
   uint8_t   licenseKey[DN_API_LICENSE_SIZE];
} dn_api_setLicenseCmd_t;

// DN_API_SET_CLI_USER
typedef struct dn_api_setCliUserCmd {
   uint8_t   role;          // dn_api_userRole
   char    password[DN_API_USRPWD_SIZE];
} dn_api_setCliUserCmd_t;

// DN_API_START_LOCATION
typedef struct dn_api_startLocationCmd {
  uint8_t  numFrames;
  uint8_t  mobileMote[DN_API_MAC_SIZE];
  // data - uint8_t  fixedMotes[MAX_FIXED_MOTES][DN_API_MAC_SIZE];
} dn_api_startLocationCmd_t;

typedef struct dn_api_startLocationRsp {
   uint32_t callbackId;
} dn_api_startLocationRsp_t;

//   DN_API_SET_COMMONJOINKEY - set common join key
typedef struct dn_api_setCommonJoinKeyCmd {
   uint8_t key[DN_API_JOINKEY_SIZE];
} dn_api_setCommonJoinKeyCmd_t;

//   DN_API_GET_IP_CFG - get IP configuration
typedef struct dn_api_getIPCfgRsp {
   uint8_t    ip6prefix[16];
   uint8_t    ip6mask[16];
} dn_api_getIPCfgRsp_t;

//   DN_API_SET_IP_CFG - set IP configuration
typedef struct dn_api_getIPCfgCmd {
   uint8_t    ip6prefix[16];
   uint8_t    ip6mask[16];
} dn_api_getIPCfgCmd_t;

//   DN_API_DEL_MOTE - delete mote
typedef struct dn_api_delMoteCmd {
   uint8_t   mac[DN_API_MAC_SIZE];
} dn_api_delMoteCmd_t;

//[ Notification structure ------------------------------------------------------
typedef struct dn_api_notifData {
   uint8_t   notifType;              // DN_API_NOTIF_DATA
   struct  dn_api_utcTime timestamp;  // time that the packet was generated (in UTC)
   uint8_t   mac[DN_API_MAC_SIZE];      // mac address of the generating mote
   uint16_t  srcPort;                // source port
   uint16_t  dstPort;                // destination port
   // Data
} dn_api_notifData_t;

typedef struct dn_api_notifIpData {
   uint8_t  notifType;               // DN_API_NOTIF_IP_DATA
   struct dn_api_utcTime timestamp;   // time that the packet was generated (in UTC)
   uint8_t  mac[DN_API_MAC_SIZE];       // mac address of the generating mote
   // 6LoWPAN data follows 
} dn_api_notifIpData_t;

typedef struct dn_api_notifHealthRep {
   uint8_t   notifType;             // DN_API_NOTIF_HR
   uint8_t   mac[8];
   // uint8_t payload[];
} dn_api_notifHealthRep_t;

typedef struct dn_api_NotifLog {
   uint8_t  notifType;            // DN_API_NOTIF_LOG
   uint8_t  mac[8];               // Notification source;
   // uint8_t logMsg[];
} dn_api_NotifLog_t;

//[ Event notification: dn_api_notifEvent - - - - - - - - - - - - - - - - - - - - - -
typedef struct dn_api_eventHdr {
   uint8_t           notifType;     // DN_API_NOTIF_EVENT
   uint32_t          eventId;
   uint8_t           eventType;     // dn_api_eventType
} dn_api_eventHdr_t;

// Payload of DN_API_NOTIF_EVENT - notification (depends from eventType)
typedef union dn_api_eventData {
   // DN_API_EV_CMD_FINISH, DN_API_EV_PACKET_SENT
   struct {
      uint32_t  callbackId;
      uint8_t   result;        // 0: OK, non-zero: error (timeout+mote reset)
   }          cmdFinish;    
   // DN_API_EV_PATH_CREATE, DN_API_EV_PATH_DELETE
   struct  {
      uint8_t   source[DN_API_MAC_SIZE];
      uint8_t   dest[DN_API_MAC_SIZE];
      uint8_t   direction;     // dn_api_pathDirection
   }           path;
   // DN_API_EV_PING_RESP
   struct {
      uint32_t     callbackId;
      uint8_t      mac[8];
      int32_t     delay;         // round trip delay in milliseconds or -1: ping timeout
      uint16_t     voltage;       // millivolts
      INT8S      temperature;   // Celsius
   }           pingRsp;
   //DN_API_EV_NET_TIME
   struct dn_api_time time;
   // DN_API_EV_MOTE_RESET, DN_API_EV_MOTE_JOIN, DN_API_EV_MOTE_OPER, 
   // DN_API_EV_MOTE_LOST, DN_API_EV_MOTE_BANDWIDTH
   uint8_t       mac[DN_API_MAC_SIZE];
   // DN_API_EV_MOTE_CREATE, DN_API_EV_MOTE_DELETE
   struct {
      uint8_t    mac[DN_API_MAC_SIZE];
      uint16_t   moteId;
   }           moteinfo;
} dn_api_eventData_t;

typedef struct dn_api_notifEvent {
   struct dn_api_eventHdr  hdr;
   union  dn_api_eventData data;
} dn_api_notifEvent_t;
//] - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//]------------------------------------------------------------------------------

PACKED_STOP


#endif
