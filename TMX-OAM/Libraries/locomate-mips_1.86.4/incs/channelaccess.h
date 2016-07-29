#ifndef __CHANNELACCESS_H__
#define __CHANNELACCESS_H__
#include "wsmp.h"

struct wme_channel_access_node {
    u_int8_t sch_imm_access;
    u_int16_t sch_ext_access;
    u_int8_t channel;
    u_int32_t psid;
    u_int8_t servicepriority;
    TAILQ_ENTRY(wme_channel_access_node) ca_list;
}__attribute__((__packed__));

struct wme_chan_access_table {
    TAILQ_HEAD (, wme_channel_access_node) ca_head;
    wme_tablelock_t lock;
}__attribute__((__packed__));

struct wme_cchserv {
    u_int16_t localserviceid;
    u_int8_t channelinterval;
    u_int8_t servicepriority;
    TAILQ_ENTRY(wme_cchserv) cchserv_list;

}__attribute__((__packed__));

struct wme_cch_service_table {
    TAILQ_HEAD (, wme_cchserv) cchserv_head;
    wme_tablelock_t lock;
}__attribute__((__packed__));
#endif
