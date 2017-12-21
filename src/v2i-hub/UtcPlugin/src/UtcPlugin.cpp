//============================================================================
// Name        : UtcPlugin.cpp
// Author      : Battelle Memorial Institute
// Version     :
// Copyright   : Copyright (c) 2014 Battelle Memorial Institute. All rights reserved.
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <unistd.h>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#include "tmx/tmx.h"
#include "tmx/IvpPlugin.h"
#include <tmx/messages/IvpNmea.h>

using namespace std;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

IvpPlugin *gPlugin = NULL;
uint64_t gFrequency = 0;

int gTimeSyncd = 0;

char* intoToString(int number)
{
	std::ostringstream number_str;

	number_str << number;
	return (char*) number_str.str().c_str();
}

uint64_t GetMsTimeSinceEpoch()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t)((double)(tv.tv_sec) * 1000 + (double)(tv.tv_usec) / 1000);
}

void GetConfigSettings()
{
	char *rawFrequency = ivp_getCopyOfConfigurationValue(gPlugin, "Frequency");
	if (rawFrequency != NULL)
	{
		pthread_mutex_lock(&lock);
		gFrequency = strtoull(rawFrequency, NULL, 0);
		pthread_mutex_unlock(&lock);
		free(rawFrequency);
	}
}

void onStateChange(IvpPlugin *plugin, IvpPluginState state)
{
	cout << "State Change" << state << endl;

	if (state == IvpPluginState_registered)
	{
		GetConfigSettings();
	}
}

void onError(IvpPlugin *plugin, IvpError err)
{
	cerr << err.level << " - " << err.error << ", " << err.sysErrNo << endl;
}

void onMessageReceived(IvpPlugin *plugin, IvpMessage *msg)
{
	assert(msg != NULL);

	if(gTimeSyncd==0)
	{
		if((strcmp(msg->type, IVPMSG_TYPE_NMEA) ==0) && (strcmp(msg->subtype, "GGA")==0) &&(msg->payload->type == cJSON_String))
		{
			time_t rawtime;
			time(&rawtime);
			struct tm *tm;
			tm = gmtime(&rawtime);

			NmeaString* payloadvalue = ivpNmea_getNmeaString(msg);

			char *p = strtok(payloadvalue, ",");
			p = strtok(NULL, ",");

			std::string time_str(p);
			std::string hr_str = time_str.substr(0,2);
			std::string min_str = time_str.substr(2,2);
			std::string sec_str = time_str.substr(4,2);

			int hr_int = atoi(hr_str.c_str());
			int min_int = atoi(min_str.c_str());
			int sec_int = atoi(sec_str.c_str());

			tm->tm_hour = hr_int;
			tm->tm_min = min_int;
			tm->tm_sec = sec_int;

			time_t newtime = mktime(tm);

			timeval tv;
			tv.tv_sec = newtime;
			tv.tv_usec =0;

			cout<< "Setting Time To: "<<hr_str<<":"<<min_str<<":"<<sec_str<<endl;
			settimeofday(&tv, NULL);
			cout<<"Done Setting Time"<<endl;

			gTimeSyncd = 1;
		}
	}
}

void onConfigChanged(IvpPlugin *plugin, const char *key, const char *value)
{
	cout << "Config Changed | " << key << ": " << value << endl;

	GetConfigSettings();
}

void SendTime(IvpPlugin *plugin, uint64_t time)
{
	//stringstream ss;
	//ss << time;
	//const string utcTime(ss.str());
	//ivp_setStatusItem(gPlugin, "UTC MS", utcTime.c_str());

	// Send millisecond time.

	cJSON *payload = cJSON_CreateObject();
	cJSON_AddNumberToObject(payload, "utc_ms", time);

	IvpMessage *msg = ivpMsg_create("Time", "MS_UTC", IVP_ENCODING_JSON, IvpMsgFlags_None, payload);
	ivp_broadcastMessage(plugin, msg);
	ivpMsg_destroy(msg);
	cJSON_Delete(payload);

	// Send formatted time.

	time_t time_sec = time / 1000;
	struct tm *tm;
	tm = gmtime(&time_sec);

	payload = cJSON_CreateObject();
	cJSON_AddNumberToObject(payload, "Month", tm->tm_mon + 1);
	cJSON_AddNumberToObject(payload, "Day", tm->tm_mday);
	cJSON_AddNumberToObject(payload, "Year", tm->tm_year + 1900);
	cJSON_AddNumberToObject(payload, "Hour", tm->tm_hour);
	cJSON_AddNumberToObject(payload, "Minute", tm->tm_min);
	cJSON_AddNumberToObject(payload, "Second", tm->tm_sec);

	ivp_setStatusItem(gPlugin, "Month", intoToString(tm->tm_mon + 1));
	ivp_setStatusItem(gPlugin, "Day",intoToString(tm->tm_mday));
	ivp_setStatusItem(gPlugin, "Year", intoToString(tm->tm_year + 1900));
	ivp_setStatusItem(gPlugin, "Hour", intoToString(tm->tm_hour));
	ivp_setStatusItem(gPlugin, "Minute", intoToString(tm->tm_min));
	ivp_setStatusItem(gPlugin, "Second", intoToString(tm->tm_sec));

	msg = ivpMsg_create("Time", "Formatted_UTC", IVP_ENCODING_JSON, IvpMsgFlags_None, payload);
	ivp_broadcastMessage(plugin, msg);
	ivpMsg_destroy(msg);
	cJSON_Delete(payload);
}

int main()
{
	cout << "Starting UTC Plugin" << endl;

	IvpPluginInformation info = IVP_PLUGIN_INFORMATION_INITIALIZER;
	info.onMsgReceived = onMessageReceived;
	info.onStateChange = onStateChange;
	info.onError = onError;
	info.onConfigChanged = onConfigChanged;

	gPlugin = ivp_create(info);

	if (!gPlugin)
	{
		cerr << "Error creating UTC Plugin" << endl;
		return EXIT_FAILURE;
	}

	IvpMsgFilter *filter = ivpSubscribe_addFilterEntry(NULL, IVPMSG_TYPE_NMEA, "GGA");
	ivp_subscribe(gPlugin, filter);
	ivpSubscribe_destroyFilter(filter);

	uint64_t lastSendTime = 0;

	while(gPlugin->state != IvpPluginState_error)
	{
		pthread_mutex_lock(&lock);
		uint64_t frequency = gFrequency;
		pthread_mutex_unlock(&lock);

		uint64_t time = GetMsTimeSinceEpoch();

		if (frequency > 0 && (time - lastSendTime) > frequency)
		{
			lastSendTime = time;
			SendTime(gPlugin, time);
		}

		usleep(50000);
	}

	return EXIT_SUCCESS;
}
