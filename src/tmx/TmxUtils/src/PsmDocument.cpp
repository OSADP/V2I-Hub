/*
 * PsmDocument.cpp
 *
 *  Created on: Nov 2, 2016
 *      Author: ivp
 */

#include "PsmDocument.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Units.h"

using namespace std;
using namespace pugi;
using namespace tmx::messages;

namespace tmx
{
namespace utils
{

// Max history points is 23 from the ASN definition.
// This can be set lower if that many points are not desired.
const int PathHistoryMaxPoints = 5;

const int TenthMicroDegree = 10000000;
const int Decimeter = 10;

const char* BasePsmMessageXml =
"<PersonalSafetyMessage>\
  <basicType>\
    <unavailable/>\
  </basicType>\
  <secMark>1</secMark>\
  <msgCnt>1</msgCnt>\
  <id>29 41 25 64</id>\
  <position>\
    <lat>43495678</lat>\
    <long>-83134275</long>\
    <elevation>00 00</elevation>\
  </position>\
  <accuracy>ff ff ff ff</accuracy>\
  <speed>8191</speed>\
  <heading>28800</heading>\
  <pathHistory>\
    <initialPosition>\
      <utcTime>\
        <year>1970</year>\
        <month>1</month>\
        <day>1</day>\
        <hour>1</hour>\
        <minute>0</minute>\
        <second>0</second>\
      </utcTime>\
      <long>-830202115</long>\
      <lat>399890979</lat>\
    </initialPosition>\
    <crumbData>\
      <pathHistoryPointSets-01>\
        <PathHistoryPointType-01>\
          <latOffset>25</latOffset>\
          <longOffset>56</longOffset>\
        </PathHistoryPointType-01>\
        <PathHistoryPointType-01>\
          <latOffset>42</latOffset>\
          <longOffset>46</longOffset>\
        </PathHistoryPointType-01>\
      </pathHistoryPointSets-01>\
    </crumbData>\
  </pathHistory>\
  <pathPrediction>\
    <radiusOfCurve>32767</radiusOfCurve>\
    <confidence>1</confidence>\
  </pathPrediction>\
  <clusterSize>\
  	<unavailable/>\
  </clusterSize>\
  <clusterRadius>1</clusterRadius>\
  <regional/>\
</PersonalSafetyMessage>";

PsmDocument::PsmDocument() :
	_msgCnt(0),
	_lastLatitude(0),
	_lastLongitude(0),
	_lastElevation(0)
{
	LoadXml(BasePsmMessageXml, "/PersonalSafetyMessage");

	// Mark all non-optional nodes as valid.
	AddValidNode("basicType");
	AddValidNode("secMark");
	AddValidNode("msgCnt");
	AddValidNode("id");
	AddValidNode("position");
	AddValidNode("accuracy");
	AddValidNode("speed");
	AddValidNode("heading");
	AddValidNode("regional");
}

PsmDocument::~PsmDocument()
{
}
std::string PsmDocument::GetBasePsmString()
{
	string out(BasePsmMessageXml);
	return out;
}

uint PsmDocument::GetNextMsgCnt()
{
	if (_msgCnt == 127)
		_msgCnt = 1;
	else
		_msgCnt++;

	return _msgCnt;
}

void PsmDocument::set_BasicType(PersonalDeviceUserType_t value)
{
	const char* basicType;

	switch (value)
	{
	case PersonalDeviceUserType_aPEDESTRIAN:
		basicType = "aPEDESTRIAN";
		break;
	case PersonalDeviceUserType_aPEDALCYCLIST:
		basicType = "aPEDALCYCLIST";
		break;
	case PersonalDeviceUserType_aPUBLICSAFETYWORKER:
		basicType = "aPUBLICSAFETYWORKER";
		break;
	case PersonalDeviceUserType_anANIMAL:
		basicType = "anANIMAL";
		break;
	case PersonalDeviceUserType_unavailable:
	default:
		basicType = "unavailable";
		break;
	}

	AddValidNode("basicType");
	xml_node node = _rootNode.select_node("basicType").node();
	RemoveAllChildren(node);
	node.append_child(basicType);
}

void PsmDocument::set_ClusterRadius(int value)
{
	AddValidNode("clusterRadius");
	_rootNode.select_node("clusterRadius").node().text().set(value);
}

void PsmDocument::set_ClusterSize(NumberOfParticipantsInCluster_t value)
{
	const char* valueString;

	switch (value)
	{
	case NumberOfParticipantsInCluster_small:
		valueString = "small";
		break;
	case NumberOfParticipantsInCluster_medium:
		valueString = "medium";
		break;
	case NumberOfParticipantsInCluster_large:
		valueString = "large";
		break;
	case NumberOfParticipantsInCluster_unavailable:
	default:
		valueString = "unavailable";
		break;
	}

	AddValidNode("clusterSize");
	xml_node node = _rootNode.select_node("clusterSize").node();
	RemoveAllChildren(node);
	node.append_child(valueString);
}

void PsmDocument::set_ClusterSizeFromNumberOfParticipants(int numberOfParticipants)
{
	NumberOfParticipantsInCluster_t clusterSize = NumberOfParticipantsInCluster_unavailable;

	if (numberOfParticipants >= 0 && numberOfParticipants <= 5)
		clusterSize = NumberOfParticipantsInCluster_small;
	else if (numberOfParticipants >= 6 && numberOfParticipants <= 10)
		clusterSize = NumberOfParticipantsInCluster_medium;
	else if (numberOfParticipants > 10)
		clusterSize = NumberOfParticipantsInCluster_large;

	set_ClusterSize(clusterSize);
}

void PsmDocument::set_Id(uint32_t value)
{
	AddValidNode("id");
	_rootNode.select_node("id").node().text().set(GetOctetHexString(value).c_str());
}

void PsmDocument::set_LatLong(const WGS84Point& point)
{
	set_Latitude(point.Latitude);
	set_Longitude(point.Longitude);
}

void PsmDocument::set_Latitude(double value)
{
	_lastLatitude = (int)(value*TenthMicroDegree);
	AddValidNode("position");
	_rootNode.select_node("position/lat").node().text().set(_lastLatitude);
}

void PsmDocument::set_Longitude(double value)
{
	_lastLongitude = (int)(value*TenthMicroDegree);
	AddValidNode("position");
	_rootNode.select_node("position/long").node().text().set(_lastLongitude);
}

void PsmDocument::set_Elevation(double value)
{
	_lastElevation = (int)(value*Decimeter);

	uint16_t elevation = 0xF000; // unknown.
	if (_lastElevation >= 0 && _lastElevation <= 61439)
		elevation = _lastElevation;
	else if (_lastElevation >= -4095 && _lastElevation <= -1)
		elevation = 0xF000 & (_lastElevation*-1);

	_rootNode.select_node("position/elevation").node().text().set(GetOctetHexString(elevation).c_str());
}

void PsmDocument::set_Speed_kmph(double value)
{
	AddValidNode("speed");
	// Units are 0.2 meters per sec
	_rootNode.select_node("speed").node().text().set((int)(value * Units::MPS_PER_KPH * 50));
}

void PsmDocument::set_Heading_deg(double value)
{
	AddValidNode("heading");
	_rootNode.select_node("heading").node().text().set((int)(value * 80));
}

void PsmDocument::set_MsgCnt()
{
	AddValidNode("msgCnt");
	_rootNode.select_node("msgCnt").node().text().set((unsigned int)GetNextMsgCnt());
}

void PsmDocument::update_PathHistory(bool setInitialPosition)
{
	// The pathHistory node is optional and not used until there is at least one history point.
	if (_historyList.size() > 0)
		AddValidNode("pathHistory");

	uint64_t msTime = GetMsTimeSinceEpoch();

	// The initial position (an optional field) contains the time.
	// Currently the time is obtained from the operating system of the current device.
	// Since the Locomate times are not always correct, allow the initialPosition to not be used.
	if (setInitialPosition)
	{
		SetDateNode("pathHistory/initialPosition/utcTime", msTime);
		_rootNode.select_node("pathHistory/initialPosition/long").node().text().set(_lastLongitude);
		_rootNode.select_node("pathHistory/initialPosition/lat").node().text().set(_lastLatitude);
	}
	else
	{
		xml_node pathHistoryNode = _rootNode.select_node("pathHistory").node();
		xml_node initialPositionNode = pathHistoryNode.select_node("initialPosition").node();
		if (!initialPositionNode.empty())
			pathHistoryNode.remove_child(initialPositionNode);
	}

	// Remove any points beyond the max.
	while (_historyList.size() > PathHistoryMaxPoints)
		_historyList.pop_back();

	// Remove any previous elements inside the list element.
	xml_node listNode = _rootNode.select_node("pathHistory/crumbData/pathHistoryPointSets-01").node();
	RemoveAllChildren(listNode);

	// Add all current history points.
	for (std::list<HistoryPoint>::const_iterator it = _historyList.begin(), end = _historyList.end(); it != end; ++it)
	{
		xml_node pointNode = listNode.append_child("PathHistoryPointType-01");

		int latOffset = _lastLatitude - (*it).Latitude;
		if (latOffset >= 131071) latOffset = 131071;
		if (latOffset <= -131071) latOffset = -131071;

		int longOffset = _lastLongitude - (*it).Longitude;
		if (longOffset >= 131071) longOffset = 131071;
		if (longOffset <= -131071) longOffset = -131071;

		int elevationOffset = _lastElevation- (*it).Elevation;
		if (elevationOffset >= 2047) elevationOffset = 2047;
		if (elevationOffset <= -2047) elevationOffset = -2047;

		// LSB unit of time is 10 ms.
		int timeOffset = (int)(msTime - (*it).Time_ms) / 10;
		if (timeOffset >= 65534) timeOffset = 65534;

		pointNode.append_child("latOffset").text().set(latOffset);
		pointNode.append_child("longOffset").text().set(longOffset);
		pointNode.append_child("elevationOffset").text().set(elevationOffset);
		pointNode.append_child("timeOffset").text().set(timeOffset);
	}

	// Save the current point in the history for next time.

	HistoryPoint point;
	point.Time_ms = msTime;
	point.Latitude = _lastLatitude;
	point.Longitude = _lastLongitude;
	point.Elevation = _lastElevation;

	_historyList.push_front(point);
}

void PsmDocument::set_PathPrediction(int radiusOfCurve, int confidence)
{
	AddValidNode("pathPrediction");
	_rootNode.select_node("pathPrediction/radiusOfCurve").node().text().set(radiusOfCurve);
	_rootNode.select_node("pathPrediction/confidence").node().text().set(confidence);
}

void PsmDocument::set_PathPredictionToStraight(int confidence)
{
	set_PathPrediction(32767, confidence);
}

} /* namespace utils */
} /* namespace tmx */
