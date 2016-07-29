#ifndef __ASNWAVE_H_
#define __ASNWAVE_H_

//#include <wave.h>
#include <wave.h>
#include <asn_application.h>
#include <asn_internal.h>

enum {
	WSMMSG_INVALID,
	WSMMSG_ALACARTE,
	WSMMSG_BSM,
	WSMMSG_BSMVERBOSE,
	WSMMSG_CSR,
	WSMMSG_EVA,
	WSMMSG_ICA,
	WSMMSG_MAPDATA,
	WSMMSG_NMEA,
	WSMMSG_PDM,
	WSMMSG_PVD,
	WSMMSG_RSA,
	WSMMSG_RTCM,
	WSMMSG_SPAT,
	WSMMSG_SRM,
	WSMMSG_SSM,
	WSMMSG_TIM,
};

#ifdef WIN32
#pragma pack(1)

struct wsm_message {
	u_int16_t type;
	void *structure;
	u_int16_t decode_status; //0 - success
	WSMIndication wsmIndication;
};

#else

struct wsm_message {
	u_int16_t type;
	void *structure;
	u_int16_t decode_status; //0 - success
	WSMIndication *wsmIndication;
} __attribute__((__packed__));

#endif
typedef struct wsm_message WSMMessage;

//int txWSMMessage(int pid, WSMRequest *req);
int rxWSMMessage(int pid, WSMMessage *msg);
int rxWSMIdentity(WSMMessage *msg, int );

int xml_print(WSMMessage msg);

#endif //  __ASNWAVE_H_
