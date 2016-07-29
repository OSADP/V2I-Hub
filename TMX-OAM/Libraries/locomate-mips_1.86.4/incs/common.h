#ifndef __COMMON_H_
#define __COMMON_H_
#include <linux/types.h>
#include <linux/in6.h>
#include <linux/in.h>
#include "chaninfo.h"
#define OCTET_MAX_LENGTH 32// Needs to be 255
#define HALFK 1300
#define IEEE80211_ADDR_LEN 6
#define RSSI_LEN 1

#ifdef ACQUILA_CODE_BASE
#include "ieee80211_mlme_wave_defs.h"
#else
struct ieee80211_scan_ssid {
	int	 len;				/* length in bytes */
	u_int8_t ssid[32];			/* ssid contents */
};
#endif


#define BIGENDIAN 		\
( { 				\
	long x = 0x00000001;	\
   	!(*(char *)(&x));	\
})

#define swap16(x) \
({ \
    long __x = (x); \
    ((long)( \
        (((long)(__x) & (long)0x000000ffUL) <<  8) | \
        (((long)(__x) & (long)0x0000ff00UL) >>  8) )); \
})


#define swap32(x) \
({ \
    unsigned long __x = (x); \
    ((unsigned long)( \
        (((unsigned long)(__x) & (unsigned long)0x000000ffUL) << 24) | \
        (((unsigned long)(__x) & (unsigned long)0x0000ff00UL) <<  8) | \
        (((unsigned long)(__x) & (unsigned long)0x00ff0000UL) >>  8) | \
        (((unsigned long)(__x) & (unsigned long)0xff000000UL) >> 24) )); \
})


#define swap64(x1) \
({ \
	unsigned long long x = (x1);\
	(((x & 0xFFULL) << 56| (x & 0xFF00000000000000ULL) >> 56| \
	(x & 0xFF00ULL) << 40| (x & 0xFF000000000000ULL) >> 40| \
	(x & 0xFF0000ULL) <<24| (x & 0xFF0000000000ULL) >> 24| \
	(x & 0xFF000000ULL) <<8 | (x & 0xFF00000000ULL) >>8 ));\
})

#define WAVE_VERSION 1
#define WAVE_HEADER_ELE_ID 1
#define WAVE_SERVINFO_ELEID 1
#define WAVE_WRA_ELEID 3
#define WAVE_EXTINFO_ELEID 0x11 //TODO
#define WAVE_CHANINFO_ELEID 2
#define WAVE_WSM_ELEID 128
#define WAVE_WSMPS_ELEID 129
#define WAVE_WSM_EXTINFO_ELEID 0x12 //TODO

#define PSC_WAVE_ELE_ID 8
#define IPADDR_WAVE_ELE_ID 9
#define SERVPORT_WAVE_ELE_ID 10
#define MACADDR_WAVE_ELE_ID 11
#define RCPI_THRESH_WAVE_ELE_ID 19
#ifdef ENABLE_HANDOFF
    #define WSACNT_THRESH_WAVE_ELE_ID 20
#else
    #define WSACNT_WAVE_ELE_ID 20
#endif
#define WSACNT_INTER_WAVE_ELE_ID 22
#define EDCA_ELE_ID 12
#define CHANNEL_ACCESS_ELE_ID 21
#define HEADER_TXPOWER_ELE_ID 4
#define HEADER_2DLOCATION_ELE_ID 5
#define HEADER_3DLOCATION_ELE_ID 6
#define HEADER_ADV_IDENTIFIER_ELE_ID 7
#define HEADER_REPEATRATE_ELE_ID 17
#define HEADER_CTRY_ELE_ID 18
#define WSM_CHANNEL_ELE_ID 15
#define WSM_RATE_ELE_ID 16
#define WSM_TXPOWER_ELE_ID  4 
#define WRA_SECDNS_ELE_ID 13 
#define WRA_GWMAC_ELE_ID 14 


struct wmeacm {
    u_int8_t length;
    char    contents[OCTET_MAX_LENGTH];
} __attribute__((__packed__));

typedef struct wmeacm wme_acm;

struct wmewsmdata {
    u_int16_t length;
    char    contents[HALFK];
} __attribute__((__packed__));

typedef struct wmewsmdata wme_wsmdata;


