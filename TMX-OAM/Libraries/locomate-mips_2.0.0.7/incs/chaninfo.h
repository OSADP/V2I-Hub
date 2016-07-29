#ifndef __CHANINFO_H_
#define __CHANINFO_H_

#define	CHAN_RATE  0x1
#define	CHAN_TXPOWER 0x2
#define CHAN_ADAPTABLE 0x4

struct edcaparam {
    u_int8_t aifsn;
    u_int8_t logcwmin;
    u_int8_t logcwmax; 
    u_int16_t txopLimit;
    u_int8_t acm;
} ;

struct channel_fixedparams {
        u_int8_t elementid;
        u_int8_t reg_class;
        u_int8_t channel;
        u_int8_t adaptable;
        u_int8_t rate;
        int8_t txpower;
} __attribute__((__packed__));

struct channel_extfield {
        struct edcaparam edca[4];
        u_int8_t channelaccess;
} __attribute__((__packed__));

#ifdef WIN32
#define u_int8_t	unsigned __int8
#pragma pack(1)
struct channelInfo {
        u_int8_t elementid;
        u_int8_t reg_class;
        u_int8_t channel;
        u_int8_t adaptable;
        u_int8_t rate;
        int8_t txpower;
//ext field
        struct edcaparam edca[4];
        u_int8_t channelaccess;
};
#else
//This channel info structure is specific to contents to be transmitted in WSA frame
struct channelInfo {
        u_int8_t elementid;
        u_int8_t reg_class;
        u_int8_t channel;
        u_int8_t adaptable;
        u_int8_t rate;
        int8_t txpower;
//ext field
        struct edcaparam edca[4];
        u_int8_t channelaccess;
} __attribute((__packed__));
#endif

struct channelinfo {
        u_int8_t reg_class;
        u_int8_t channel;
        u_int8_t adaptable;
        u_int8_t rate;
        int8_t txpower;
        struct edcaparam edca[4];
        u_int8_t channelaccess;
}  __attribute__((__packed__));


#define MAX_CHAN 15
struct wme_channel_table {
	u_int8_t count;
	struct channelInfo chaninfo[MAX_CHAN];
//        u_int8_t channelnumber[MAX_CHAN];
//	wme_tablelock_t lock;
};
#endif
