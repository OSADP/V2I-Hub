/*
 ============================================================================
 Name        : SpatGeneratorPlugin.c
 Version     :
 Copyright   : Battelle
 Description : Create the Battelle SPaT Messages and send them to the DSRC
 ============================================================================
 */
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


#include "tmx/tmx.h"
#include "tmx/IvpPlugin.h"
#include <tmx/messages/IvpBattelleDsrc.h>
#include <tmx/messages/IvpSignalControllerStatus.h>
#include <tmx/messages/IvpJ2735.h>

#include "signalController.h"
#include "utils/common.h"
#include "utils/spat.h"
#include "utils/PerformanceTimer.h"
#include "utils/logger.h"
#include "XmlPtlmParser.h"
#include "SpatMessage_r41.h"

using namespace std;

pthread_mutex_t gSettingsMutex = PTHREAD_MUTEX_INITIALIZER;
IvpPlugin*	gPlugin = NULL;
char*		gLocalIp;
char*		gLocalUdpPort;
char*		gTscIp;
char*		gTscRemoteSnmpPort;

bool		gConfigurationLoaded = false;
bool		gConfigured = false;

SignalController sc;
SpatMessage_r41 smr41;

bool gPtlmFilesLoaded = false;
std::map<int, std::string> _ptlmFiles;
int _actionNumber = -1;

template<class Type>
void SetStatus(const char *key, Type value)
{
	std::ostringstream ss;
	ss << value;
	ivp_setStatusItem(gPlugin, key, ss.str().c_str());
}

bool ParsePtlmFilesJson(cJSON *root)
{
	if (root == NULL)
		return false;

	cJSON *actionNumber;
	cJSON *filePath;

	cJSON *item = cJSON_GetObjectItem(root, "PtlmFiles");

	if (item == NULL || item->type != cJSON_Array)
		return false;

	_ptlmFiles.clear();

	for (int i = 0 ; i < cJSON_GetArraySize(item) ; i++)
	{
		cJSON *subitem = cJSON_GetArrayItem(item, i);

		actionNumber = cJSON_GetObjectItem(subitem, "Action");
		filePath = cJSON_GetObjectItem(subitem, "FilePath");

		if (actionNumber == NULL || actionNumber->type != cJSON_Number || filePath == NULL || filePath->type != cJSON_String)
			return false;

		_ptlmFiles.insert(std::pair<int, std::string>(actionNumber->valueint, filePath->valuestring));
	}

	return true;
}

void DebugPrintPtlmFiles()
{
	std::map<int, std::string>::iterator iter;

	std::cout << _ptlmFiles.size() <<  " PTLM files specified by configuration settings:" << std::endl;

	for (iter = _ptlmFiles.begin(); iter != _ptlmFiles.end(); iter++)
	{
		 int key = iter->first;
		 std::string value = iter->second;
		 std::cout << "-- Action " << key << " file is " << value << std::endl;
	}
}

std::string GetPtlmFile(int actionNumber)
{
	std::cout << "Getting PTML file for Action " << actionNumber << std::endl;

	if (_ptlmFiles.empty())
		return "";

	if (actionNumber < 0)
		actionNumber = 1;

	// Try to find a PTLM file for the current action number.
	std::map<int, std::string>::iterator pair = _ptlmFiles.find(actionNumber);
	if (pair == _ptlmFiles.end())
	{
		std::cout << "PTML file not found for Action " << actionNumber << std::endl;

		// Could not find a PTLM file for the current action number.
		if (_ptlmFiles.size() > 1)
			std::cout << "Multiple PTML files available, using the first one." << std::endl;

		// Default to any in the std::map.
		pair = _ptlmFiles.begin();
		actionNumber = pair->first;
	}

	std::cout << "Using PTML file for Action " << actionNumber << std::endl;

	SetStatus<int>("PTLM Action Number", actionNumber);
	ivp_setStatusItem(gPlugin, "PTLM File", pair->second.c_str());

	return pair->second;
}

