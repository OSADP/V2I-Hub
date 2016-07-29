#ifndef _WAVE_H_
#define _WAVE_H_

#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h> 
#include "queue.h" 
#include "os.h"
#include "chaninfo.h"
#include "gpsc_probe.h"
#ifndef WIN32
#include <sys/time.h> //for gettimeofday
#include <endian.h>
#include <byteswap.h>
#endif
#ifndef htobe16

# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define htobe16(x) bswap_16 (x)
#  define htole16(x) (x)
#  define be16toh(x) bswap_16 (x)
#  define le16toh(x) (x)
#  define htobe32(x) bswap_32 (x)
#  define htole32(x) (x)
#  define be32toh(x) bswap_32 (x)
#  define le32toh(x) (x)
#  define htobe64(x) bswap_64 (x)
#  define htole64(x) (x)
#  define be64toh(x) bswap_64 (x)
#  define le64toh(x) (x)
#else
#  define htobe16(x) (x)
#  define htole16(x) bswap_16 (x)
#  define be16toh(x) (x)
#  define le16toh(x) bswap_16 (x)
#  define htobe32(x) (x)
#  define htole32(x) bswap_32 (x)
#  define be32toh(x) (x)
#  define le32toh(x) bswap_32 (x)
#  define htobe64(x) (x)
#  define htole64(x) bswap_64 (x)
#  define be64toh(x) (x)
#  define le64toh(x) bswap_64 (x)
# endif
#endif /*htobe16*/
#ifdef WIN32
#define BIGENDIAN1 		\
( { 				\
	long x = 0x00000001;	\
   	!(*(char *)(&x));	\
})

#define swap16_(x)       ((((x)&0x00FF)<<8) | (((x)>>8)&0x00FF))

#define swap16(x)  \
({  \
	long __x=(x);\
	((((long)(__x) & (long)0x000000ff)<<8)|\
	 (((long)(__x) & (long)0x0000ff00)>>8));\
})


