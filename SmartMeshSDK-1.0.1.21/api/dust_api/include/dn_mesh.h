/*
 * Copyright (c) 2010, Dust Networks.  All rights reserved.
 *
 * $HeadURL$
 * $Rev$
 * $Author$
 * $LastChangedDate$ 
 */

 
#ifndef _DN_MESH_H
#define _DN_MESH_H

#include "dn_typedef.h"

// Timeslot length
#define DN_IP_USEC_PER_TS                    7250

// Dust epoch: seconds since 20:00 07/02/2002 GMT
#define DN_IP_ASN0_UTC_SEC                   1025665200

// size of ASN
#define DN_ASN_SIZE                          5

// size of 128-bit key 
#define DN_KEY128_SIZE                       16

// size of mac address
#define DN_MACADDR_SIZE                      8
#define DN_MESH_LONG_ADDR_SIZE               DN_MACADDR_SIZE

// size of Serial Number
#define DN_SERNUM_SIZE                       8

// size of short address
#define DN_MESH_SHORT_ADDR_SIZE              2

// size of IPV6 address
#define DN_IPV6_ADDR_SIZE                    16

// size of nonce for NET MIC calculations
#define DN_SEC_NONCE_SIZE                    13

// 802.15.4 defines
#define DN_802154_MAXPK_SIZE                 127 // excludes len

// minimum overhead for MAC: frame control(2), sec(2), short src/dest (4), mic(4), crc(2)
#define DN_MIN_MAC_OVRHEAD                   14                   

// max size of mac payload
#define DN_MAX_MAC_PAYLOAD                   (DN_802154_MAXPK_SIZE-DN_MIN_MAC_OVRHEAD)

#define DN_UDP_CTRL_PORT                    DN_UDP_PORT_BASE_12BITS

   
#define DN_UDP_EXT_MNGR_PORT              0xF0B0                       // Port for manager commands
#define DN_UDP_EXT_APP_PORT               0xF0B1                       // Port for external application commands
#define DN_UDP_LOC_PORT                   0xF0B2                       // Location port
#define DN_UDP_DATA_PORT                  0xF0B3                       // Default data port
#define DN_UDP_SERAPI_PORT                0xF0B8                       // Serial API pass-through
#define DN_UDP_OAP_PORT                   0xF0B9                       // Oski Application Protocol

// to refactor

#define DN_MGR_SHORT_ADDR                      0xFFFE
#define DN_MESH_BCAST_ADDR                     0xFFFF
#define DN_UDP_PORT_BASE_8BITS                 0xF000
#define DN_UDP_PORT_BASE_12BITS                0xF0B0

PACKED_START

// ASN string
typedef struct {
   uint8_t   byte[DN_ASN_SIZE];
}  dn_asn_t;

// UTC time <sec, usec>
typedef struct {
   INT64S  seconds;
   int32_t  useconds;
}  dn_utc_time_t;

// ASN-based time
typedef struct {
   dn_asn_t  asn;
   uint16_t    offset;    //ASN offset by micro-seconds
}  dn_asn_time_t;

// Security key 
typedef uint8_t  dn_key_t[DN_KEY128_SIZE];

// MAC/Mesh layer MAC address
typedef uint8_t  dn_macaddr_t[DN_MACADDR_SIZE];

typedef  struct {
   uint8_t  byte[DN_IPV6_ADDR_SIZE];
}  dn_ipv6_addr_t;

// network id
typedef uint16_t dn_netid_t;

// mote id
typedef uint16_t dn_moteid_t;

extern const dn_ipv6_addr_t DN_MOTE_IPV6_BCAST_ADDR;
extern const dn_ipv6_addr_t DN_MGR_IPV6_MULTICAST_ADDR;
extern const dn_ipv6_addr_t DN_IPV6_LOCALHOST_ADDR;
extern const dn_macaddr_t DN_MGR_LONG_ADDR;
extern const dn_macaddr_t DN_BCAST_LONG_ADDR;
extern const dn_macaddr_t DN_ZERO_LONG_ADDR;
extern const dn_asn_t DN_ZERO_ASN;

PACKED_STOP

#endif // DN_MESH_H