#define ACM_COPY(_a, _b) memcpy((_a), (_b), sizeof(wme_acm))
#ifdef ACQUILA_CODE_BASE
#define IEEE80211_RATE_MAXSIZE 36
#else
#define IEEE80211_RATE_MAXSIZE 15
#endif


struct ieee80211_rateset {
	u_int8_t		rs_nrates;
	u_int8_t		rs_rates[IEEE80211_RATE_MAXSIZE];
};

typedef struct phyParamType { 
	u_int8_t aifsn; 
	u_int8_t logcwmin;
	u_int8_t logcwmax; 
	u_int16_t txopLimit;
	u_int8_t acm;
} paramType;

struct serviceinfo {
    u_int8_t wave_elementid;
    u_int32_t  psid;
    u_int8_t priority;
    u_int8_t channelidx;
}  __attribute__((__packed__)); 

struct extention_field {
    u_int8_t ele_id;
    u_int8_t length;
    u_int8_t contents[255];
} __attribute__((__packed__));

struct ext_field {
    uint8_t psc[32];
    uint8_t psclen;
    struct in6_addr ipv6addr;
    u_int16_t serviceport;
    u_int8_t macaddr[IEEE80211_ADDR_LEN];
    uint8_t rcpithresh;
    uint8_t wsacntthresh;
} __attribute__((__packed__));

struct wsm_header {
    u_int8_t version;
    u_int32_t psid;
    u_int8_t channel;
    u_int8_t rate;
    int8_t txpower;
};

    //WRA structures
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


struct pstEntry {
#ifdef WAVE_DRAFT_1_4
//service info
        //This has to be the first element on this structure
       struct serviceinfo serviceinfo;
//ext fields
       struct ext_field ext; 
#else
	u_int16_t length;
	u_int8_t contents;	
	u_int32_t psid;
	char acf[OCTET_MAX_LENGTH];
	u_int8_t priority;
	struct in6_addr ipaddr;
//	u_int32_t reserved[3];	//	hack for ipv6
	u_int16_t serviceport;
	u_int8_t addressing;	
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	u_int8_t channel;
#endif
}__attribute__((__packed__));

#ifdef WAVE_DRAFT_1_4
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

struct wave_header_ext_field_content {
    uint8_t repeatrate;
    uint8_t txpower;
    struct location_2d loc_2d;
    struct location_3d loc_3d;
     struct ieee80211_scan_ssid ssid;
    int8_t  ctry_string[3];
} __attribute__((__packed__));


#endif
struct ustEntry {
	u_int32_t psid;
	u_int8_t confirm;
	u_int8_t match_any;
	struct in6_addr ipaddr;
	u_int16_t serviceport;
#ifdef WAVE_DRAFT_1_4
        uint8_t userreqtype;
        uint8_t priority;
        uint8_t srcmac[6];
	struct ieee80211_scan_ssid ssid;
        uint8_t regclass;
        uint8_t channel;
        uint8_t linkquality;
        uint8_t schaccess;
        uint16_t schextaccess;
        uint8_t servicestatus; 
#endif
}__attribute__((__packed__));

struct astEntry {
	u_int32_t psid;
//	u_int8_t acid;
//	wme_acm acm;
	u_int8_t type; // 1 - provider 2 - user
	u_int8_t status; 
	u_int8_t priority;
	struct in6_addr notif_ipaddr;
	u_int16_t notif_port;
}__attribute__((__packed__));

struct wme_linkQualityParams {
    uint8_t macaddr[6];
    uint8_t wsacntthresh;
    uint8_t wsacntper5secs;
    struct timer_list  wme_waveendtimer;
     
} __attribute__((__packed__));

struct ApplRegisterRequest {
    u_int8_t action;
    union {
        struct pstEntry pstentry;
        struct ustEntry ustentry;
    } entry;
    struct in6_addr notif_ipaddr;
    u_int16_t notif_port;
} __attribute__((__packed__));// needs to be here to do remote login from Windows 