#define swap32(x)\
({ \
	unsigned long __x = (x);	\
	((unsigned long)(	\
	(((unsigned long)(__x) & (unsigned long)(0x000000ff) << 24 ) |     \
	(((unsigned long)(__x) & (unsigned long)(0x0000ff00) <<  8 ) |     \
	(((unsigned long)(__x) & (unsigned long)(0x00ff0000) >>  8 ) |     \
	(((unsigned long)(__x) & (unsigned long)(0xff000000) >> 24 ) );    \
})

#define swap32_(x) (((x & 0xff)<<24)|((x>>24) & 0xff)|((x & 0x0000ff00)<<8)|((x & 0x00ff0000) >>8))	

#define swap64(x)  	(((x & 0x00000000000000ff)<<56)	 \
					 | ((x & 0xff00000000000000)>>56)\
				     | ((x & 0x000000000000ff00)<<40)\
				     | ((x & 0x00ff000000000000)>>40)\
					 | ((x & 0x0000000000ff0000)<<24)\
					 | ((x & 0x0000ff0000000000)>>24)\
					 | ((x & 0x00000000ff000000)<<8) \
					 | ((x & 0x000000ff00000000)>>8))	




#else
#define BIGENDIAN 		\
( { 				\
	long x = 0x00000001;	\
   	!(*(char *)(&x));	\
})

#define swap16_(x)       ((((x)&0x00FF)<<8) | (((x)>>8)&0x00FF))

#define swap16(x) \
({ \
    long __x = (x); \
    ((long)( \
        (((long)(__x) & (long)0x000000ffUL) <<  8) | \
        (((long)(__x) & (long)0x0000ff00UL) >>  8) )); \
})

#define swap32_(x)       ((((x)&0xFF)<<24)       \
                         |(((x)>>24)&0xFF)       \
                         |(((x)&0x0000FF00)<<8)  \
                         |(((x)&0x00FF0000)>>8)  )
#define swap32(x) \
({ \
    unsigned long __x = (x); \
    ((unsigned long)( \
        (((unsigned long)(__x) & (unsigned long)0x000000ffUL) << 24) | \
        (((unsigned long)(__x) & (unsigned long)0x0000ff00UL) <<  8) | \
        (((unsigned long)(__x) & (unsigned long)0x00ff0000UL) >>  8) | \
        (((unsigned long)(__x) & (unsigned long)0xff000000UL) >> 24) )); \
})

#define swap64_(x)        ((((x)&0x00000000000000FF)<<56) \
                         |(((x)&0xFF00000000000000)>>56) \
                         |(((x)&0x000000000000FF00)<<40) \
                         |(((x)&0x00FF000000000000)>>40) \
                         |(((x)&0x0000000000FF0000)<<24) \
                         |(((x)&0x0000FF0000000000)>>24) \
                         |(((x)&0x00000000FF000000)<<8)  \
                         |(((x)&0x000000FF00000000)>>8))

#define swap64(x1) \
({ \
	unsigned long long x = (x1);\
	(((x & 0xFFULL) << 56| (x & 0xFF00000000000000ULL) >> 56| \
	(x & 0xFF00ULL) << 40| (x & 0xFF000000000000ULL) >> 40| \
	(x & 0xFF0000ULL) <<24| (x & 0xFF0000000000ULL) >> 24| \
	(x & 0xFF000000ULL) <<8 | (x & 0xFF00000000ULL) >>8 ));\
})

#endif
#define OCTET_MAX_LENGTH 32 
#define HALFK 1300
#define FOURK 4096
#define SIXTYFOURK 65000
#define IEEE80211_ADDR_LEN 6
#define ACM_COPY(_a, _b) memcpy((_a), (_b), sizeof(ACM)

#ifdef ACQUILA_CODE_BASE
#define IEEE80211_RATE_MAXSIZE 36
#else
#define IEEE80211_RATE_MAXSIZE 15
#endif
#define MAX_WSA_SIZE 1500
#define GPS_INVALID_DATA -5000.0

#define MACADDRSIZE 17
#define HASHSIZE 32

#define LIBWAVE 1
#define XML 1
#define CSV 2
#define PCAPHDR 3
#define PCAP 4


//**********************Data Structures***************************
typedef struct channelInfo ChannelInfo;
typedef struct {
	int	 len;				/* length in bytes */
	u_int8_t ssid[32];	/* ssid contents */
} ScanSSID;


typedef struct {
	u_int8_t	numrates;
	u_int8_t	rates[IEEE80211_RATE_MAXSIZE];
} RateSet;

typedef struct wsm_hdr{
                u_int8_t version;
                u_int32_t psid;
                struct channelinfo chaninfo;
                u_int8_t txpriority;
                u_int8_t security;
                u_int8_t macaddr[IEEE80211_ADDR_LEN];
        }WSMHDR;

#ifdef	WIN32

#pragma pack(1)
struct GenericDataShort {
    unsigned __int8 		length;
    char     		contents[OCTET_MAX_LENGTH];
} ;

#pragma pack(1)
struct GenericDataLong{
    unsigned __int16 		length;
    char     		contents[HALFK];
} ;

#else
struct GenericDataShort {
    u_int8_t 		length;
    char     		contents[OCTET_MAX_LENGTH];
} __attribute__((__packed__));

struct GenericDataLong{
    u_int16_t 		length;
    char     		contents[HALFK];
} __attribute__((__packed__));
#endif

typedef struct GenericDataShort ACM;
typedef struct GenericDataLong  WSMData;

#define GPS_TIM 0x00000001
#define GPS_LAT 0x00000002
#define GPS_LON 0x00000004
#define GPS_ALT 0x00000008
#define GPS_SPD 0x00000010
#define GPS_DIR 0x00000020
#define GPS_NSV 0x00000040
#define GPS_FIX 0x00000080
#define GPS_HDP 0x00000100
#define GPS_VDP 0x00000200
#define GPS_HEE 0x00000400
#define GPS_VEE 0x00000800
#define GPS_TOD 0x00001000
#define GPS_TSF 0x00002000
#define GPS_TIM_TSF 0x00004000
#define GPS_TOW 0x00008000
#define GPS_BIN 0x01000000
#define GPS_PRE 0x02000000
#define GPS_STG 0x04000000
#define GPS_ERR 0x08000000

typedef struct phyParamType { 
	u_int8_t aifsn; 
	u_int8_t logcwmin;
	u_int8_t logcwmax; 
	u_int16_t txopLimit;
	u_int8_t acm;
} ParamType;

#ifdef WIN32
#pragma pack(1)
#endif
typedef struct {
	u_int16_t length;
	u_int8_t contents;	
	u_int32_t psid;
	char acf[OCTET_MAX_LENGTH];
	u_int8_t priority;
#if 0
#ifdef	WIN32
	struct _IN_ADDR_IPV6  ipv6addr;//modified for WINDOWS
#else
	struct in6_addr ipv6addr;
#endif
#endif
	struct in6_addr ipaddr;
//	u_int32_t reserved[3];	//	hack for ipv6
	u_int16_t serviceport;
	u_int8_t addressing;	
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	u_int8_t channel;
#ifdef WIN32
} PSTEntry;
#else
} __attribute__((__packed__)) PSTEntry;
#endif

#ifdef WIN32
#pragma pack(1)
#endif
typedef struct {
	u_int32_t psid;
	u_int8_t confirm;
	u_int8_t match_any;
	struct in6_addr ipaddr;
	u_int16_t serviceport;
#ifdef WIN32
} USTEntry;
#else
} __attribute__((__packed__)) USTEntry;
#endif
typedef struct {
	u_int8_t acid;
	ACM acm;
	u_int8_t type; // 1 - provider 2 - user
	u_int8_t status; 
	u_int8_t priority;
#if 0
#ifdef	WIN32
	struct _IN_ADDR_IPV6  ipv6addr;//modified for WINDOWS
#else
	struct in6_addr ipv6addr;
#endif
#endif
	struct in6_addr notif_ipaddr;
	u_int16_t notif_port;
#ifdef WIN32
} ASTEntry;
#else
} __attribute__((__packed__)) ASTEntry;
#endif


