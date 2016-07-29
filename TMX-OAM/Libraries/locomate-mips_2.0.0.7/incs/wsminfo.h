#ifndef __WSM_INFO__
#define __WSM_INFO__

struct wme_wsmserv {
    u_int32_t psid;
    TAILQ_ENTRY(wme_wsmserv) wsminfo_list;
    LIST_ENTRY(wme_wsmserv) wsminfo_hash; 
} __attribute__((__packed__));

struct wme_wsmserviceinfo_table {
    TAILQ_HEAD (, wme_wsmserv) wsmt_head;
    ATH_LIST_HEAD (, wme_wsmserv) wsmt_hash[HASHSIZE];
    uint8_t bm[8];
    wme_tablelock_t lock;
} __attribute__((__packed__));

enum {
    WSMSERV_ADD = 1,
    WSMSERV_DELETE
};

struct wme_wsmserv * wme_find_wsmservinfo(
                       struct wme_wsmserviceinfo_table *wsm_t,
                       uint32_t psid);
int8_t wme_alloc_wsmservinfo(struct wme_wsmserviceinfo_table *wsm_t,
                               uint32_t psid);

void wme_free_wsmservinfo(struct wme_wsmserviceinfo_table *wsm_t,
                               uint32_t psid);



#endif
