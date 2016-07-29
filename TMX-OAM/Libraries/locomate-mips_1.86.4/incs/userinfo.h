#ifndef __USERINFO_H__
#define __USERINFO_H__
#include "wsmp.h"

struct userInfo {
	
	uint32_t psid;
	uint8_t confirm;
	uint8_t match_any;
#ifdef WAVE_DRAFT_1_4
        uint8_t userreqtype;
        uint8_t priority;
        uint8_t pr_mac[6];
#ifdef ENABLE_HANDOFF
        uint8_t curprovmac[6];
#endif
	struct ieee80211_scan_ssid ssid;
        uint8_t regclass;
        uint8_t channel;
        uint8_t linkquality;
        uint8_t schaccess;
        uint16_t schextaccess;
        uint8_t servicestatus; 
	struct in6_addr ipaddr;
	uint16_t serviceport;
	uint8_t ipservice;
#endif
	struct wme_user_table *ut;
	TAILQ_ENTRY(userInfo) ui_list;
	LIST_ENTRY(userInfo) ui_hash;
} __attribute__((__packed__));


struct wme_user_table {
	TAILQ_HEAD (, userInfo) ut_user;
	ATH_LIST_HEAD (, userInfo) ut_hash[HASHSIZE];
	wme_tablelock_t lock;
};


struct location {
        struct location_2d loc_2d;
        struct location_3d loc_3d;
}__attribute__((__packed__));


struct availserviceInfo {
//****** UserAvailableServiceTableIndex   	
    uint8_t wsatype;
//****** UserAvailableServiceResultCode   	
//****** UserAvailableGenerationTime   	
//****** UserAvailableLifeTime   	
//****** UserAvailableExpectedCrlTime
    uint8_t srcmac[6];
    uint32_t psid;
//****** UserAvailableServiceSpecificPermissions   	
    uint8_t servicepriority;
    uint8_t psc[32]; // ProviderServiceContext
    struct in6_addr ipv6addr;
    uint8_t serviceport;
    uint8_t providermac[6];
    uint8_t rcpithresh;
//    uint8_t rcpi;
    int16_t rcpi;
#ifdef ENABLE_HANDOFF
    uint8_t wsacps;
#endif
    uint8_t wsacntthresh;
#ifdef ENABLE_HANDOFF
    uint8_t wsacntinterval;
#endif
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
    uint8_t certificate; //TODO: check the type
    uint8_t ipservice;
    struct wme_avialable_servicetable *ast;
    TAILQ_ENTRY(availserviceInfo) ast_list;
    LIST_ENTRY(availserviceInfo) ast_hash;

} __attribute__((__packed__));

struct wme_avialable_servicetable {
    TAILQ_HEAD (, availserviceInfo) ast_user;
    ATH_LIST_HEAD (, availserviceInfo) ast_hash[HASHSIZE];
    wme_tablelock_t lock;

};

struct userInfo *
wme_find_user(struct wme_user_table *pt,
	uint32_t psid);
struct userInfo *
find_user_by_provmac(struct wme_user_table *ut,uint8_t * macaddr);
struct userInfo *
wme_alloc_user(struct wme_user_table *ut, void *ustentry);

void
wme_free_user(struct userInfo *uinfo);

void
wme_free_user_list(struct wme_user_table *ut);


struct availserviceInfo *
wme_find_availserviceinfo(struct wme_avialable_servicetable *ast,
                                  uint32_t psid, uint8_t *macaddr);

uint8_t
wme_find_channel_bypsid(struct wme_avialable_servicetable *ast,
                                  uint32_t psid);

void
wme_free_availserviceinfo(struct availserviceInfo *astinfo);

struct userInfo *wme_find_matchinguser(struct wme_user_table *ut,
                                       struct availserviceInfo *astinfo);

struct availserviceInfo * 
wme_find_matchingservice(struct wme_avialable_servicetable *ast,
                               struct userInfo * uinfo );

void
wme_free_availserviceinfo_list(struct wme_avialable_servicetable *ast);
#endif
