/*

* Copyright (c) 2005-2007 Arada Syatems, Inc. All rights reserved.

* Proprietary and Confidential Material.

*

*/


#ifndef __WAVE_L_
#define __WAVE_L_
#ifdef	WIN32
#define	uint_8		unsigned __int8
#define uint16_t	unsigned __int16
#endif
#include "wave.h"
#include "queue.h"

typedef int bpf_int32;
typedef u_int bpf_u_int32;

#define FILE_SIZE_EXCEDDED -100

struct entry {

	uint64_t totalPackets;
	uint64_t lastSeqNoRcvd;
	uint64_t startPacketNo;
	uint64_t outOfOrderPackets;
	uint32_t lostPackets;
	uint32_t latePackets;
	long sec;
	long usec;
	char src[20];
	TAILQ_ENTRY(entry) si_list;
	LIST_ENTRY(entry) si_hash;
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

struct pcap_pkthdr {
        struct timeval ts;      /* time stamp */
        bpf_u_int32 caplen;     /* length of portion present */
        bpf_u_int32 len;        /* length this packet (off wire) */
};

struct src_loss_table {
	TAILQ_HEAD(st_user, entry) st_user;
	ATH_LIST_HEAD (st_hash, entry) st_hash[HASHSIZE];
};


/* Circular buffer related functions and structures */
#define CBLOGSZ 500
typedef struct {
    uint16_t length;
    char data[1024];
} logpacket;

typedef struct {
    int size;           /* maximum number of elements           */
    int start;          /* index of oldest element              */
    int end;            /* index at which to write new element  */
    logpacket   *entries;  /* vector of elements                   */
} circbuflog;

int cblogIsFull(circbuflog *cb);
int cblogIsEmpty(circbuflog *cb);

char *get_logfile();
void set_logfile(char *filename);
int open_log(uint32_t psid);
int close_log();
int write_logentry(char *logentry, int len);
void set_logging_mode(uint8_t mode);
void set_logging_addr(struct sockaddr_in ip, uint16_t port);
void set_logging_format(uint8_t format);
void set_packet_delay(uint64_t packetDelay);
void start_logging();
void stop_logging();

int build_gps_logentry(uint8_t logtype, char *buf, WSMIndication *wsm, additionalWSMP *nodeinfo, GPSData *gps, int gpscontents);
int build_gps_csventry(uint8_t logtype, char *buf, WSMIndication *wsm, additionalWSMP *nodeinfo, GPSData *gps, int gpscontents);
int build_gps_xmlentry(uint8_t logtype, char *buf, WSMIndication *wsm, additionalWSMP *nodeinfo, GPSData *gps, int gpscontents);
char *get_logfile();
uint8_t hash_scr(char *source_mac_address);
#endif
