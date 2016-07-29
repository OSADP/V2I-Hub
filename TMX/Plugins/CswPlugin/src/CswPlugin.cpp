//============================================================================
// Name        : CswPlugin.cpp
// Author      : Battelle Memorial Institute
// Version     :
// Copyright   : Copyright (c) 2014 Battelle Memorial Institute. All rights reserved.
// Description : Curve Speed Warning (CSW) Plugin
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <map>
#include <tmx/tmx.h>
#include <tmx/IvpPlugin.h>
#include <tmx/messages/IvpJ2735.h>
#include <asn_j2735_r41/TravelerInformation.h>
#include "TimeHelper.h"
#include "XmlCurveParser.h"
#include "VehicleLocate.h"
#include <tmx/messages/IvpDmsControlMsg.h>
#include "PluginUtil.h"

using namespace std;
using namespace tmx::utils;

pthread_mutex_t _settingsMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t _timMutex = PTHREAD_MUTEX_INITIALIZER;

IvpPlugin *_plugin = NULL;
uint64_t _frequency = 0;
uint64_t _snapInterval = 0;
uint64_t _vehicleTimeout = 1000;

TravelerInformation _tim;
std::string _mapFile;
bool _isMapFileNew = false;
bool _isTimLoaded = false;
unsigned int _speedLimit = 0;
int _lastMsgIdSent = -1;

struct ZoneInfo
{
	// ID of the zone the vehicle is in.  0 indicates no zone.
	int ZoneId;

	// The current speed of the vehicle in MPH.
	float Speed_mph;

	// The time the ZoneId was last updated.
	// Note that if a 0 is received for ZoneId, this time will not be set unless
	// it has been longer than a configurable duration.
	uint64_t LastUpdateTime;
};

// For each vehicle ID, store what zone they are in.
map<int32_t, ZoneInfo> _vehicleInZone;

// Update the zone and speed for the vehicle id specified.
void UpdateVehicleInZone(int32_t vehicleId, int zoneId, float speed_mph)
{
	uint64_t now = TimeHelper::GetMsTimeSinceEpoch();

	// If msgId is 0, the vehicle is removed from the map.
	// If msgId is not 0, the vehicle is either added or updated in the map.

	std::map<int32_t, ZoneInfo>::iterator it = _vehicleInZone.find(vehicleId);
	if (it == _vehicleInZone.end())
	{
		if (zoneId != 0)
		{
			cout << "Adding   Vehicle ID: " << vehicleId << ", zone: " << zoneId << ", speed: " << speed_mph << endl;

			ZoneInfo zoneInfo;
			zoneInfo.ZoneId = zoneId;
			zoneInfo.Speed_mph = speed_mph;
			zoneInfo.LastUpdateTime = now;

			_vehicleInZone.insert(std::pair<int32_t, ZoneInfo>(vehicleId, zoneInfo));
		}
	}
	else
	{
		pthread_mutex_lock(&_settingsMutex);
		uint64_t snapInterval = _snapInterval;
		pthread_mutex_unlock(&_settingsMutex);

		if (zoneId == 0 && (now - it->second.LastUpdateTime) < snapInterval)
		{
			cout << "Removing Vehicle ID: " << vehicleId << ", zone: " << zoneId << ", speed: " << speed_mph << endl;
			_vehicleInZone.erase(vehicleId);
		}
		else
		{
			if (it->second.ZoneId != zoneId || it->second.Speed_mph != speed_mph)
			{
				cout << "Updating Vehicle ID: " << vehicleId << ", zone: " << zoneId << ", speed: " << speed_mph << endl;
				it->second.ZoneId = zoneId;
				it->second.Speed_mph = speed_mph;
			}
			it->second.LastUpdateTime = now;
		}
	}
}

// Remove any vehicles where a BSM has not been received within the timeout period.
void RemoveOldVehicles()
{
	uint64_t now = TimeHelper::GetMsTimeSinceEpoch();

	pthread_mutex_lock(&_settingsMutex);
	uint64_t vehicleTimeout = _vehicleTimeout;
	pthread_mutex_unlock(&_settingsMutex);

	std::map<int32_t, ZoneInfo>::iterator it = _vehicleInZone.begin();

	while (it != _vehicleInZone.end())
	{
	    if (now - it->second.LastUpdateTime > vehicleTimeout)
	    {
	    	std::map<int32_t, ZoneInfo>::iterator toErase = it;
	       it++;
	       _vehicleInZone.erase(toErase);
	    }
	    else
	    {
	       it++;
	    }
	}
}