#ifndef NUM_RPIDENSITY
#define NUM_RPIDENSITY	8
#endif

typedef struct measurement_request_element {
	u_int8_t id;
	u_int8_t length;
	u_int8_t token;
	struct {
		u_int8_t  reserved : 1;
		u_int8_t    enable : 1;
		u_int8_t   request : 1;
		u_int8_t    report : 1;
	} mode;
	u_int8_t type; 
	struct {
		u_int8_t    channel;
		u_int64_t startTime;
		u_int16_t  duration;	
	} request;
} MeasurementRequestElement;



typedef struct measurement_report_element {
	u_int8_t id;
	u_int8_t length;
	u_int8_t token;
	struct {
		u_int8_t      late : 1;
		u_int8_t incapable : 1;
		u_int8_t   refused : 1;
	} mode;
	u_int8_t type; //deterimines how to set the var field: 0-Basic, 1-CCA, 2-RPI
	struct {
		u_int8_t    channel;
		u_int64_t startTime;
		u_int16_t  duration;	
	} report;
	union {
			struct {
				u_int8_t      late : 1;
				u_int8_t incapable : 1;
				u_int8_t   refused : 1;
			} map;
			u_int8_t ccabusyfraction;
			u_int8_t rpidensity[NUM_RPIDENSITY];
	} var;
} MeasurementReportElement;


typedef struct measurment_frame {
	u_int8_t category;
	u_int8_t action;
	u_int8_t dialogtoken;
	struct {
		MeasurementRequestElement	requestelem;
		MeasurementReportElement   	reportelem;
	} body;
} MeasurementFrame;


typedef struct wrssrequest_request {
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	MeasurementRequestElement wrssreq_elem;
} WMEWRSSRequest;

#ifdef	WIN32
#pragma pack(1)
struct wrss_report {
	u_int8_t    channel;
	u_int64_t measurementTime;
	u_int8_t  wrss;	
};
#else
struct wrss_report {
	u_int8_t    channel;
	u_int64_t measurementTime;
	u_int8_t  wrss;	
} __attribute__((__packed__));
#endif
typedef struct wrss_report WMEWRSSReport;


typedef struct wrssrequest_indication {
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	u_int8_t dialogtoken;
	WMEWRSSReport wrssreport;
} WMEWRSSRequestIndication;


