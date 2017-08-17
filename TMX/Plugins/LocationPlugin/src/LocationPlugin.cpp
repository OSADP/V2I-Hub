//============================================================================
// Name        : LocationPlugin.cpp
// Author      : Battelle Memorial Institute - Matt Cline (cline@battelle.org)
// Version     :
// Copyright   : Battelle 2016
// Description : Plugin to send location messages after gathering the information
//				 from gpsd.
//============================================================================

#include <mutex>
#include <thread>

#define USE_STD_CHRONO
#include <FrequencyThrottle.h>

#include <PluginClient.h>
#include <LocationMessage.h>
#include <LocationMessageEnumTypes.h>
#include <PluginDataMonitor.h>
#include <Uuid.h>

#include <gps.h>

using namespace std;
using namespace tmx;
using namespace tmx::utils;
using namespace tmx::messages;
using namespace tmx::messages::location;

namespace LocationPlugin{

class LocationPlugin : public PluginClient
{

public:
	LocationPlugin(std::string name);
	~LocationPlugin();
	int Main();

protected:
	void UpdateConfigSettings();

	// virtual function overrides
	void OnConfigChanged(const char* key, const char* value);
	void OnStateChange(IvpPluginState state);

	// Message handler
	void HandleDataChange(tmx::messages::DataChangeMessage &, tmx::routeable_message &);
private:
	std::mutex _dataLock;
	std::atomic<uint64_t> _frequency;
	std::atomic<double> _latchSpeed;
	std::atomic<double> _lastHeading;
	std::atomic<bool> _configSet;
	std::atomic<bool> _monitoringActive;
	std::atomic<bool> _gpsGood;
	std::atomic<int> _gpsError;

	// Need to monitor this value
	std::string _gpsdHost;
	DATA_MONITOR(_gpsdHost);

	// Location Message data for status
	std::atomic<uint64_t> _time;
	std::atomic<int> _numSats;
	std::atomic<int> _fix;
	std::atomic<int> _quality;
	std::atomic<double> _alt;
	std::atomic<double> _lat;
	std::atomic<double> _lon;
	std::atomic<double> _mph;
	std::atomic<double> _heading;
	std::atomic<double> _hdop;

	// Throttle for messages
	FrequencyThrottle<int> _throttle;

