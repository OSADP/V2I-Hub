/*
 * ThreatManager.hpp
 *
 *  Created on: Jan 7, 2017
 *      Author: gmb
 */

#ifndef INCLUDE_THREATMANAGER_HPP_
#define INCLUDE_THREATMANAGER_HPP_

#include <atomic>
#include <mutex>

#include <ApplicationMessage.h>
#include <LocationMessage.h>
#include <GeoVector.h>
#include <PluginLog.h>

namespace tmx
{

namespace utils
{

template <int AppId, class LocMsgType = tmx::messages::LocationMessage>
class ThreatManager
{
public:
	template <typename PluginType>
	ThreatManager(PluginType &plugin):
		_reactionTime(0.0), _mu(0.0), _safetyOffset(0.0),
		_pluginName(plugin.GetName()), _curLocTime(0) {}

	virtual ~ThreatManager() {}

	std::atomic<double> &ReactionTime() { return _reactionTime; }
	std::atomic<double> &Friction() { return _mu; }
	std::atomic<double> &SafetyOffset() { return _safetyOffset; }

	int NumberZones()
	{
		std::lock_guard<std::mutex> lock(_lock);
		return _zones.size();
	}

	void ClearZones()
	{
		std::lock_guard<std::mutex> lock(_lock);
		_zones.clear();
	}

	void AddZone(double distance, int severity)
	{
		std::lock_guard<std::mutex> lock(_lock);
		_zones.push_back(std::pair<double, int>(distance, severity));
	}

	LocMsgType CurrentPosition()
	{
		std::lock_guard<std::mutex> lock(_lock);
		return _curLoc;
	}

	uint64_t LastUpdateTime()
	{
		std::lock_guard<std::mutex> lock(_lock);
		return _curLocTime;
	}

	void UpdateCurrentPosition(LocMsgType &msg, tmx::routeable_message &routeableMsg)
	{
		PLUGIN_LOG(logDEBUG, _pluginName) << "Incoming " <<
				battelle::attributes::type_id_name(msg) << ": " << msg;

		std::lock_guard<std::mutex> lock(_lock);
		_curLoc = msg;
		_curLocTime = routeableMsg.get_timestamp();
	}

	/**
	 * This method assumes that we are heading towards the target.
	 * Callers must ensure this is the case.
	 */
	double FindDistanceAhead(const WGS84Point &target)
	{
		LocMsgType posMsg = CurrentPosition();
		WGS84Point curPos(posMsg.get_Latitude(), posMsg.get_Longitude());

		// Make sure we are heading toward that position
		double pathToAnchorAngle = GeoVector::AngleBetweenPathsInDegrees(curPos, posMsg.get_Heading(), curPos, target);
		if (pathToAnchorAngle > 90.0 || pathToAnchorAngle < -90.0)
		{
			PLUGIN_LOG(logDEBUG, _pluginName) << "Path to point is at " << pathToAnchorAngle << " deg angle.  Moving away from the point.";
			return -1;
		}

		// Get the distance
		return GeoVector::DistanceInMeters(curPos, target);
	}

	/**
	 * Find distance from our current position to point 1 of target path
	 * taking into consideration the heading of the target path.
	 */
	double FindDistanceAhead(const WGS84Point &targetPathP1, const WGS84Point &targetPathP2)
	{
		LocMsgType posMsg = CurrentPosition();
		WGS84Point curPos(posMsg.get_Latitude(), posMsg.get_Longitude());

		/**
		 * Make sure we are heading toward that position
		 * in order to eliminate alerting cars going in opposite direction.
		 * We will look at two angles:
		 *  - Angle A between our path and path from us to first point of the target.
		 *  - Angle B between our path and target path.
		 *  If A is 0, this means that we are heading directly toward the target point 1.
		 *  Ultimately, both angles A and B must be acute for us to be considered to be heading towards the taget path.
		 */
		double sourcePathToTargetPointAngle = GeoVector::AngleBetweenPathsInDegrees(curPos, posMsg.get_Heading(), curPos, targetPathP1);
		double sourcePathToTargetPathAngle = GeoVector::AngleBetweenPathsInDegrees(curPos, posMsg.get_Heading(), targetPathP1, targetPathP2);
		if (	sourcePathToTargetPointAngle > 90.0 || sourcePathToTargetPointAngle < -90.0 ||
				sourcePathToTargetPathAngle  > 90.0 || sourcePathToTargetPathAngle  < -90.0 )
		{
			PLUGIN_LOG(logDEBUG, _pluginName) << "Source Path to target point is at " << sourcePathToTargetPointAngle << " deg angle.";
			PLUGIN_LOG(logDEBUG, _pluginName) << "Source Path to target path is at "  << sourcePathToTargetPathAngle  << " deg angle.";
			PLUGIN_LOG(logDEBUG, _pluginName) << "Moving away from the point.";
			return -1;
		}

		// Get the distance
		return GeoVector::DistanceInMeters(curPos, targetPathP1);
	}