struct wmeNotif_Indication {
              
   enum
   {
       CHANNEL_ASSIGNED=1,
       NO_CHANNEL_ASSIGNED=2,
       REQUEST_MATCHED=3,
   }RecvIndication_event;

   int local_service_index;
   enum
   {
       UNSPECIFIED,
       REQUESTED,
       CHANNEL_UNAVAILABLE,
       SERVICE_COMPLETE,
       REQUESTED_FRAME_SCHEDULED,
       PRIORITY_PREEMPTION,
       SECURITYCREDENTILALFAILURE,
   }RecvIndication_reason;

} __attribute__((__packed__));


#define IEEE80211_COUNTRY_MAX_TRIPLETS (83)
struct ieee80211_ie_country {
        u_int8_t        country_id;
        u_int8_t        country_len;
        u_int8_t        country_str[3];
        u_int8_t        country_triplet[IEEE80211_COUNTRY_MAX_TRIPLETS*3];
} __attribute__((__packed__));

#if 1
struct wme_ta_info_element {
    uint8_t id;
    uint8_t length;
    uint8_t timing_cap;
    uint8_t timevalue[10];
    uint8_t timingerror[5];
} __attribute__((__packed__));
#endif
struct WmeTAIndication {
    u_int64_t tsf;
    u_int64_t localtsf;
    u_int8_t macaddr[6];
    int16_t rcpi;
    uint8_t channel;
    struct timeval time;
    struct ieee80211_ie_country ic_country_ie;
    uint8_t power_constraint[3];
    struct wme_ta_info_element ta_info;	
} __attribute__((__packed__));

   
#ifdef	WIN32 
#pragma pack(1) 
#endif
typedef struct {
    u_int8_t action;
	//u_int8_t temp[3]; // Patching done for  1276 build only
    union {
        PSTEntry pstentry;
        USTEntry ustentry;
    } entry;
#if 0
#ifdef	WIN32
	struct _IN_ADDR_IPV6  ipv6addr;//modified for WINDOWS
#else
	struct in6_addr ipv6addr;
#endif
#endif
	struct in6_addr notif_ipaddr;
	u_int16_t notif_port;
#ifdef	WIN32 
} WMEApplicationRegistrationRequest; 
#else 
} __attribute__((__packed__)) WMEApplicationRegistrationRequest;
#endif

struct ieee80211_scan_ssid {
        int      len;                           /* length in bytes */
        u_int8_t ssid[32];      /* ssid contents */
};

typedef struct {
	u_int32_t psid;
	char  acf[OCTET_MAX_LENGTH];
	u_int8_t priority;
	u_int8_t requestType;
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	u_int8_t repeats;	
	u_int8_t persistence;
	u_int8_t channel;		
        u_int16_t localserviceid;    //local service identifier
        u_int8_t action;       //ADD, CHANGE, DELETE
        u_int8_t servmac[6];
        u_int8_t psclen;
        char psc[32];
        u_int8_t wsatype;         //secured or unsecured wsa
        u_int8_t channelaccess;   /*channel access type
                                  1 -> provider access sch in both sch and cch
                                  interval (continous access)
                                  0 -> provider access sch in sch interval 
                                         (alternating access) */
        u_int8_t repeatrate;      /*number of VSA frames to be transmitted per 5  
                                  seconds. ignored in case of unicast frames.*/
        u_int8_t ipservice;       //TRUE / FALSE
        struct in6_addr ipv6addr;
        u_int16_t serviceport;
        u_int8_t rcpithres;      /*Value in dBm below which, the service 
                                 should be ignored by the recipient   */
        u_int8_t wsacountthres;  /*minimum number of WSAs recieved below which
                                  below which the service should be ignored 
                                  by the recipient */
        //USER specific info
        u_int8_t userreqtype; /*1: auto access. allows SCH access if it matches 
                                   an available service info
                                2: auto access, unconditional. allows SCH access 
                                   irrespective of available services
                                3: No sch access  */
        struct ieee80211_scan_ssid ssid;
        u_int8_t linkquality;
        u_int8_t schaccess;
        u_int16_t schextaccess;
	u_int16_t notif_port;
} __attribute__((__packed__)) WMEApplicationRequest;

