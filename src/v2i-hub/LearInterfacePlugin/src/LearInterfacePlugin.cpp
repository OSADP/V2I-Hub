//============================================================================
// Name        : EpcwPlugin.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include <time.h>
#include <sys/time.h>
#include <atomic>

#include "TmxMessageManager.h"

//#ifdef __cplusplus

#include <tmx/j2735_messages/MessageFrame.hpp>

void sendWsm(uint8_t *data, uint16_t dataLen, uint8_t *addr, uint16_t addrLen);

extern "C"{
//#endif
//Needed for Lear SDK
#include <math.h>
#include "dot3/p16093WMEChannelService.h"
#include "dot3/p16093WMEWsmService.h"
#include "dot3/p16093WMENotification.h"
#include "dot3/p16093WMEAvailableService.h"
#include "dot3/p16093WMEChanInfo.h"
#include "dot3/dot3WSMCmd.h"
#include "dot3/p16093WSMRx.h"
#include "sdee_api.h"
#include "learRoadstar.h"

p16093WMEChannelServiceRequest csrReq;
p16093WMEWsmServiceRequest wsmRequest;
appData *samplePriv;
int psid = 130;
int channel = 172;

/*
 * Below function is the callback function to libdot3
 * This is called when there is a WME notification from
 * dot3 layer
 */
void rxWmeCallBack(p16093WMEData *ind)
{
    p16093WMENotificationIndication *notif;

    notif = (p16093WMENotificationIndication *)ind->data;
    /* handle wme notification appropriately */
    //printf("WME Notification\n");
    //printf("Event %d\n", notif->event);
    //printf("Reason %d\n", notif->reason);
    //printf("Data length %d\n", notif->length);
    if (notif->length > 0)
    {
        if (serviceJoined == notif->event)
        {
            p16093WMEAvailableService *service;
            service = (p16093WMEAvailableService *)notif->data;
            //printf("PSID %d\n", service->psid);
            //printf("Channel %d\n", service->chInfo.channelParams.channelNumber);
        }
        else if (channelAssigned == notif->event)
        {
            p16093WMEChannelInfo *ch;
            ch = (p16093WMEChannelInfo *)notif->data;
            //printf("Channel %d\n", ch->channelParams.channelNumber);
        }
    }

    return;
}

/*
 * Below function is the callback function to libdot3
 * This is called when there is a WSM notification from
 * dot3 layer.
 *
 * WSM notifications are given when there is
 * a WSMP packet received for the PSID whic is matching
 * with the application.
 *
 * Please note that there should be a minimal processing in
 * this function to avoid the RX delay.
 */
void rxWsmCallBack(p16093WSMData *wsmReqData)
{
    int result = 0;
    uint8_t tempBuf[WSM_MAX_LENGTH] = {0};
    uint32_t tmpLen = 0;

    memset(tempBuf, 0, sizeof(tempBuf));
    p16093WSMRxIndication *rxData = (p16093WSMRxIndication *)wsmReqData->data;


    /* DOT2 Verification API */
    result = sdeeVerifyDataReq(samplePriv->asmContext, rxData->data,
            rxData->length, samplePriv->psid, NULL, 0,
            0, 0, 0, 0, NULL, 0, tempBuf, &tmpLen);
    if (result != RESULT_SUCCESS)
    {
        //printf("sdeeVerifyData Request failed\n");
        //printf("%s:%d  sdeeVerifyDataReq fail: (%s)\n", __func__, __LINE__, sdeeGetErrorString(result));
        return;
    }
   // else if(result == RESULT_SUCCESS)
   // {
   //     printf("Verifydata request is success\n");
   // }
    

    samplePriv->rxCount++;
    //printf("Sample Rx :%d Packets\n************* contents *************\n", samplePriv->rxCount);
    //printf("wsmpVersion %u\n", rxData->wsmpVersion);
    //printf("Transmission power %u\n", rxData->txPwrUsed);
    //printf("PSID %u\n", rxData->psid);
    //printf("MAC [%2x]:[%2x]:[%2x]:[%2x]:[%2x]:[%2x]\n", rxData->peerMac[0], rxData->peerMac[1], rxData->peerMac[2], rxData->peerMac[3], rxData->peerMac[4], rxData->peerMac[5]);
    //printf("length %u\n", rxData->length);

    
    /*printf("tempBuf as hex\n");
    for (int i=0 ; i < tmpLen; i++)
    {
        printf("0x%02x ", tempBuf[i]);
    }
    printf("\n");
    */
    sendWsm(tempBuf, tmpLen, rxData->peerMac, IEEE80211_ADDR_LEN);

    
}

//#ifdef __cplusplus
}
//#endif

