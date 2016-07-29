#ifndef __PIDINFO_H_
#define __PIDINFO_H_
#include "wsmp.h"

struct psidlist {
	uint32_t psid;
	TAILQ_ENTRY(psidlist) entries;	
};

struct pidInfo {
    	u_int16_t pid;
    	uint16_t  token;
	u_int8_t acid;
	//u_int32_t psid;
	wme_acm acm;
	layer4addr udpaddr;

	TAILQ_HEAD(, rx_buf) rxqhead;
	TAILQ_HEAD(, wsmp_buf) wsmpqhead;
	TAILQ_HEAD(, wsmp_buf) process_wsmpqhead;
	TAILQ_HEAD(, psidlist) psidlist_head;

	WME_WSMP_LOCK_T wsmplock;
	WME_RX_LOCK_T rxlock;

	struct wsmp_buf *wsmpbufs;
	struct waveHandler *wavehandlers;
	struct pid_table *pt;
	TAILQ_ENTRY(pidInfo) pid_list;
	LIST_ENTRY(pidInfo) pid_hash;

};

struct pid_table {
	TAILQ_HEAD (, pidInfo) pt_pidinfo;
	ATH_LIST_HEAD (, pidInfo) pt_hash[HASHSIZE];
	wme_tablelock_t lock;
};

struct pidInfo *
wme_find_pidinfo_bypid(struct pid_table *pt, 
	uint16_t pid);
struct pidInfo *
wme_find_pidinfo_bypsid(struct pid_table *pt, 
	uint32_t psid);
struct pidInfo *
wme_find_pidinfo_bytoken(struct pid_table *pt, 
	uint16_t token);
struct pidInfo *
wme_modify_pidinfo(struct pid_table *pt, uint16_t pid, uint16_t token, uint32_t psid, layer4addr *udpaddr);
struct pidInfo *
wme_alloc_pidinfo(struct pid_table *pt, uint16_t pid, uint16_t token, uint32_t psid, layer4addr *udpaddr);
void
wme_free_pidinfo(struct pidInfo *pidinfo);
#endif