struct location_2d {
    uint32_t latitude;
    uint32_t longitude;
}__attribute__((__packed__));

struct location_3d {
    uint32_t latitude;
    uint32_t longitude;
    uint16_t elevation;
    uint8_t pos_elev_confidence;
    uint32_t pos_accuracy;
} __attribute__((__packed__));

struct location {
        struct location_2d loc_2d;
        struct location_3d loc_3d;
}__attribute__((__packed__));

struct ieee80211_rateset {
        u_int8_t                rs_nrates;
        u_int8_t                rs_rates[IEEE80211_RATE_MAXSIZE];
};  


struct availserviceInfo {
    uint8_t wsatype;
    uint8_t srcmac[6];
    uint32_t psid;
    uint8_t servicepriority;
    uint8_t psc[32]; // ProviderServiceContext
    struct in6_addr ipv6addr; 
    uint8_t serviceport;
    uint8_t providermac[6];
    uint8_t rcpithresh;
    int16_t rcpi;
    uint8_t wsacntthresh;
    struct channelinfo channelinfo;   
    struct location provider_loc;
    uint8_t linkquality;
    uint8_t servicestatus;
    uint8_t wsa_count;
    uint8_t psclen;
    struct ieee80211_scan_ssid ssid;
    struct ieee80211_rateset basicRateSet;
    struct ieee80211_rateset opRateSet;
    uint8_t txpower;
    struct in6_addr defaultgw;
    struct in6_addr wra_ipprefix;
    struct in6_addr primaryDNS;
    struct in6_addr secondaryDNS;
    uint8_t prefix_length;
    uint8_t defaultgw_mac[6];
    uint8_t certificate;
    uint8_t ipservice;
    //struct availserviceInfo *ast_info;	

}__attribute__((__packed__))GETAvailableServiceInfo;



typedef struct {
    uint8_t action;
    uint8_t repeatrate;
    uint8_t channel;
    uint8_t channelinterval;
    uint8_t servicepriority;

    
} __attribute__((__packed__)) WMETARequest;


    //WRA extension fields
struct wsa_wra_ext_fields {
    struct in6_addr secondaryDNS;
    uint8_t gwmacaddr[IEEE80211_ADDR_LEN];
} __attribute__((__packed__));

struct wsa_wra {
    u_int8_t wra_elementid;
    u_int16_t router_lifetime;
    struct in6_addr ipPrefix;
    u_int8_t prefix_length;
    struct in6_addr defaultgw;
    struct in6_addr primaryDNS;
} __attribute__((__packed__));

typedef struct {
    struct location_3d loc_3d;
    struct ieee80211_scan_ssid adv_identifier;
    struct wsa_wra wra;
    struct wsa_wra_ext_fields wra_ext;
    uint8_t isupdated;
} __attribute__((__packed__)) WMEWSAConfig;

typedef struct  {
	u_int8_t linkConfirm;
} WMEApplicationResponse;

enum {
   USER_REQ_SCH_ACCESS_AUTO = 1,
   USER_REQ_SCH_ACCESS_AUTO_UNCONDITIONAL,
   USER_REQ_SCH_ACCESS_NONE,
};



typedef struct {
	u_int8_t event;
	u_int8_t reason;
	ACM acm;
	char acf[OCTET_MAX_LENGTH];
	u_int8_t certificate;
	struct in6_addr ipaddr;
	u_int16_t serviceport;
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	u_int8_t gateway;
	u_int8_t gateway_macaddr[IEEE80211_ADDR_LEN];
	ChannelInfo chaninfo;
	u_int8_t bssid[IEEE80211_ADDR_LEN];
	ScanSSID ssid;
	u_int64_t timestamp;
	u_int64_t localtime;
	RateSet bssRateSet;
	RateSet opRateSet;
	ParamType wmeparam[4];
	WMEWRSSReport wrssreport;
} WMENotificationIndication ;

typedef struct {
	u_int8_t acid;
	ACM acm;
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	struct in6_addr ipaddr;
	u_int8_t persistence;
} WMEApplicationIndication;

