#ifndef __APPINFO_H
#define __APPINFO_H
#include "wsmp.h"
enum appclassid {
	ACID_SYSTEM,
	ACID_FEECOLLECTION	
};

struct applicationInfo {
	uint32_t psid;
	uint8_t acid;
	wme_acm acm;
	uint8_t type; // 1 - provider 2 - user
	uint8_t status; 
	uint8_t priority;
#if 0	
	struct in6_addr notif_ipv6addr;
#endif	
	struct in6_addr notif_ipaddr;
	uint16_t notif_port;
    struct radio *radio;
	struct wme_application_table *at;
	TAILQ_ENTRY(applicationInfo) ai_list;
	LIST_ENTRY(applicationInfo) ai_hash;
	
} __attribute__((__packed__));


struct wme_application_table {
	TAILQ_HEAD (, applicationInfo) at_application;
	ATH_LIST_HEAD (, applicationInfo) at_hash[HASHSIZE];
	wme_tablelock_t lock;
};

struct applicationInfo *
wme_find_application(struct wme_application_table *pt,
	uint32_t psid);
struct applicationInfo *
wme_alloc_application(struct wme_application_table *at, struct astEntry *astentry,struct radio *radio);

void
wme_free_application(struct applicationInfo *ainfo);
void
wme_free_application_list(struct wme_application_table *at);

int
activeApps (struct wme_application_table *at, struct radio *radio);
//int anyApp (struct wme_application_table *at);

#endif
