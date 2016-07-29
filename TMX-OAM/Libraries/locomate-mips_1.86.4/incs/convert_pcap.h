/* convert_pcap.h contains 802.11 header,
 * LLC  layer header.
 * Related pcap file,pcap pkt, wlan_prism headers  */


#include<stdint.h>
#define u_int8_t uint8_t
#define u_int16_t uint16_t
#define u_int32_t uint32_t
#define __packed __attribute__((packed)) 
#define IEEE80211_ADDR_LEN 6 

struct llc {
    u_int8_t llc_dsap;
    u_int8_t llc_ssap;
    union {
        struct {
        u_int8_t control;
        u_int8_t format_id;
        u_int8_t class;
        u_int8_t window_x2;
        } __packed type_u;
        struct {
        u_int8_t num_snd_x2;
        u_int8_t num_rcv_x2;
        } __packed type_i;
        struct {
        u_int8_t control;
        u_int8_t num_rcv_x2;
        } __packed type_s;
        struct { 
            u_int8_t control;
        /* 
 *          * We cannot put the following fields in a structure because
 *                   * the structure rounding might cause padding.
 *                            */
        u_int8_t frmr_rej_pdu0;
        u_int8_t frmr_rej_pdu1;
        u_int8_t frmr_control;
        u_int8_t frmr_control_ext; 
        u_int8_t frmr_cause;
        } __packed type_frmr;
        struct {
        u_int8_t  control;
        u_int8_t  org_code[3];
        u_int16_t ether_type;
        } __packed type_snap;
        struct {
        u_int8_t control;
        u_int8_t control_ext;
        } __packed type_raw;
    } llc_un /* XXX __packed ??? */;
} __packed;

struct ieee80211_frame {
    u_int8_t    i_fc[2];
    u_int8_t    i_dur[2];
    u_int8_t    i_addr1[IEEE80211_ADDR_LEN];
    u_int8_t    i_addr2[IEEE80211_ADDR_LEN];
    u_int8_t    i_addr3[IEEE80211_ADDR_LEN];
    u_int8_t    i_seq[2];
    /* possibly followed by addr4[IEEE80211_ADDR_LEN]; */
    /* see below */
} __packed;

struct ieee80211_qosframe {
    u_int8_t    i_fc[2];
    u_int8_t    i_dur[2];
    u_int8_t    i_addr1[IEEE80211_ADDR_LEN];
    u_int8_t    i_addr2[IEEE80211_ADDR_LEN];
    u_int8_t    i_addr3[IEEE80211_ADDR_LEN];
    u_int8_t    i_seq[2];
    u_int8_t    i_qos[2];
    /* possibly followed by addr4[IEEE80211_ADDR_LEN]; */
    /* see below */
} __packed;

typedef struct {
    u_int32_t did;
    u_int16_t status;
    u_int16_t len;
    u_int32_t data;
} p80211item_uint32_t;

typedef struct {
    u_int32_t msgcode;
    u_int32_t msglen;
#define WLAN_DEVNAMELEN_MAX 16
    u_int8_t devname[WLAN_DEVNAMELEN_MAX];
    p80211item_uint32_t hosttime;
    p80211item_uint32_t mactime;
    p80211item_uint32_t channel;
    p80211item_uint32_t rssi;
    p80211item_uint32_t sq;
    p80211item_uint32_t signal;
    p80211item_uint32_t noise;
    p80211item_uint32_t rate;
    p80211item_uint32_t istx;
    p80211item_uint32_t frmlen;
} wlan_ng_prism2_header;

typedef  int32_t bpf_int32; 
typedef uint32_t bpf_u_int32;

struct pcap_timeval {
    bpf_int32 tv_sec;           /* seconds */
    bpf_int32 tv_usec;          /* microseconds */
};

struct pcap_pkthdr {
    struct pcap_timeval ts;     /* time stamp */
    bpf_u_int32 caplen;         /* length of portion present */
    bpf_u_int32 len;            /* length this packet (off wire) */
};

struct pcap_file_header {
        bpf_u_int32 magic;
        u_short version_major;
        u_short version_minor;
        bpf_int32 thiszone;     /* gmt to local correction */
        bpf_u_int32 sigfigs;    /* accuracy of timestamps */
        bpf_u_int32 snaplen;    /* max length saved portion of each pkt */
        bpf_u_int32 linktype;   /* data link type (LINKTYPE_*) */
};            

struct file_records{
       char logstring[50];
       char hrlogfilename[100];
       unsigned int file_size;	
       unsigned int seq_num;		
       uint8_t tmp_buff[8192];	
       unsigned int sizeLeft;	
       int fd;
};

/* Each packet log entry consists of the following fixed length header 
 *    followed by variable length log information determined by log_type */
struct widi_loghdr {
    u_int32_t flags;    /* See flags defined below */
    u_int16_t log_type; /* Type of log information foll this header */
    u_int16_t size;       /* Size of variable length log information in bytes */
    u_int8_t ic_index;       /* Size of variable length log information in bytes */
//    u_int32_t timestamp;
        struct timeval time;
} __attribute__ ((packed));

struct GenericDataLong {
    u_int16_t length;
    char contents[1300];
};
typedef struct GenericDataLong WSMData;

#define TX_ON 1
#define RX_ON 2
#define DIRECTION_ENABLED 1
#define INTERFACE_SET 1
#define TX_RX_ON 3
#define TX_RX_ON 3
#define TX_RX_DIRECTION_INTERFACE 1
#define TX_RX_INTERFACE 2
#define TX_RX_DIRECTION 3
#define TX_INTERFACE 4
#define RX_INTERFACE 5
#define TX_DIRECTION 6
#define RX_DIRECTION 7
#define NO_DIRECTION 8
#define MB 1048576
#define PCAP_VERSION_MAJOR 2
#define PCAP_VERSION_MINOR 4
#define TCPDUMP_MAGIC           0xa1b2c3d4

#define CRC_LENGTH 4
#define LINKTYPE_IEEE802_11 105
#define LINKTYPE_PRISM_HEADER 0x0077
#define LINKTYPE_IEEE802_11_RADIO 127
#define SNAPLEN 0xffff /* max length saved portion of each pkt */

#define PRISM_HEADER_LENGTH sizeof(wlan_ng_prism2_header)
#define WLAN_HEADER_LENGTH sizeof(struct ieee80211_qosframe)
#define LLC_HEADER_LENGTH sizeof(struct llc)

#define SECURITY 1	//193
#define DIGEST 	2 	//194
#define MSG_CNT_DIGEST 92	//213
#define MSG_CNT_CERT 21		//284
#define MSG_CNT_PLAIN 9		//201
#define DIGEST_DATA_OFFSET 84
#define CERT_DATA_OFFSET 13
void fill_buffer(char *,int ,int );