typedef struct {
        u_int32_t packetnum;
        u_int8_t rssi;
        u_int8_t macaddr[IEEE80211_ADDR_LEN];
} additionalWSMP;

#ifdef	WIN32
#pragma pack(1)
struct wsm_packet
{
	u_int8_t version;
	u_int8_t security;	
	u_int8_t channel;
	u_int8_t rate;
	u_int8_t txpower;
	u_int8_t app_class;
	ACM acm;
	WSMData data;
};
#else
struct wsm_packet
{
	u_int8_t version;
	u_int8_t security;	
	u_int8_t channel;
	u_int8_t rate;
	u_int8_t txpower;
	u_int8_t app_class;
	ACM acm;
	WSMData data;
} __attribute__((__packed__));
#endif

typedef struct wsm_packet WSMPacket;

struct wsm_header {
    u_int8_t version;
    u_int32_t psid;
    u_int8_t channel;
    u_int8_t rate;
    int8_t txpower;
};
typedef struct wsm_header WSMHeader;

#ifdef	WIN32
#pragma pack(1)
struct ip_packet {
	u_int32_t packetnum;
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	u_int8_t rssi;
	WSMData data;
};
#else
struct ip_packet {
	u_int32_t packetnum;
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	u_int8_t rssi;
	WSMData data;
} __attribute__((__packed__));
#endif
typedef struct ip_packet IPPacket;


#ifdef WIN32
#pragma pack(1)
struct wsm_request {
	u_int32_t psid;
	struct channelinfo chaninfo;
	u_int8_t version;
	u_int8_t security;
	u_int8_t txpriority;
	u_int8_t wsmps;
	u_int64_t expirytime;
	WSMData data;
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	u_int8_t srcmacaddr[IEEE80211_ADDR_LEN];

};
#else
struct wsm_request {
	u_int32_t psid;
	struct channelinfo chaninfo;
	u_int8_t version;
	u_int8_t security;
	u_int8_t txpriority;
	u_int8_t wsmps;
	u_int64_t expirytime;
	WSMData data;
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	u_int8_t srcmacaddr[IEEE80211_ADDR_LEN];
} __attribute__((__packed__));
#endif
typedef struct wsm_request WSMRequest;

#ifdef WIN32

#pragma pack(1)
struct wsm_indication {
	struct channelinfo chaninfo;
	u_int32_t psid;
	u_int8_t version;
	u_int8_t txpriority;
	u_int8_t wsmps;
	u_int8_t security;
	WSMData data;
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	u_int8_t rssi;
}; 

#else
struct wsm_indication {
	struct channelinfo chaninfo;
	u_int32_t psid;
	u_int8_t version;
	u_int8_t txpriority;
	u_int8_t wsmps;
	u_int8_t security;
	WSMData data;
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	u_int8_t rssi;
} __attribute__((__packed__));
#endif
typedef struct wsm_indication WSMIndication;


typedef struct {
	u_int8_t result;
	u_int64_t timer;
} TSFTimer ;


typedef struct {
	u_int8_t aci;
	u_int8_t channel;
} WMECancelTxRequest;

#ifdef	WIN32

#pragma pack(1)
struct waveHandler
{
    u_int16_t length;
    u_int16_t type;
    char data[1400];
};
#else
struct waveHandler
{
    u_int16_t length;
    u_int16_t type;
    char data[1400];
} __attribute__((__packed__));
#endif


typedef struct waveHandler WAVEHandler;

struct HostApplicationInfo {
	u_int16_t	notif_port;
	u_int16_t	service_port;
	struct in6_addr 	notif_ipaddr;
	struct in6_addr	service_ipaddr;
#if 0
#ifdef WIN32
	struct _IN_ADDR_IPV6  ipv6addr;//modified for WINDOWS
#else
	struct in6_addr ipv6addr;
#endif
#endif
	u_int8_t acid;
	ACM acm;
	struct HostApplicationTable *hat;
	TAILQ_ENTRY(HostApplicationInfo) hainfo_list;
	LIST_ENTRY(HostApplicationInfo) hainfo_hash;
};