	void MonitorGPS();
};

const char *ToString(FixTypes fix)
{
	switch (fix)
	{
	case FixTypes::NoFix: return FIXTYPES_NOFIX_STRING;
	case FixTypes::TwoD: return FIXTYPES_TWOD_STRING;
	case FixTypes::ThreeD: return FIXTYPES_THREED_STRING;
	default: return FIXTYPES_UNKNOWN_STRING;
	}
}

const char *ToString(SignalQualityTypes quality)
{
	switch (quality)
	{
	case SignalQualityTypes::GPS: return SIGNALQUALITYTYPES_GPS_STRING;
	case SignalQualityTypes::DGPS: return SIGNALQUALITYTYPES_DGPS_STRING;
	case SignalQualityTypes::PPS: return SIGNALQUALITYTYPES_PPS_STRING;
	case SignalQualityTypes::RealTimeKinematic: return SIGNALQUALITYTYPES_REALTIMEKINEMATIC_STRING;
	case SignalQualityTypes::FloatRTK: return SIGNALQUALITYTYPES_FLOATRTK_STRING;
	case SignalQualityTypes::DeadReckoning: return SIGNALQUALITYTYPES_DEADRECKONING_STRING;
	case SignalQualityTypes::ManualInputMode: return SIGNALQUALITYTYPES_MANUALINPUTMODE_STRING;
	case SignalQualityTypes::SimulationMode: return SIGNALQUALITYTYPES_SIMULATIONMODE_STRING;
	default: return SIGNALQUALITYTYPES_INVALID_STRING;
	}
}

/**
 * Default Constructor. Good Place to initialize atomic variables.
 *
 * @param name string used to identify the plugin.
 */
LocationPlugin::LocationPlugin(std::string name) : PluginClient(name)
{
	_frequency = 500;
	_configSet = false;
	_monitoringActive = false;

	AddMessageFilter<tmx::messages::DataChangeMessage>(this, &LocationPlugin::HandleDataChange);
	SubscribeToMessages();
}

/**
 * Default Deconstructor.
 */
LocationPlugin::~LocationPlugin()
{

}

/**
 * Called to update the configuration parameters stored in the database.
 */
void LocationPlugin::UpdateConfigSettings()
{
	{
		lock_guard<mutex> lock(_dataLock);
		GetConfigValue("Frequency", _frequency);
		GetConfigValue("LatchHeadingSpeed", _latchSpeed);
		GetConfigValue("GPSSource", _gpsdHost);
		_throttle.set_Frequency(std::chrono::milliseconds(_frequency));
	}

	__gpsdHost_mon.check();
	_configSet = true;
}

/**
 * Function called when a configuration parameter is updated
 *
 * @param key the name of the parameter updated
 * @param value the new value of the parameter
 */
void LocationPlugin::OnConfigChanged(const char* key, const char* value)
{
	PluginClient::OnConfigChanged(key, value);
	UpdateConfigSettings();
}

/**
 * Function called when the state of the plugin changes
 *
 * @param state the new state of the plugin
 */
void LocationPlugin::OnStateChange(IvpPluginState state)
{
	PluginClient::OnStateChange(state);

	if(state == IvpPluginState_registered)
	{
		UpdateConfigSettings();
	}
}

void LocationPlugin::HandleDataChange(DataChangeMessage &msg, routeable_message &routeableMsg)
{
	// Reset the connection by setting monitoring active to false
	_monitoringActive = false;
}

/**
 * Monitors changes reported by gpsd on its own thread and populates the
 * fields used in the Location Message.
 */
void LocationPlugin::MonitorGPS()
{
	struct gps_data_t gps_data;
	int oStatus = -1;

	while(_plugin->state != IvpPluginState_error)
	{
		if (!_monitoringActive)
		{
			_gpsGood = false;
			_gpsError = EPIPE;

			// If monitoring was stopped while we were processing an open connection, then
			// close the stream and wait a few seconds
			if (oStatus == 0)
			{
				PLOG(logWARNING) << "Shutting down active GPSD connection";
				gps_stream(&gps_data, WATCH_DISABLE, NULL);
				gps_close(&gps_data);

				sleep(2);
			}

			// Do not connect unless we have a good value for the host
			lock_guard<mutex> lock(_dataLock);
			if (_gpsdHost.empty())
			{
				sleep(1);
				continue;
			}

			PLOG(logINFO) << "Connecting to GPS on " << _gpsdHost << ":" << DEFAULT_GPSD_PORT;

			oStatus = gps_open(_gpsdHost.c_str(), DEFAULT_GPSD_PORT, &gps_data);

			if(oStatus < 0)
			{
				PLOG(logERROR) << "Problem opening socket to GPSD: " << gps_errstr(errno);

				// Sleep some time before trying again
				sleep(5);
				continue;
			}

			PLOG(logDEBUG) << "Watching GPSD on " << _gpsdHost;

			gps_stream(&gps_data, WATCH_ENABLE | WATCH_JSON, NULL);

			_monitoringActive = true;
		}

		LocationMessage msg;

		if((_gpsGood = gps_waiting(&gps_data, _frequency * 1000)))
		{
			int bytes = gps_read(&gps_data);
			_gpsGood = (bytes > 0);

			if (bytes == 0)	// No data returned
			{
				_gpsError = ENODATA;
				continue;
			}
			else if (bytes < 0)	// True read error
			{
				PLOG(logERROR) << "Error reading GPS data from the socket.";
				_gpsError = EIO;

				// Reconnect
				_monitoringActive = false;
				continue;
			}
			else
			{
				_gpsError = 0;
			}

			uint64_t gpsTime = (uint64_t)(1000 * gps_data.fix.time);
			msg.set_Id(Uuid::NewGuid());
			msg.set_FixQuality((FixTypes)gps_data.fix.mode);
			msg.set_SignalQuality(msg.get_FixQuality() > 0 ? SignalQualityTypes::GPS : SignalQualityTypes::Invalid);
			msg.set_Time(std::to_string(gpsTime));
			msg.set_NumSatellites(gps_data.satellites_used);
			if(gps_data.fix.mode > 1)
			{
				msg.set_Latitude(gps_data.fix.latitude);
				msg.set_Longitude(gps_data.fix.longitude);
				msg.set_Speed_mps(gps_data.fix.speed); //fix speed is in m/s

				if (_latchSpeed > msg.get_Speed_mph())
				{
					msg.set_Heading(_lastHeading);
				}
				else
				{
					msg.set_Heading(gps_data.fix.track);
					_lastHeading = msg.get_Heading();
				}

				if(gps_data.fix.mode == 3)
				{
					msg.set_Altitude(gps_data.fix.altitude);
				}
				if(!std::isnan(gps_data.dop.hdop))
				{
					msg.set_HorizontalDOP(gps_data.dop.hdop);
				}
			}

			// Broadcast the message
			if (_throttle.Monitor(0))
			{
				PLOG(logDEBUG) << "Broadcasting Location Message: " << msg;
				BroadcastMessage(msg);
			}

			// Save off the latest message data for use later
			_time = gpsTime;
			_numSats = msg.get_NumSatellites();
			_fix = (int)msg.get_FixQuality();
			_quality = (int)msg.get_SignalQuality();
			_alt = msg.get_Altitude();
			_lat = msg.get_Latitude();
			_lon = msg.get_Longitude();
			_mph = msg.get_Speed_mph();
			_heading = msg.get_Heading();
			_hdop = msg.get_HorizontalDOP();
		}
	}

	if (oStatus == 0)
	{
		gps_stream(&gps_data, WATCH_DISABLE, NULL);
		gps_close(&gps_data);
	}
}

/**
 * Main Function logic to execute on a separate thread
 *
 * @return exit code
 */
int LocationPlugin::Main()
{
	PLOG(logINFO) << "Starting Plugin.";

	while(!_configSet)
	{
		usleep(10000);
	}

	// Monitor GPS on a separate thread
	std::thread gps(&LocationPlugin::MonitorGPS, this);

	if(!_monitoringActive)
	{
		usleep(10000);
	}

	while(_plugin->state != IvpPluginState_error)
	{

		std::chrono::milliseconds epochTime(_time);
		std::chrono::time_point<std::chrono::system_clock> tp(epochTime);

		LocationMessage msg;
		int fix = _fix;
		int quality = _quality;

		msg.set_NumSatellites(_numSats);
		msg.set_FixQuality((FixTypes) fix);
		msg.set_SignalQuality((SignalQualityTypes) quality);
		msg.set_Altitude(_alt);
		msg.set_Latitude(_lat);
		msg.set_Longitude(_lon);
		msg.set_Speed_mph(_mph);
		msg.set_Heading(_heading);
		msg.set_HorizontalDOP(_hdop);

		SetStatus("Last GPS Time (UTC)", Clock::ToUtcPreciseTimeString(tp));
		SetStatus("Current Time (UTC)", Clock::ToUtcPreciseTimeString(std::chrono::system_clock::now()));
		SetStatus("Number of Satellites", msg.get_NumSatellites());
		SetStatus("Fix Quality", ToString(msg.get_FixQuality()));
		SetStatus("Signal Quality", ToString(msg.get_SignalQuality()));
		SetStatus("Altitude", msg.get_Altitude());
		SetStatus("Latitude", msg.get_Latitude(), false, 10);
		SetStatus("Longitude", msg.get_Longitude(), false, 10);
		SetStatus("Speed (MPH)", msg.get_Speed_mph());
		SetStatus("Heading", msg.get_Heading());
		SetStatus("HDOP", msg.get_HorizontalDOP(), false, 4);

		std::string status("Connection ");
		if (_gpsGood)
			status += "Active";
		else
		{
			status += "Inactive: ";
			status += strerror(_gpsError);
		}

		SetStatus("GPS Status", status);

		// Only record every 3 seconds
		sleep(3);
	}

	gps.join();

	return 0;
}


} /* namespace LocationPlugin */

int main(int argc, char* argv[])
{
	return run_plugin<LocationPlugin::LocationPlugin>("Location", argc, argv);
}