// Get the zone id with the highest priority of all vehicles that are speeding.
int GetHighestPriorityZoneId(unsigned int speedLimit_mph)
{
	// Remove any old vehicles that have timed out.
	RemoveOldVehicles();

	// The lower the zone id, the higher the priority.

	int id = 9999;

	for (auto& kv : _vehicleInZone)
	{
		if (kv.second.ZoneId != 0 && id > kv.second.ZoneId && kv.second.Speed_mph > speedLimit_mph)
			id = kv.second.ZoneId;
	}

	return id == 9999 ? 0 : id;
}

void SetStatusForVehiclesInZones(IvpPlugin *plugin)
{
	ostringstream ss;
	ss.precision(1);

	bool first = true;
	for (auto& kv : _vehicleInZone)
	{
		if (first)
			first = false;
		else
			ss << ", ";

		ss << "[ID: " << kv.first << ", Zone: " << kv.second.ZoneId << ", Speed: " << fixed << kv.second.Speed_mph << "]";
	}

	if (first)
		ss << "None";

	PluginUtil::SetStatus<string>(plugin, "Vehicles In Zones", ss.str());
}

void GetConfigSettings()
{
	PluginUtil::GetConfigValue(_plugin, "Frequency", &_frequency, &_settingsMutex);

	if (PluginUtil::GetConfigValue(_plugin, "MapFile", &_mapFile, &_settingsMutex))
		_isMapFileNew = true;

	PluginUtil::GetConfigValue(_plugin, "Snap Interval", &_snapInterval, &_settingsMutex);
	PluginUtil::GetConfigValue(_plugin, "Vehicle Timeout", &_vehicleTimeout, &_settingsMutex);
}

void onStateChange(IvpPlugin *plugin, IvpPluginState state)
{
	std::cout << "State Change: " << PluginUtil::IvpPluginStateToString(state) << std::endl;

	if (state == IvpPluginState_registered)
	{
		GetConfigSettings();
	}
}

void onError(IvpPlugin *plugin, IvpError err)
{
	std::cerr << "Level: " << err.level << ", Error: " << err.error << ", Sys Error: " << err.sysErrNo << std::endl;
}

void onMessageReceived(IvpPlugin *plugin, IvpMessage *msg)
{
	assert(msg != NULL);

	//printf("Received Message - Type: %s, Subtype: %s, Source: %s\n", msg->type, msg->subtype, msg->source);

	if (msg->type && strcmp(msg->type, "J2735") == 0
			&& msg->subtype && strcmp(msg->subtype, "BSM") == 0
			&& msg->payload && msg->payload->type == cJSON_String)
	{
		pthread_mutex_lock(&_timMutex);
		if (_isTimLoaded)
		{
			// Process the BSM, getting the message ID (zone) to send to the DMS.
			// The vehicle ID and speed are also extracted from the BSM.

			int regionNumber;
			float speed_mph;
			int32_t vehicleId;

			if (VehicleLocate::ProcessBsmMessage(msg, &_tim, &regionNumber, &speed_mph, &vehicleId))
			{
				// Store the ID in a map, where the vehicle ID is the key, and the value is the speed
				// and zone/region where the vehicle is currently at.
				UpdateVehicleInZone(vehicleId, regionNumber, speed_mph);

				// Get the highest priority id for all vehicles that are speeding.
				int zoneId = GetHighestPriorityZoneId(_speedLimit);

				//cout << "Highest Priority Zone: " << zoneId << endl;

				// Message ID is the ID of the message to display on the sign.
				// An ID of 0 blanks the sign.
				int msgId = 0;

				// Only show a message on the sign when in zones 1, 2, or 3.
				// Always show the same message (ID 3).  Otherwise the sign is blank.
				if (zoneId >= 1 && zoneId <= 3)
					msgId = 3;

				if (_lastMsgIdSent != msgId)
				{
					printf("Sending DMS Message ID: %d\n", msgId);
					_lastMsgIdSent = msgId;

					IvpMessage *actionMsg = ivpDmsCont_createMsg(msgId);
					if (actionMsg!=NULL)
					{
						ivp_broadcastMessage(_plugin, actionMsg);
						ivpMsg_destroy(actionMsg);

						PluginUtil::SetStatus<int>(_plugin, "DMS Message ID Sent", msgId);
					}
				}

				SetStatusForVehiclesInZones(plugin);
			}
		}
		pthread_mutex_unlock(&_timMutex);
	}
}