struct ApplRequest {
	u_int32_t psid;
	char  acf[OCTET_MAX_LENGTH];
        u_int8_t priority;
	u_int8_t requestType;
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	u_int8_t repeats;	
	u_int8_t persistence;
	u_int8_t channel;	
#ifdef WAVE_DRAFT_1_4
        u_int16_t localserviceid;    //local service identifier
        u_int8_t action;       //ADD, CHANGE, DELETE
        u_int8_t servmac[6];
        u_int8_t psclen;
        u_int8_t psc[32];
        u_int8_t wsatype;         //secured or unsecured wsa
        u_int8_t channelaccess;   /*channel access type
                                  0 -> provider access sch in both sch and cch
                                  interval (continous access)
                                  1 -> provider access sch in sch interval 
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
#endif	
}__attribute__((__packed__));

enum {
   USER_REQ_SCH_ACCESS_AUTO = 1,
   USER_REQ_SCH_ACCESS_AUTO_UNCONDITIONAL, 
   USER_REQ_SCH_ACCESS_NONE,
};

struct ApplResponse {
	u_int8_t linkconfirm;
};

struct ApplTARequest {
    uint8_t action;
    uint8_t repeatrate;
    uint8_t channel;
    uint8_t channelinterval;
    uint8_t servicepriority;
} __attribute__((__packed__));

struct ApplWSAConfig {
    struct location_3d loc_3d;
    struct ieee80211_scan_ssid adv_identifier;
    struct wsa_wra wra;
    struct wsa_wra_ext_fields wra_ext;
    uint8_t isupdated;
}__attribute__((__packed__));

struct ApplCCHRequest {
    uint8_t action;
    uint16_t localserviceid;
    uint8_t channelinterval;
    uint8_t servicepriority;
} __attribute__((__packed__));

struct ApplMgmtdataRequest {
    uint8_t action;
    uint8_t channel;
    uint8_t channelinterval;
    uint8_t macaddr[6];
    uint8_t mgmt_id;
    uint8_t servicepriority;
    uint8_t repeatrate;
    uint8_t data[255];
} __attribute__((__packed__));

struct ApplWsmServiceReq {
    uint16_t localserviceid;
    uint8_t action;
    uint32_t psid;
} __attribute__((__packed__)); 

struct wrss_report {
	u_int8_t    channel;
	u_int64_t measurementTime;
	u_int8_t  wrss;	
} __attribute__((__packed__));;


struct WmeNotificationIndication {
	u_int8_t event;
	u_int8_t reason;
	wme_acm acm;
	char acf[OCTET_MAX_LENGTH];
	u_int8_t certificate;
	struct in6_addr ipaddr;
	u_int16_t serviceport;
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	u_int8_t gateway;
	u_int8_t gateway_macaddr[IEEE80211_ADDR_LEN];
	struct channelInfo chaninfo;
	u_int8_t bssid[IEEE80211_ADDR_LEN];
	struct ieee80211_scan_ssid ssid;
	u_int64_t timestamp;
	u_int64_t localtime;
	struct ieee80211_rateset bssRateSet;
	struct ieee80211_rateset opRateSet;
	paramType wmeparam[4];
	struct wrss_report wrssreport;
};
struct WmeApplicationIndication {
//	u_int8_t acid;
	u_int32_t psid;
	wme_acm acm;
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	struct in6_addr ipaddr;
	struct in6_addr ipv6addr;
	uint8_t persistence;	
};

struct WmeWRSSRequestIndication {
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	u_int8_t dialogtoken;
	struct wrss_report wrssreport;
};


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
    u_int8_t    country_id;
    u_int8_t    country_len;
    u_int8_t    country_str[3];
    u_int8_t    country_triplet[IEEE80211_COUNTRY_MAX_TRIPLETS*3];
} __packed;

#if 1
struct wme_ta_info_element {
    uint8_t id;
    uint8_t length;
    uint8_t timing_cap;
    uint8_t timevalue[10];
    uint8_t timingerror[5];
} __packed;
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

#ifndef NUM_RPIDENSITY
#define NUM_RPIDENSITY 8
#endif

struct measurement_request_element {
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
};


struct measurement_report_element {
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
};

struct measurment_frame {
	u_int8_t category;
	u_int8_t action;
	u_int8_t dialogtoken;
	union {
		struct measurement_request_element requestelem;
		struct measurement_report_element   reportelem;
	} body;
};

struct wrssrequest_request {
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	struct measurement_request_element wrssreq_elem;
};

struct wrssrequest_indication {
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	u_int8_t dialogtoken;
	struct measurement_report_element reportelem;
};


typedef enum 
{
 PUBLIC_SAFETY=19,
 VEHICLE_SAFETY,
}Providerserviceid;

struct wsmrequest {
	u_int32_t psid;
	struct channelinfo chaninfo;
	u_int8_t version;
	u_int8_t security;
	u_int8_t txpriority;
	u_int8_t wsmps;
        u_int64_t expirytime;
	wme_wsmdata data;
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	u_int8_t srcmacaddr[IEEE80211_ADDR_LEN];

} __attribute__((__packed__));
 
typedef struct wsmrequest wsm_request;
#if 0
struct wsm_ext_field {
    u_int8_t ext_elementid;
    u_int8_t length;
    u_int8_t channel;
    u_int8_t rate;
    int8_t txpower; 
} __attribute__((__packed__));

struct wsm_header {
    uint8_t version;
    uint32_t psid;
    struct  wsm_ext_field wsm_ext;
    uint8_t wsm_eleid;
    uint16_t wsmlen;
} __attribute__((__packed__));
#endif 

struct wsmindication {
        struct channelinfo chaninfo;
	u_int32_t psid;
	u_int8_t version;
	u_int8_t txpriority;
	u_int8_t wsmps;
        u_int8_t security;
	wme_wsmdata data;
	u_int8_t macaddr[IEEE80211_ADDR_LEN];
	u_int8_t rssi;
} __attribute__((__packed__));

typedef struct wsmindication wsm_indication;


struct wme_canceltx_request {
	u_int8_t aci;
	u_int8_t channel;
};

struct wsa_serviceinfo {
    u_int32_t psid;
    u_int8_t priority;
    u_int8_t channelindex;
    uint8_t psc[32];
    uint8_t psclen;
    struct in6_addr ipv6addr;
    u_int16_t serviceport;
    u_int8_t macaddr[IEEE80211_ADDR_LEN];
    uint8_t rcpithresh;
    uint8_t wsacntthresh;
    uint8_t wsacntinterval;
} __attribute__((__packed__));

struct wme_wsaservice_table {
    int count;
    struct wsa_serviceinfo service[32];
 
} __attribute__((__packed__));

struct wsa_channelinfo {
    u_int8_t elementid;
    u_int8_t reg_class;
    u_int8_t channel;
    u_int8_t adaptable;
    u_int8_t rate;
    int8_t txpower;
    struct edcaparam edca[4];
    u_int8_t channelaccess;
} __attribute__((__packed__));

struct wme_wsachannel_table {
    int count;
    struct wsa_channelinfo channelinfo[32];
} __attribute__((__packed__));


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
    WME_DEL_USER = 4,
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
	RESULT_INSUFF_PRIORITY = -4,
	RESULT_INSUFF_MEMORY = -5
};

/*Channel interval*/
enum {
    CHAN_INT_CCH = 0x1, //only cch interval
    CHAN_INT_SCH,     //only sch interval
    CHAN_INT_SCH_CCH  //both sch and cch interval
};

enum {
    WME_MGMT_DATA_ADD = 1,
    WME_MGMT_DATA_CHANGE = 2,
    WME_MGMT_DATA_DELETE = 3
};
#define FALSE   0           
#define TRUE    1           


struct waveHandler
{
    u_int16_t length;
    u_int16_t type;
    char data[1400];
} __attribute__((__packed__));

struct l4addr{
	struct in6_addr ip;
	uint16_t port;
} __attribute__((__packed__));

typedef struct l4addr layer4addr;

struct udpData
{
	struct in6_addr ip;
	uint16_t port;
	unsigned char buf[1400];
	int len;
}__attribute__((__packed__));


#define APPINDICATION_PORT 10023

#define ACID 0x0001
#define ACM 0x0002
#define ACF 0x0004
#define PRIORITY 0x0008
#define CHANNEL 0x0010
#define IPV6ADDR 0x0020
#define PEERMAC 0x0040

enum {
    TA_ADD = 0x1,
    TA_CHANGE,
    TA_DELETE 
};

uint8_t wme_setbit(uint8_t *bm, uint32_t psid);
uint8_t wme_issetbit(uint8_t *bm, uint32_t psid);
void wme_clearbit(uint8_t *bm, uint32_t psid);
void wme_swapGenericData(int size, void *data);
#endif
