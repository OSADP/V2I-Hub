/*
 ============================================================================
 Name        : main.cpp
 Description : DSRC Receiver Plugin

 This application only receives DSRC messages over the radio and forwards
 them to the ivpcore application.  It does not transmit anything over the radio.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <tmx/tmx.h>
#include <tmx/IvpPlugin.h>
#include <tmx/messages/IvpJ2735.h>
#include <tmx/messages/IvpBattelleDsrc.h>
#include <signal.h>
#include <vector>
#include <map>
#include <iostream>
#include <getopt.h>

#ifdef __cplusplus
extern "C"
{
#endif
	#include <wave.h>
	#include "WaveRadio.h"
#ifdef __cplusplus
}
#endif

#include <sstream>

#define CHANNEL_ACCESS_MODE CHACCESS_CONTINUOUS
//#define CHANNEL_ACCESS_MODE CHACCESS_ALTERNATIVE

#define CHAN_BSM 172
#define PSID_BSM 0x20
#define CHAN_MAPSPAT 172
#define PSID_MAPSPAT 0x8002
#define CHAN_SPAT 172
#define PSID_SPAT 0x8002
#define CHAN_MAP 172
#define PSID_MAP 0x8002

using namespace std;

struct ChanPsidIndex {
	uint32_t psid;
	uint16_t chan;

	bool operator== (const ChanPsidIndex &cmp) const
	{
		return this->psid == cmp.psid && this->chan == cmp.chan;
	}

	bool operator< (const ChanPsidIndex &cmp) const
	{
		std::stringstream ss1, ss2;
		ss1 << this->psid << "||" << this->chan;
		ss2 << cmp.psid << "||" << cmp.chan;
		return ss1.str() < ss2.str();
	}
};

pid_t gPid;
map<ChanPsidIndex, WMEApplicationRequest> gProviders;
IvpPlugin *gPlugin;
int gIsRegistered;
map<ChanPsidIndex, uint64_t> gMessageCounts;

void (*oldsig_int)(int);
void (*oldsig_kill)(int);
void (*oldsig_quit)(int);
void (*oldsig_term)(int);
void (*oldsig_segv)(int);

void sig(int sig)
{
	signal(SIGINT, oldsig_int);
	signal(SIGKILL, oldsig_kill);
	signal(SIGQUIT, oldsig_quit);
	signal(SIGTERM, oldsig_term);
	signal(SIGSEGV, oldsig_segv);

	for(map<ChanPsidIndex, WMEApplicationRequest>::iterator itr = gProviders.begin(); itr != gProviders.end(); itr++)
	{
		removeProvider(gPid, &(itr->second));
	}

	raise(sig);
}

uint64_t GetMsTimeSinceEpoch()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t)((double)(tv.tv_sec) * 1000 + (double)(tv.tv_usec) / 1000);
}

// Helper method to convert an IvpPluginState enumeration to a string.
std::string IvpPluginStateToString(IvpPluginState state)
{
	switch (state)
	{
	case IvpPluginState_disconnected:
		return "Disconnected";
	case IvpPluginState_connected:
		return "Connected";
	case IvpPluginState_registered:
		return "Registered";
	case IvpPluginState_error:
		return "Error";
	default:
		return "Unknown";
	}
}

// Helper method to convert a channel/psid to a string.
std::string ChanPsidToString(uint16_t channel, uint32_t psid, bool includePsidAndChan)
{
	stringstream ss;

	if (channel == CHAN_BSM && psid == PSID_BSM)
		ss << "BSM";
	else if (channel == CHAN_MAPSPAT && psid == PSID_MAPSPAT)
		ss << "MAPs & SPATS";
	else if (channel == CHAN_SPAT && psid == PSID_SPAT)
		ss << "SPAT";
	else if (channel == CHAN_MAP && psid == PSID_MAP)
		ss << "MAP";
	else
	{
		// If there is not a friendly name, always return channel and psid.
		ss << "Channel " << channel << ", PSID 0x" << hex << psid;
		return ss.str();
	}

	if (includePsidAndChan)
		ss << ", Channel " << channel << ", PSID 0x" << hex << psid;

	return ss.str();
}

void onStateChange(IvpPlugin *plugin, IvpPluginState state)
{
	// Print the current state of this plugin.
	cout << "Plugin State Change: " << IvpPluginStateToString(state) << endl;

	if (state == IvpPluginState_registered)
	{
		//This is a good place to "START" the plugin logic.
		gIsRegistered = 1;
	}
	else
	{
		//This is a good place to "STOP" the plugin logic.
		gIsRegistered =0;
	}
}

void onError(IvpPlugin *plugin, IvpError err)
{
	// Print the error that occurred.
	cerr << "Level: " << err.level << ", Error: " << err.error << ", Sys Error: " << err.sysErrNo << endl;
}

void addProviderEntry(int channel, int psid)
{
	ChanPsidIndex index;
	index.chan = channel;
	index.psid = psid;
	map<ChanPsidIndex, WMEApplicationRequest>::iterator providerEntry = gProviders.find(index);
	if (providerEntry == gProviders.end())
	{
		{
			WMEApplicationRequest entry;
			entry.psid = index.psid;
			entry.priority = 31;
			entry.channel = index.chan;
			entry.serviceport = 0;
			entry.ipservice = 0;
			entry.repeatrate = 50;
			entry.linkquality = 1;
			entry.channelaccess = CHANNEL_ACCESS_MODE;

			gProviders.insert(pair<ChanPsidIndex, WMEApplicationRequest>(index, entry));

			// Register the provider.
			// The sample code from Arada tries again if it fails the first time, so that logic is employed here.
			int registerStatus = registerProvider(gPid, &entry);
			if (registerStatus < 0)
			{
				removeProvider(gPid, &entry);
				registerStatus = registerProvider(gPid, &entry);
			}

			cout << "Register Provider (" << ChanPsidToString(entry.channel, entry.psid, true) << "): ";
			cout << (registerStatus < 0 ? "Failed" : "Success") << endl;
		}
	}
}

void onMessageReceived(IvpPlugin *plugin, IvpMessage *msg)
{
	assert(msg != NULL);

	printf("Received Message - Type: %s, Subtype: %s, Source: %s\n", msg->type, msg->subtype, msg->source);
}

void onConfigChanged(IvpPlugin *plugin, const char *key, const char *value)
{
	printf("Config Changed | %s: %s\n", key, value);
}

void GetUInt32(unsigned char *buf, uint32_t *value)
{
	*value = (uint32_t)((buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3]);
}

void receiveWSMIndication(WSMIndication *rxpkt) 
{
	uint64_t msTimeSinceEpochStart = GetMsTimeSinceEpoch();
	//printf("Received WSM Packet: Channel = %d, PSID = %d\n", rxpkt->chaninfo.channel, rxpkt->psid);

	// Track how many times a message has been received for this channel/psid combination.
	ChanPsidIndex index;
	index.chan = rxpkt->chaninfo.channel;
	index.psid = rxpkt->psid;
	map<ChanPsidIndex, uint64_t>::iterator entry = gMessageCounts.find(index);
	if (entry == gMessageCounts.end())
		gMessageCounts.insert(pair<ChanPsidIndex, uint64_t>(index, 1));
	else
		entry->second = entry->second + 1;

	//cout << "Channel: " << entry->first.chan << ", PSID: " << entry->first.psid << ", Count: " << entry->second << endl;

	// This will receive any J2735 message.  However, addProviderEntry() must be called in main()
	// for each PSID to be received.
	IvpJ2735Msg *decodedMsg = ivpJ2735_decode((uint8_t *)rxpkt->data.contents, rxpkt->data.length);
	if (decodedMsg)
	{
		IvpMessage *msg = ivpJ2735_createMsgFromEncoded((uint8_t *)rxpkt->data.contents, rxpkt->data.length, IvpMsgFlags_None);
		if (msg != NULL && gIsRegistered==1)
		{
			//ivpMsg_addDsrcMetadata(msg, rxpkt->chaninfo.channel, rxpkt->psid);
			ivp_broadcastMessage(gPlugin, msg);
			ivpMsg_destroy(msg);
		}

		ivpJ2735_destroyJ2735Msg(decodedMsg);
	}
	// This is not a J2735 message, check for Map or Spat.
	else
	{
		uint8_t *data = (uint8_t *)rxpkt->data.contents;
		int dataLength = rxpkt->data.length;

		if (dataLength > 6)
		{
			uint8_t messageId = data[0];

			IvpMessage *msg = NULL;
			// Receive Map.
			if (messageId == 0x87)
				msg = ivpBattelleDsrc_createMsg(data, dataLength, IvpBattelleDsrcMsgType_GID, IvpMsgFlags_None);
			// Receive Spat.
			if (messageId == 0x8d)
			{
				uint64_t msTimeSinceEpoch = GetMsTimeSinceEpoch();
				uint32_t timestampSeconds = 0;
				uint8_t timestampTenths = 0;


				GetUInt32(data+15, &timestampSeconds);
				timestampTenths = data[19];


				//cout<<"DSRC Receiver, "<< timestampSeconds <<"."<< timestampTenths << ", SPAT TIME, "<<msTimeSinceEpoch<<endl;
				msg = ivpBattelleDsrc_createMsg(data, dataLength, IvpBattelleDsrcMsgType_SPAT, IvpMsgFlags_None);





			}

			if (msg != NULL && gIsRegistered==1)
			{
				//ivpMsg_addDsrcMetadata(msg, rxpkt->chaninfo.channel, rxpkt->psid);
				ivp_broadcastMessage(gPlugin, msg);
				ivpMsg_destroy(msg);
			}
		}
	}

	uint64_t msTimeSinceEpochEnd = GetMsTimeSinceEpoch();
	//cout<<"DSRC Receiver Process time (ms): "<<(msTimeSinceEpochEnd - msTimeSinceEpochStart)<<endl;
}

void receiveWME_NotifIndication(WMENotificationIndication *wmeindication)
{

}

void receiveWRSS_Indication(WMEWRSSRequestIndication *wrssindication)
{
	printf("WRSS receive Channel = %d   Report = %d\n",
			(u_int8_t) wrssindication->wrssreport.channel,
			(u_int8_t) wrssindication->wrssreport.wrss);
}

void receiveTsfTimerIndication(TSFTimer *timer)
{
	printf("TSF Timer: Result=%d, Timer=%llu",
			(u_int8_t) timer->result,
			(u_int64_t) timer->timer);
}

void PrintMessageCounts()
{
	map<ChanPsidIndex, uint64_t>::iterator iter;

	if (gMessageCounts.size() == 0) return;

	cout << "Total Messages: ";
	bool isFirst = true;

	for (iter = gMessageCounts.begin(); iter != gMessageCounts.end(); iter++)
	{
		if (!isFirst)
			cout << ", ";

		cout << ChanPsidToString(iter->first.chan, iter->first.psid, false) << ": ";
		cout << iter->second;

		isFirst = false;
	}

	cout << endl;
}

int main(int argc, char *argv[])
{
	int flagHelp = 0;
	int flagBsm = 0;
	int flagMap = 0;
	int flagSpat = 0;

	gIsRegistered = 0;
	while (1)
	{
		static struct option long_options[] =
		{
			{"help", no_argument, &flagHelp, 1},
			{"bsm", no_argument, &flagBsm, 1},
			{"map", no_argument, &flagMap, 1},
			{"spat", no_argument, &flagSpat, 1},
			{0, 0, 0, 0}
		};

		int option_index = 0;
		int c = getopt_long (argc, argv, "", long_options, &option_index);

		// Detect the end of the options.
		if (c == -1)
			break;
	}

	if (flagHelp)
	{
		printf("Usage: %s [--bsm] [--map] [--spat]\n", argv[0]);
		printf("\tIf no parameters are specified, all messages are received.\n");
		printf("\tOtherwise only the specified messages are received.\n");
		exit(0);
	}

	// If none were specified, receive all of them.
	if (!(flagBsm || flagMap || flagSpat))
	{
		flagBsm = 1;
		flagMap = 1;
		flagSpat = 1;

		// For some reason, this printf will cause the app to crash!
		//printf("No command line options specified.  Receiving all messages.\n");
		cout << "No command line options specified.  Receiving all messages." << endl;
	}

	printf("Receive BSM: %s\n", flagBsm ? "yes" : "no");
	printf("Receive Map: %s\n", flagMap ? "yes" : "no");
	printf("Receive Spat: %s\n", flagSpat ? "yes" : "no");

	gPid = getpid();

	oldsig_int = signal(SIGINT, sig);
	oldsig_kill = signal(SIGKILL, sig);
	oldsig_quit = signal(SIGQUIT, sig);
	oldsig_term = signal(SIGTERM, sig);
	oldsig_segv = signal(SIGSEGV, sig);

	IvpPluginInformation info = IVP_PLUGIN_INFORMATION_INITIALIZER;
	info.onMsgReceived = onMessageReceived;
	info.onStateChange = onStateChange;

	info.onError = onError;
	info.onConfigChanged = onConfigChanged;
	info.manifestLocation = "/var/bin/manifest.json";

	gPlugin = ivp_create(info);

	if (!gPlugin)
	{
		printf("Error creating plugin\n");
		return EXIT_FAILURE;
	}

	WaveRadioOptions wrOptions;
	wrOptions.security = 0;
	wrOptions.txPower = 14;
	wrOptions.txPriority = 2;
	wrOptions.txRate = 6;
	wrOptions.wsmps = 0;
	wrOptions.pid = gPid;
	waveRadio_setOptions(&wrOptions);

	registerWSMIndication(receiveWSMIndication);

	if (invokeWAVEDevice(WAVEDEVICE_LOCAL, 0) < 0)
	{
		printf("Open Failed. Quitting\n");
		exit(-1);
	}

	registerWMENotifIndication(receiveWME_NotifIndication);
	registerWRSSIndication(receiveWRSS_Indication);
	registertsfIndication(receiveTsfTimerIndication);
	
	// Request transmission of a Timing Advertisement frame.
	WMETARequest tareq;
	tareq.action = TA_ADD;
	tareq.repeatrate = 100;
	tareq.channel = 178;
	tareq.channelinterval = 1;
	tareq.servicepriority = 1;
	if (transmitTA(&tareq) < 0) 
	{
		printf("Send TA failed.\n");
	} 
	else 
	{
		printf("Send TA successful.\n") ;
	}

	if (flagBsm)
		addProviderEntry(CHAN_BSM, PSID_BSM);
	if (flagSpat || flagMap)
		addProviderEntry(CHAN_MAPSPAT, PSID_MAPSPAT);

	uint64_t lastPrintTime = 0;

	while(gPlugin->state != IvpPluginState_error)
	{
		WSMIndication rxpkt;
		int ret = rxWSMPacket(gPid, &rxpkt);

		if (ret > 0)
		{
			receiveWSMIndication(&rxpkt);
		}
		else
		{
			// Print message counts every 2 seconds.
			uint64_t now = GetMsTimeSinceEpoch();
			if (now - lastPrintTime > 2000)
			{
				PrintMessageCounts();
				lastPrintTime = now;
			}

			usleep(1000);
		}
	}

	return EXIT_SUCCESS;
}