	/**
	 * Function to calculate the distance needed to reach certain speed based on
	 * the current speed of the vehicle and the coefficient of
	 * friction between the tires and road for the current vehicle.
	 * There are a couple of assumptions that simplify the calculation
	 * of stopping distance. By including a safety offset to error on the side
	 * of caution we can ignore the fact that friction is not constant in vehicle
	 * dynamics.
	 *
	 * @param incline the slope of the road in degrees, positive is incline negative is decline
	 * @param initial speed in Speed, meters/sec obtained from current Location if not provided.
	 * @param final speed in meters/sec - defaulted to 0 for a complete stop.
	 *
	 * @return The distance needed to stop in meters
	 */
	double GetStoppingDistance(double incline = 0, double speed_mps = -1, double finalSpeed_mps = 0)
	{
		if (speed_mps < 0)
			speed_mps = CurrentPosition().get_Speed_mps();

		double distance = 0.0;
		distance = (_reactionTime * speed_mps) + abs(((finalSpeed_mps*finalSpeed_mps) - (speed_mps * speed_mps)) / (2 * 9.8 * ((_mu * cos(incline)) + sin(incline))));

		return distance;
	}

	/**
	 * Returns an empty application message (@see tmx::messages::appmessage::NOAPPID) if the current target is
	 * not a thread, or a filled in application message if the threat is real, i.e. inside a specifed zone;
	 * @param The distance between source and target in meters
	 */
	tmx::messages::ApplicationMessage Assess(double dist)
	{
		tmx::messages::ApplicationMessage Ret;

		if (dist < 0)
			return Ret;

		int zone = -1;

		// Iterate backwards through the zones
		std::lock_guard<std::mutex> lock(_lock);
		for (int i = _zones.size() - 1; i >= 0; i--)
		{
			if (dist < _zones[i].first)
			{
				zone = i;
				break;
			}
		}

		if (zone < 0)
			return Ret;

		// This is a threat
		PLUGIN_LOG(logINFO, _pluginName) << "Vehicle " << dist << " m from the target, detected in zone " << zone;

		Ret.set_AppId((tmx::messages::appmessage::ApplicationTypes)AppId);
		Ret.set_Timestamp(std::to_string(_curLocTime));		// Default time
		Ret.set_EventID("Zone" + std::to_string(zone));
		Ret.set_Severity((tmx::messages::appmessage::Severity)_zones[zone].second);

		return Ret;
	}

	/**
	 * Returns an empty application message (@see tmx::messages::appmessage::NOAPPID) if the current target is
	 * not a thread, or a filled in application message if the threat is real, i.e. inside a specifed zone;
	 */
	tmx::messages::ApplicationMessage Assess(WGS84Point target)
	{
		return Assess(FindDistanceAhead(target));
	}


	/**
	 * Returns an empty application message (@see tmx::messages::appmessage::NOAPPID) if the current target is
	 * not a thread, or a filled in application message if the threat is real, i.e. inside a specifed zone;
	 * This version of a method takes a path of a target allowing it to compare our heading to the heading of the target path as well.
	 */
	tmx::messages::ApplicationMessage Assess(WGS84Point targetPathP1, WGS84Point targetPathP2)
	{
		return Assess(FindDistanceAhead(targetPathP1, targetPathP2));
	}
private:
	std::atomic<double> _reactionTime;
	std::atomic<double> _mu;
	std::atomic<double> _safetyOffset;

	std::string _pluginName;

	std::mutex _lock;
	LocMsgType _curLoc;

	std::atomic<uint64_t> _curLocTime;
	std::vector< std::pair<double, int> > _zones;
};

}	/* namespace utils */
}	/* namespace tmx */



#endif /* INCLUDE_THREATMANAGER_HPP_ */