using namespace std;

using namespace tmx;
using namespace tmx::utils;
using namespace tmx::messages;


namespace LearInterfacePlugin {

class LearInterfacePlugin: public TmxMessageManager {
public:
	LearInterfacePlugin(std::string);
	virtual ~LearInterfacePlugin();
	int Main();

	// Override handler to send newly created messages
	void OnMessageReceived(tmx::routeable_message &msg);

	static LearInterfacePlugin *get_instance();
protected:

	void UpdateConfigSettings();

	// Virtual method overrides.
	void OnConfigChanged(const char *key, const char *value);
	//void OnMessageReceived(IvpMessage *msg);
	void OnStateChange(IvpPluginState state);
private:
	void buildCsrReqPacket(p16093WMEChannelServiceRequest *req);
	static LearInterfacePlugin *_self;
};

LearInterfacePlugin *LearInterfacePlugin::_self = NULL;

/**
 * Construct a new EpcwPlugin with the given name.
 *
 * @param name The name to give the plugin for identification purposes
 */
LearInterfacePlugin::LearInterfacePlugin(string name) :
		TmxMessageManager(name) {
	_self = this;
}

LearInterfacePlugin::~LearInterfacePlugin() {
}

void LearInterfacePlugin::UpdateConfigSettings() {
 	GetConfigValue("PSID", psid);
    GetConfigValue("Channel", channel);
    PLOG(logDEBUG) << "Config Values Updated PSID: " << psid << "Channel: " << channel;
}

void LearInterfacePlugin::OnConfigChanged(const char *key, const char *value) {
	TmxMessageManager::OnConfigChanged(key, value);
	UpdateConfigSettings();
}

void LearInterfacePlugin::OnStateChange(IvpPluginState state) {
	TmxMessageManager::OnStateChange(state);

	if (state == IvpPluginState_registered) {
		UpdateConfigSettings();
	}
}

void LearInterfacePlugin::OnMessageReceived(routeable_message &msg) {
	this->OutgoingMessage(msg);
}
/*
 * Channel service request API structure is filled as below
 * This structure maps to the API definition in IEEE1609.3
 * specification
 */
void LearInterfacePlugin::buildCsrReqPacket(p16093WMEChannelServiceRequest *req)
{
    /* Local index is generated randomly by libdot3, applications
     * can use that by accessing as below
     */ 
    req->localIndex = samplePriv->wmeCtx.localIndex;
    req->action = ACTION_ADD;
    strcpy((char *)req->channelIdentifier.countryString, "US");
    req->channelIdentifier.operatingClass = 17;
    req->channelIdentifier.channelNumber = samplePriv->cData.channelNo;
    req->timeSlot = BOTH;
}

LearInterfacePlugin *LearInterfacePlugin::get_instance() {
	return _self;
}


int LearInterfacePlugin::Main() {
	FILE_LOG(logINFO) << "Starting plugin.";

	int  ret;
    unsigned int result;
	uint64_t lastSendTime = 0;

    while (_plugin->state != IvpPluginState_registered) {
        usleep(1000);
    }

    samplePriv = (appData *)calloc(1, sizeof(appData));
    memset(samplePriv, 0, sizeof(appData));

    PLOG(logDEBUG) << "PSID: " << psid << " Channel: " << channel;

    samplePriv->psid = psid;
    samplePriv->cData.channelNo = channel;
    csrReq.timeSlot = BOTH;

    /*
     * Initialize the DOT2 layer interface, this will be required for 
     * DOT2 sign and verification
     */
    
    /* Initialize the security Library */
    sdeeInitLibrary();

    //if(samplePriv->psid == 0x20) {
        chdir("/etc");
        ret = sdeeRegisterSecurityServices(samplePriv->psid, NULL, NULL,
                NULL, NULL, &result);
        if (ret)
        {
            PLOG(logERROR) << "Failed to initialize security services: " << (const char *)sdeeGetErrorString(ret);
            return -1;
        }
        samplePriv->asmContext = result;
    //}

    /* Below call will establish the connection with DOT3 layer.
     * Configure the IP address and port number if the dot3
     * is running remotely.
     * By default the IP address is configure to 127.0.0.1
     */
    strcpy(samplePriv->wmeCtx.interface, "127.0.0.1");
    samplePriv->wmeCtx.interfacePort = WME_INTERFACE_PORT;
    samplePriv->wmeCtx.fn = rxWmeCallBack;

    strcpy(samplePriv->wsmCtx.interface, "127.0.0.1");
    samplePriv->wsmCtx.interfacePort = WSM_INTERFACE_PORT;
    samplePriv->wsmCtx.fn = rxWsmCallBack;
    result = lib16093Init(&samplePriv->wmeCtx, &samplePriv->wsmCtx);
    if (result == LIB_SUCCESS)
    {
        PLOG(logDEBUG) << "Init success";
    }
    else
    {
    	PLOG(logERROR) << "Init failed [error " << result << "]";
        exit(-1);
    }

    buildCsrReqPacket(&csrReq);
    result = wmeChannelServiceRequest(&samplePriv->wmeCtx, &csrReq);
    if(result == WME_ACCEPTED)
        PLOG(logDEBUG) << "channel Service request success";
    else {
        PLOG(logERROR) << "channel Service request failed[error " << result << "]";
        return -1;
    }

    /* Send WSM service request to receive the WSMP Indications
     * (WSMP RX) with this PSID. Without this application will not
     * receive the RX Packets.
     */
    wsmRequest.action = ACTION_ADD;
    /* Local index is generated randomly by libdot3, applications
     * can use that by accessing as below
     */ 
    wsmRequest.localIndex = samplePriv->wmeCtx.localIndex;
    wsmRequest.psid = samplePriv->psid;
    result = wmeWsmServiceRequest(&samplePriv->wmeCtx, &wsmRequest);
    if(result != WME_ACCEPTED)
    {
        PLOG(logERROR) << "Service request failed [error " << result << "]";
        return -1;
    }

	
	while (_plugin->state != IvpPluginState_error) {
		usleep(100000); //sleep for microseconds set from config.
	}

	return (EXIT_SUCCESS);
}
} /* namespace LearInterfacePlugin */

void sendWsm(uint8_t *data, uint16_t dataLen, uint8_t *addr, uint16_t addrLen) {

	if (LearInterfacePlugin::LearInterfacePlugin::get_instance() == NULL)
		return;

	uint8_t group = 0;
	uint8_t id = 0;
	if (addr && addrLen > 0) {
		id = addr[addrLen - 1];
	}
	if (addr && addrLen > 1) {
		group = addr[addrLen - 2];
	}

	LearInterfacePlugin::LearInterfacePlugin::get_instance()->IncomingMessage(
			data, dataLen, ASN1_CODEC<tmx::messages::MessageFrameMessage>::Encoding, group, id, Clock::GetMillisecondsSinceEpoch());
}

int main(int argc, char *argv[]) {
	return run_plugin < LearInterfacePlugin::LearInterfacePlugin > ("LearInterface", argc, argv);
}
