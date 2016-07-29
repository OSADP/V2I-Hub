#ifndef __WME_H__
#define __WME_H__
#include "wsmp.h"
#include "providerinfo.h"
#include "userinfo.h"
#include "appinfo.h"
#include "channelaccess.h"
#include "wsminfo.h"

#define WME_NUM_AC      4   /* 4 AC categories */
    
#define WME_PARAM_ACI       0x60    /* Mask for ACI field */
#define WME_PARAM_ACI_S     5   /* Shift for ACI field */
#define WME_PARAM_ACM       0x10    /* Mask for ACM bit */
#define WME_PARAM_ACM_S     4   /* Shift for ACM bit */
#define WME_PARAM_AIFSN     0x0f    /* Mask for aifsn field */
#define WME_PARAM_AIFSN_S   0   /* Shift for aifsn field */
#define WME_PARAM_LOGCWMIN  0x0f    /* Mask for CwMin field (in log) */
#define WME_PARAM_LOGCWMIN_S    0   /* Shift for CwMin field */
#define WME_PARAM_LOGCWMAX  0xf0    /* Mask for CwMax field (in log) */
#define WME_PARAM_LOGCWMAX_S    4   /* Shift for CwMax field */

struct wmelocalinfo {
	struct ieee80211_scan_ssid ssid;
	struct ieee80211_rateset opRateSet;
	uint8_t num_chan;
	uint16_t registration_port;
	uint16_t forward_port;
	uint16_t wsmmaxlen;
};

struct wme {
	uint8_t wme_macaddr[IEEE80211_ADDR_LEN];
	struct wmelocalinfo wme_localinfo;
	struct wme_provider_table wme_providertable;
	struct wme_user_table wme_usertable;
	struct wme_application_table wme_apptable;
	struct wme_channel_table wme_chantable;
        struct wme_avialable_servicetable wme_availservtable;
        struct wme_chan_access_table wme_chanaccesstable;
        struct wme_cch_service_table wme_cchservicetable;
        struct wme_wsmserviceinfo_table wme_wsmservicetable;
        struct wme_linkQualityParams wme_linkquality;
};

#endif