struct HostApplicationTable {
	TAILQ_HEAD ( hostapp, HostApplicationInfo) application;
	ATH_LIST_HEAD (hosthash , HostApplicationInfo) tablehash[HASHSIZE];
};


enum {
    ERROR_ENTRY_PRESENT = -1,
    ERROR_NO_ENTRY = -2 ,
    ERROR_INVALID = -3
};
enum {
    WAVE_REQUEST ,
    WAVE_INDICATION
};
enum {
    WME_ADD_PROVIDER = 1 ,
    WME_DEL_PROVIDER = 2,
    WME_ADD_USER = 3,
    WME_DEL_USER = 4
};
enum {
    APP_PROVIDER = 1,
    APP_USER,
    APP_USER_UPDATE
};

//ioctl to be change
enum {
    	WAVE_APP_REGISTER,
    	WAVE_APP_REQUEST,
		WAVE_APP_DELETE,
		WAVE_GET,
    	WAVE_SET,
    	WME_APP_NOTIFICATION,
	WME_APP_INDICATION,
	WAVE_APP_RESPONSE,
	WAVE_WRSS_REQUEST,
	WAVE_WRSS_INDICATION,
	WAVE_WSA_INDICATION,
	WAVE_TA_INDICATION,
	WAVE_TSFTIMER_REQUEST,
	WAVE_SETTSFTIMER_REQUEST,
	WAVE_TSFTIMER_INDICATION,
	WAVE_CANCELTX_REQUEST,
	WAVE_WSM_REQUEST,
        WAVE_USERSRV_REQUEST,
        WAVE_TA_REQUEST,
        WAVE_CCHSERVICE_REQUEST,
        WAVE_MGMTDATASERVICE_REQUEST,
        WAVE_WSM_SERVICE_REQUEST,
	WAVE_MACADDR_REQUEST,
	WAVE_WSA_CONFIG,
    WAVE_MAXTXPOWER_REQUEST,
	WAVE_AVAILSERVICEINFO_REQUEST
};

enum {  
    TA_ADD = 0x1,
    TA_CHANGE,
    TA_DELETE 
};


enum {
	APP_INACTIVE = 1,
	APP_ACTIVE,
	APP_UNAVAILABLE
};

enum {
	REASON_UNSPEC,
	REASON_APP_REQUEST,
	REASON_RADIO_SWITCH,
	REASON_CHAN_INACTIVITY,
	REASON_APP_COMPLETE,
	REASON_PRIORITY,
	REASON_SECURITY
};

enum {
	EVENT_WBSS_ACTIVE,
	EVENT_WBSS_INACTIVE,
	EVENT_WBSS_UNAVAILABLE,
	EVENT_WBSS_TERMINATE,
	EVENT_WRSS_REPORT_RECIEVED,
	EVENT_WSA_ENCRYPT
};


enum {
	RESULT_SUCCESS = 0,
	RESULT_UNSPEC_FAILURE = -1,
	RESULT_INVALID_PARAMS = -2,
	RESULT_PSID_CONFLICT = -3,
	RESULT_INSUFF_PRIORITY = -4
};


enum {
	SERVERR_PORT_ALLOCED = 100,
	SERVERR_PSID_ALLOCED,
	SERVERR_MEMORY
};

enum {
	INTERSECTIONID=1,
	INTERSECTIONSTATUS,
	MSGTIMESTAMP,
	MOVEMENT,
	LANESET,
	CURSTATE,
	MINTIMEREMAIN,
	MAXTIMEREMAIN,
	YELLOWSTATE,
	YELLOWTIME,
	PEDESTRAIN,
	PEDESORVEHICLECOUNT,
	ENDOFBLOB,
};

#define WAVE_PSID 0x0001
#define WAVE_ACM 0x0002
#define WAVE_ACF 0x0004
#define WAVE_PRIORITY 0x0008
#define WAVE_CHANNEL 0x0010
#define WAVE_IPV6ADDR 0x0020
#define WAVE_PEERMAC 0x0040

#define CHACCESS_ALTERNATIVE 1 //pvk as per the standard 1-alternative & 0-Continuous
#define CHACCESS_CONTINUOUS 0 //pvk

