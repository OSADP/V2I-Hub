#ifndef __PROVIDERINFO_H__
#define __PROVIDERINFO_H__
#include "wsmp.h"

struct providerInfo {
#ifdef WAVE_DRAFT_1_4
//service info
    //This has to be the first element in this structure
    struct  serviceinfo serviceinfo;  
//ext fields
    struct ext_field ext;
        /*Above order should match to that in  struct pstEntry*/
//following  parameters are part of MIB only
        uint16_t localserviceid;
        uint8_t wsatype;
        uint8_t channel;
        uint8_t channelaccess; 
        uint8_t repeatrate;
        uint8_t ipservice;
        uint8_t servicestatus;
        uint8_t regclass;
        uint8_t *tahandler;
#else
	uint16_t length;
	uint8_t contents;	
	//uint8_t acid;
	uint8_t psid;
	wme_acm acm;
	char acf[OCTET_MAX_LENGTH];
	uint8_t priority;
	struct in6_addr ipaddr;
//	uint32_t reserved[3];	//	hack for ipv6
	uint16_t serviceport;
	uint8_t addressing;	
	uint8_t macaddr[IEEE80211_ADDR_LEN];
	uint8_t channel;
#endif
	struct wme_provider_table *pt;
	TAILQ_ENTRY(providerInfo) pi_list;
	LIST_ENTRY(providerInfo) pi_hash;
} __attribute__((__packed__));

struct wme_provider_table {
	TAILQ_HEAD (, providerInfo) pt_provider;
	ATH_LIST_HEAD (, providerInfo) pt_hash[HASHSIZE];
	wme_tablelock_t lock;
};

struct providerInfo *
wme_find_provider(struct wme_provider_table *pt,
	uint32_t psid);
struct providerInfo *
wme_alloc_provider(struct wme_provider_table *pt, void *pstentry, struct radio *radio);
void
wme_free_provider(struct providerInfo *pinfo);



#endif
