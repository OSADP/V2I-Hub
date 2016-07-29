/**
 * @file         WaveRadio.c
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

#include "WaveRadio.h"

#include <asnwave.h>
#include <wave.h>
#include <tool_def.h>

static WaveRadioOptions waveOptions;

void waveRadio_setOptions(WaveRadioOptions *options)
{
	memcpy(&waveOptions, options, sizeof(WaveRadioOptions));
}

void waveRadio_txPacket(asn_TYPE_descriptor_t *msgType, void *msgStruct, int psid, int chan)
{
	WSMRequest request;
	// Fill request header information
	request.chaninfo.channel    = chan;
	request.chaninfo.rate       = waveOptions.txRate;
	request.chaninfo.txpower    = waveOptions.txPower;
	request.version             = 2;
	request.psid                = psid;
	request.wsmps               = waveOptions.wsmps;
	request.txpriority          = waveOptions.txPriority;
	request.security            = waveOptions.security;
	request.macaddr[0] = 255;
	request.macaddr[1] = 255;
	request.macaddr[2] = 255;
	request.macaddr[3] = 255;
	request.macaddr[4] = 255;
	request.macaddr[5] = 255;
	//request.expirytime = ? ;
	getMACAddr(request.srcmacaddr, chan);
	
	memset(&request.data, 0, sizeof(WSMData));
	
	asn_enc_rval_t rvalenc = der_encode_to_buffer(msgType, msgStruct, &request.data.contents, HALFK);
 
	if(rvalenc.encoded != -1)
	{
		request.data.length = rvalenc.encoded;
		int ret = txWSMPacket(waveOptions.pid, &request);
		if(ret < 0)
		{
			printf("WaveRadio: Could not transmit packet\n");
		}
		else
		{
			printf("WaveRadio: Transmitted packet\n");
		}
	}
	else
	{
		printf("WaveRadio: Could not encode structure\n");
	}
}

void waveRadio_txRawPacket(uint8_t *data, int dataLength, int psid, int chan)
{
	WSMRequest request;
	// Fill request header information
	request.chaninfo.channel    = chan;
	request.chaninfo.rate       = waveOptions.txRate;
	request.chaninfo.txpower    = waveOptions.txPower;
	request.version             = 2;
	request.psid                = psid;
	request.wsmps               = waveOptions.wsmps;
	request.txpriority          = waveOptions.txPriority;
	request.security            = waveOptions.security;
	request.macaddr[0] = 255;
	request.macaddr[1] = 255;
	request.macaddr[2] = 255;
	request.macaddr[3] = 255;
	request.macaddr[4] = 255;
	request.macaddr[5] = 255;
	//request.expirytime = ? ;
	getMACAddr(request.srcmacaddr, chan);
	
	memset(&request.data, 0, sizeof(WSMData));
	memcpy(&request.data.contents, data, dataLength);
	request.data.length = dataLength;

	int ret = txWSMPacket(waveOptions.pid, &request);
	if(ret < 0)
	{
		printf("WaveRadio: Could not transmit packet\n");
	}
	else
	{
		printf("WaveRadio: Transmitted packet\n");
	}
}


static unsigned int waveRadioNumberOfActivePackets = 0;
WaveRxPacket *waveRadio_createRxPacket(asn_TYPE_descriptor_t *type, void *structure)
{
	WaveRxPacket *results = (WaveRxPacket *)malloc(sizeof(WaveRxPacket));
	if (results)
	{
		results->structure = structure;
		results->type = type;
		clock_gettime(CLOCK_MONOTONIC, &results->rxTime);
		waveRadioNumberOfActivePackets++;
	}
	if (waveRadioNumberOfActivePackets > 15)
		printf("WaveRadio: Current number of active packets: %u\n", waveRadioNumberOfActivePackets);
	
	return results;
}

void waveRadio_destroyRxPacket(WaveRxPacket *packet)
{
	if (packet->structure)
	{	
		waveRadioNumberOfActivePackets--;
		ASN_STRUCT_FREE(*packet->type, packet->structure);
	}

	free(packet);
}