//***************WAVE API Public-Functions*********************************
#define BUFSIZE 600
#define WAVE_UDPSERVER_PORT 	9999
#define WAVE_UDPSERVER_IP 	"127.0.0.1"
#define WME_APPIND_PORT		10023
enum { 
	WAVEDEVICE_LOCAL =	1,
	WAVEDEVICE_REMOTE
};

u_int8_t table_hash(u_int8_t acid, ACM acm);
int allocHostAppl(struct HostApplicationInfo *hainfo);
void freeHostAppl(struct HostApplicationInfo *hainfo);
struct HostApplicationInfo * findHostAppl(u_int8_t acid, ACM acm);
/*
******** Driver Init Funcitons*********
*/
void inputhandler(int signo);
int invokeWAVEDevice(int type, int blockflag);
int invokeWAVEDriver(int blockflag);
int invokeWAVEServer(void);
int invokeWAVEClient(uint16_t *, uint16_t *);
int setRemoteDeviceIP(char *ipaddr);
void registerWMENotifIndication ( void (*wmeNotifIndCallBack)(WMENotificationIndication *) );
void registerWSMIndication ( void (*wsmIndCallBack)(WSMIndication *) );
void registerWRSSIndication ( void (*wrssIndCallBack)(WMEWRSSRequestIndication *) );
void registertsfIndication(void (*tsfTimerIndCallBack)(TSFTimer *));
//void registerLinkConfirm( int (*confirmLinkCallBack)(unsigned __int8 , ACM) );
void registerLinkConfirm( int (*confirmLinkCallBack)(WMEApplicationIndication *) );
void user_registerLinkConfirm( int (*user_confirmLinkCallBack)(struct wmeNotif_Indication *) );
void registerWaveMsgConfirm(int (*WaveMsgConfirm)(struct wsm_confirm *));

/*
******************EXIT Function************
*/
void closeWAVEDevice();

/*
******** Application-libwave Funcitons*********
*/

int setWMEApplRegNotifParams(WMEApplicationRequest *req);
int registerProvider(int pid, WMEApplicationRequest *appreq);
int Get_Available_Serviceinfo(int pid, struct availserviceInfo *get_astinfo,uint32_t);
int removeProvider(int pid, WMEApplicationRequest *appreq);
int removeAll(void);
int transmitTA(WMETARequest *tareq);
int wsa_config(WMEWSAConfig *wsaconfig);
int registerUser(int pid, WMEApplicationRequest *appreq);
int removeUser(int pid, WMEApplicationRequest *appreq);
int startWBSS(int pid, WMEApplicationRequest *req);
int stopWBSS(int pid, WMEApplicationRequest *req);
int makeUnavailableWBSS(int pid, WMEApplicationRequest *req);
int getWRSSReport(int pid, WMEWRSSRequest *req);
u_int64_t getTsfTimer(int pid);
u_int64_t setTsfTimer(uint64_t tsf);
int txWSMPacket(int pid, WSMRequest *req);
int rxWSMPacket(int pid, WSMIndication *ind);
int __txWSMPacket(int pid, void *txbuf, int size);
int __rxWSMPacket(int pid, void *rxbuf);
int cancelTX(int pid, WMECancelTxRequest *req);
void getUSTIpv6Addr(struct in6_addr *ustAddr, char *ifName);
/*
******** libwave-WME  Funcitons*********
*/

int generateWMEApplRequest(WMEApplicationRequest *req);
int generateWMEApplRegRequest(void *req);
int generateWMEApplDelRequest(void);
int generateWMETARequest(WMETARequest *req);
int generateWSAConfigfill(WMEWSAConfig * req);
int generateWMEWRSSRequest(WMEWRSSRequest *req);
u_int64_t generatetsfRequest();
u_int64_t settsfRequest(uint64_t);
int generateMaxtxpowReq();
int generateWMEApplResponse(WMEApplicationResponse *req);
int generateWSMRequest(WSMRequest *req);
int __generateWSMRequest(void *txbuf, int size);
int generateWMECancelTxRequest(WMECancelTxRequest *req);
void getMACAddr(uint8_t *, uint8_t); 
void generate_random_mac(uint8_t *); 

int get_date(int Tdate);
int get_time(int Ttime);
#endif