void GetConfigSettings()
{
	char *signalControllerIP = ivp_getCopyOfConfigurationValue(gPlugin, "Local_IP");
	if (signalControllerIP != NULL)
	{
		pthread_mutex_lock(&gSettingsMutex);
		gLocalIp = strdup(signalControllerIP);
		pthread_mutex_unlock(&gSettingsMutex);
		free(signalControllerIP);
	}
	char *SignalControllerUDPPort = ivp_getCopyOfConfigurationValue(gPlugin, "Local_UDP_Port");
	if (SignalControllerUDPPort != NULL)
	{
		pthread_mutex_lock(&gSettingsMutex);
		gLocalUdpPort = strdup(SignalControllerUDPPort);
		pthread_mutex_unlock(&gSettingsMutex);
		free(SignalControllerUDPPort);
	}
	char *SignalControllerSNMPIp = ivp_getCopyOfConfigurationValue(gPlugin, "TSC_IP");
	if (SignalControllerSNMPIp != NULL)
	{
		pthread_mutex_lock(&gSettingsMutex);
		gTscIp = strdup(SignalControllerSNMPIp);
		pthread_mutex_unlock(&gSettingsMutex);
		free(SignalControllerSNMPIp);
	}
	char *SignalControllerSNMPPort = ivp_getCopyOfConfigurationValue(gPlugin, "TSC_Remote_SNMP_Port");
	if (SignalControllerSNMPPort != NULL)
	{
		pthread_mutex_lock(&gSettingsMutex);
		gTscRemoteSnmpPort = strdup(SignalControllerSNMPPort);
		pthread_mutex_unlock(&gSettingsMutex);
		free(SignalControllerSNMPPort);
	}

	char *rawPtlmFiles = ivp_getCopyOfConfigurationValue(gPlugin, "PTLM_Files");
	if (rawPtlmFiles != NULL)
	{
		cJSON *root = cJSON_Parse(rawPtlmFiles);

		pthread_mutex_lock(&gSettingsMutex);

		if (ParsePtlmFilesJson(root))
		{
			gPtlmFilesLoaded = true;
			DebugPrintPtlmFiles();
		}
		else
		{
			std::cout << "Error parsing PtlmFiles config setting." << std::endl;
		}

		pthread_mutex_unlock(&gSettingsMutex);

		free(rawPtlmFiles);
	}

	gConfigurationLoaded = true;
	printf("GetConfigSettings Complete %s, %s, %s, %s\n", gLocalIp , gLocalUdpPort, gTscIp, gTscRemoteSnmpPort);
}


void onStateChange(IvpPlugin *plugin, IvpPluginState state)
{
	printf("State Change: %d\n", state);

	if (state == IvpPluginState_registered)
	{
		printf("Reading Configuration Files\n");
		GetConfigSettings();
	}
}

