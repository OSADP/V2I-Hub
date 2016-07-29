/**
 * @file         WaveRadio.h
 * @author       Joshua Branch
 * 
 * @copyright Copyright (c) 2014 Battelle Memorial Institute. All rights reserved.
 * 
 * @par
 * Unauthorized use or duplication may violate state, federal and/or
 * international laws including the Copyright Laws of the United States
 * and of other international jurisdictions.
 * 
 * @par
 * @verbatim
 * Battelle Memorial Institute
 * 505 King Avenue
 * Columbus, Ohio  43201
 * @endverbatim
 * 
 * @brief
 * TBD
 * 
 * @details
 * TBD
*/

#ifndef _WAVERADIO_H_
#define _WAVERADIO_H_

#include <inttypes.h>
#include <asn_application.h>
#include <time.h>


typedef struct {
	struct timespec rxTime;
	asn_TYPE_descriptor_t *type;
	void *structure;
} WaveRxPacket;

typedef struct WaveRadioOptions {

	uint8_t security;
	int txPower;
	uint8_t txPriority;
	int txRate;
	uint8_t wsmps;
	int pid;

} WaveRadioOptions;

void waveRadio_setOptions(WaveRadioOptions *options);

void waveRadio_txPacket(asn_TYPE_descriptor_t *msgType, void *msgStruct, int psid, int chan);
void waveRadio_txRawPacket(uint8_t *data, int dataLength, int psid, int chan);

WaveRxPacket *waveRadio_createRxPacket(asn_TYPE_descriptor_t *msgTypeDes, void *structure);
void waveRadio_destroyRxPacket(WaveRxPacket *packet);

#endif