void onConfigChanged(IvpPlugin *plugin, const char *key, const char *value)
{
	std::cout << "Config Changed. Key: " << key << ", Value: " << value << std::endl;
	GetConfigSettings();
}

bool LoadTim(TravelerInformation *tim)
{
	memset(tim, 0, sizeof(TravelerInformation));

	// J2735 packet header.

	tim->msgID = DSRCmsgID_travelerInformation;

	DsrcBuilder::SetPacketId(tim);

	// Data Frame (1 of 1).

	XmlCurveParser curveParser;

	std::cout << "Loading curve file: " << _mapFile << std::endl;

	// Read the curve file, which creates and populates the data frame of the TIM.
	if (!curveParser.ReadCurveFile(_mapFile, tim))
		return false;

	// Verify that a single data frame was added by the parser.
	if (tim->dataFrameCount == NULL || *tim->dataFrameCount != 1)
		return false;

	std::cout << "TIM was created." << std::endl;

	_speedLimit = curveParser.SpeedLimit;

	PluginUtil::SetStatus<unsigned int>(_plugin, "Speed Limit", _speedLimit);

	return true;
}

void TestFindRegion()
{
	if (_isTimLoaded)
	{
		WGS84Point point;
		point.Latitude = 42.2891;
		point.Longitude = -83.71933195;
		int regionNumber = VehicleLocate::FindRegion(&_tim, point, 0);
		std::cout << "In Region: " << regionNumber << std::endl;
	}
}

int main()
{
	std::cout << "Starting CSW Plugin" << std::endl;

	IvpPluginInformation info = IVP_PLUGIN_INFORMATION_INITIALIZER;
	info.onMsgReceived = onMessageReceived;
	info.onStateChange = onStateChange;
	info.onError = onError;
	info.onConfigChanged = onConfigChanged;

	_plugin = ivp_create(info);

	if (!_plugin)
	{
		std::cerr << "Error creating CSW Plugin" << std::endl;
		return EXIT_FAILURE;
	}

	IvpMsgFilter *filter = NULL;
	filter = ivpSubscribe_addFilterEntry(filter, "J2735", "BSM");
	ivp_subscribe(_plugin, filter);
	ivpSubscribe_destroyFilter(filter);

	// Set the update frequency to once per day.
	// It needs updated periodically because the TIM is only valid for about 1.5 months.
	uint64_t updateFrequency = 24 * 60 * 60 * 1000;
	uint64_t lastUpdateTime = 0;

	uint64_t lastSendTime = 0;

	while (_plugin->state != IvpPluginState_error)
	{
		pthread_mutex_lock(&_settingsMutex);
		uint64_t sendFrequency = _frequency;
		pthread_mutex_unlock(&_settingsMutex);

		// Load the TIM from the map file if it is new.
		if (_isMapFileNew)
		{
			_isMapFileNew = false;

			pthread_mutex_lock(&_timMutex);
			if (_isTimLoaded)
				ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_TravelerInformation, &_tim);
			_isTimLoaded = LoadTim(&_tim);
			//TestFindRegion();
			pthread_mutex_unlock(&_timMutex);
		}

		// Get system time in milliseconds.
		uint64_t time = TimeHelper::GetMsTimeSinceEpoch();

		// Update the start time of the TIM message if it is time.
		// Since the contents of the TIM change, the packet ID is also updated.
		// If the packet ID is not changed, a recipient may choose to ignore it.
		if (_isTimLoaded && (time - lastUpdateTime) > updateFrequency)
		{
			lastUpdateTime = time;
			pthread_mutex_lock(&_timMutex);
			if (_isTimLoaded)
			{
				std::cout << "Updating TIM start time." << std::endl;
				DsrcBuilder::SetPacketId(&_tim);
				DsrcBuilder::SetStartTimeToYesterday(_tim.dataFrames.list.array[0]);
			}
			pthread_mutex_unlock(&_timMutex);
 		}

		// Send out the TIM at the frequency read from the configuration.
		if (_isTimLoaded && sendFrequency > 0 && (time - lastSendTime) > sendFrequency)
		{
			lastSendTime = time;
            IvpMessage *msg = ivpJ2735_createMsg(&_tim, DSRCmsgID_travelerInformation, IvpMsgFlags_RouteDSRC);
            if (msg != NULL)
            {
            	ivpMsg_addDsrcMetadata(msg, 172, 0x0025);
                ivp_broadcastMessage(_plugin, msg);
                ivpMsg_destroy(msg);
            }
		}

		usleep(50000);
	}

	return EXIT_SUCCESS;
}