void onError(IvpPlugin *plugin, IvpError err)
{
	fprintf(stderr, "%d - %d, %d\n", err.level, err.error, err.sysErrNo);
//	fprintf(stderr, "%s - %s, %d\n", ivpError_getLogLevelText(err.level), ivpError_getIvpErrorText(err.error), err.sysErrNo);
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

int main(void)
{
	int iCounter=0;
	spat sm;

	IvpPluginInformation info = IVP_PLUGIN_INFORMATION_INITIALIZER;
	info.onMsgReceived = onMessageReceived;
	info.onStateChange = onStateChange;
	info.onError = onError;
	info.onConfigChanged = onConfigChanged;

	gPlugin = ivp_create(info);

	if (!gPlugin)
	{
		printf("Error creating plugin\n");
		return EXIT_FAILURE;
	}

	IvpMsgFilter *filter = NULL;
	ivp_subscribe(gPlugin, filter);
	ivpSubscribe_destroyFilter(filter);

	// How long it took to update and send the SPaT message the last time through the loop.
	__useconds_t sendElapsedMicroSec = 0;

	try
	{
	while(gPlugin->state != IvpPluginState_error)
	{
		// wait to send next message
		if (gConfigurationLoaded && gPtlmFilesLoaded)
		{
			if (!gConfigured)
			{
				int action = sc.getActionNumber();

				pthread_mutex_lock(&gSettingsMutex);
				std::cout <<  "Get PTLM file specified by configuration settings" << std::endl;
				std::string ptlmFile = GetPtlmFile(action);
				pthread_mutex_unlock(&gSettingsMutex);

				if (!ptlmFile.empty())
				{
					_actionNumber = action;

					// sc.spat_message_mutex does not need locked because the thread is not running yet.

					pthread_mutex_lock(&gSettingsMutex);
					sc.setConfigs(gLocalIp , gLocalUdpPort, gTscIp, gTscRemoteSnmpPort, ptlmFile.c_str());
					pthread_mutex_unlock(&gSettingsMutex);

					// Start the signal controller thread.
					sc.Start(&sm);
					// Give the spatdata pointer to the message class
					smr41.setSpatData(sc.getSpatData());

					gConfigured = true;
				}
			}

			// SPaT must be sent exactly every 100 ms.  So adjust for how long it took to do the last send.
			if (sendElapsedMicroSec < 100000)
				usleep(100000 - sendElapsedMicroSec);
			iCounter++;

			bool messageSent = false;
			PerformanceTimer timer;

			// Update PTLM file if the action number has changed.
			int actionNumber = sc.getActionNumber();
			if (_actionNumber != actionNumber)
			{
				_actionNumber = actionNumber;

				pthread_mutex_lock(&gSettingsMutex);
				std::string ptlmFile = GetPtlmFile(_actionNumber);
				pthread_mutex_unlock(&gSettingsMutex);

				if (!ptlmFile.empty())
				{
					pthread_mutex_lock(&sc.spat_message_mutex);
					sc.updatePtlmFile(ptlmFile.c_str());
					pthread_mutex_unlock(&sc.spat_message_mutex);
				}
			}
			if(sc.getIsConnected())
			{
				ivp_setStatusItem(gPlugin, "TSC Connection", "Connected");

				smr41.createSpat();
	            IvpMessage *msg = ivpJ2735_createMsgFromEncodedwType(smr41.derEncoded , smr41.derEncodedBytes, IvpMsgFlags_RouteDSRC, "SPAT-P");

	            if (msg != NULL)
				{
					ivpMsg_addDsrcMetadata(msg, 172, 0x8002);
					ivp_broadcastMessage(gPlugin, msg);
					ivpMsg_destroy(msg);
					messageSent = true;
				}

				if(iCounter%20==0)
				{
					iCounter = 0;
					// Action Number
					IvpMessage *actionMsg = ivpSigCont_createMsg(sc.getActionNumber());
					if(actionMsg!=NULL)
					{
						ivp_broadcastMessage(gPlugin, actionMsg);
						ivpMsg_destroy(actionMsg);
					}
				}
//				*/
//	            ivpMsg_destroy(msg);
			}
			else
			{
				ivp_setStatusItem(gPlugin, "TSC Connection", "Disconnected");
			}

			sendElapsedMicroSec = timer.Elapsed().total_microseconds();
//			LOG_DEBUGGING("SpatGen Main Loop Time: " << sendElapsedMicroSec / 1000.0 << " ms, Data sent? " << (messageSent ? "yes" : "no"));
		}
	}
	}
	catch (exception &ex)
	{
		stringstream ss;
		ss << "SpatPlugin terminating from unhandled exception: " << ex.what();

		ivp_addEventLog(gPlugin, IvpLogLevel_error, ss.str().c_str());
		std::terminate();
	}

	return EXIT_SUCCESS;
}
