/*
 * PvdDocument.cpp
 *
 *  Created on: Nov 8, 2016
 *      Author: svj
 */

#include "PvdDocument.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Units.h"

using namespace std;
using namespace pugi;
using namespace tmx::messages;

#define TENTHMICRODEGREE 10000000

namespace tmx
{
namespace utils
{

const char* BasePvdMessageXml =
"<ProbeVehicleData>\
  <msgID>\
    <probeVehicleData />\
  </msgID>\
  <segNum>0</segNum>\
  <probeID>\
    <name>Simulated Vehicle</name>\
    <vin>11111aaaaabbbbbbcccccc</vin>\
    <ownerCode>Unknown</ownerCode>\
  </probeID>\
  <startVector>\
    <utcTime>\
      <year>0</year>\
      <month>0</month>\
      <day>0</day>\
      <hour>31</hour>\
      <minute>60</minute>\
      <second>65535</second>\
    </utcTime>\
    <long>1800000001</long>\
    <lat>900000001</lat>\
    <elevation>F0 00</elevation>\
    <heading>2880</heading>\
    <speed>1f ff</speed>\
  </startVector>\
  <vehicleType><unknown/></vehicleType>\
  <cntSnapshots>0</cntSnapshots>\
  <snapshots></snapshots>\
</ProbeVehicleData>";

PvdDocument::PvdDocument()
{
	LoadXml(BasePvdMessageXml, "/ProbeVehicleData");

	// Mark all non-optional nodes as valid.
	AddValidNode("msgID");
	AddValidNode("startVector");
	AddValidNode("vehicleType");
	AddValidNode("cntSnapshots");
	AddValidNode("snapshots");
}

PvdDocument::~PvdDocument()
{
}

void PvdDocument::set_LatLong(xml_node &node, const WGS84Point& point)
{
	set_Latitude(node, point.Latitude);
	set_Longitude(node, point.Longitude);
}

void PvdDocument::set_Latitude(xml_node &node, double value)
{
	node.child("lat").text().set((int)(value * TENTHMICRODEGREE));
}

void PvdDocument::set_Longitude(xml_node &node, double value)
{
	node.child("long").text().set((int)(value * TENTHMICRODEGREE));
}

void PvdDocument::set_Speed_kmph(xml_node &node, double value)
{
	// Unit is 0.2 meters per second
	uint16_t speed = value * Units::MPS_PER_KPH * 50;
	node.child("speed").text().set(GetOctetHexString(speed).c_str());
}
void PvdDocument::set_Elevation(xml_node &node, double value)
{
	node.child("elevation").text().set((int)(value * 10));
}

void PvdDocument::set_Heading_deg(xml_node &node, double value)
{
	node.child("heading").text().set((int)(value * 80));
}

void PvdDocument::fillLocationData(xml_node &node, WGS84Point loc, double altitude, double heading, double speed_kph)
{
	set_LatLong(node, loc);
	set_Elevation(node, altitude);
	set_Heading_deg(node, heading);
	set_Speed_kmph(node, speed_kph);
}

std::string PvdDocument::GetBasePvdString()
{
	string out(BasePvdMessageXml);

	return out;
}

void PvdDocument::set_StartVector(WGS84Point loc, double altitude, double heading, double speed_kph, uint64_t timestamp)
{
	AddValidNode("startVector");
	SetDateNode("startVector/utcTime",timestamp);
	xml_node start = _rootNode.select_node("startVector").node();
	fillLocationData(start, loc, altitude, heading, speed_kph);
}

void PvdDocument::add_Snapshot(WGS84Point loc, double altitude, double heading, double speed_kph, uint64_t timestamp)
{
	AddValidNode("snapshots");
	xpath_node docNode = _rootNode.select_node("snapshots");
	xml_node positionNode = docNode.node().append_child("Snapshot").append_child("thePosition");

	xml_node dateNode = positionNode.append_child("utcTime");
	dateNode.append_child("year");
	dateNode.append_child("month");
	dateNode.append_child("day");
	dateNode.append_child("hour");
	dateNode.append_child("minute");
	dateNode.append_child("second");
	SetDateNode(dateNode,timestamp);

	positionNode.append_child("long");
	positionNode.append_child("lat");
	positionNode.append_child("elevation");
	positionNode.append_child("heading");
	positionNode.append_child("speed");
	fillLocationData(positionNode, loc, altitude, heading, speed_kph);
}

void PvdDocument::set_cntSnapshots(uint32_t value)
{
	AddValidNode("cntSnapshots");
	_rootNode.select_node("cntSnapshots").node().text().set(value);

	if (value == 0)
	{
		// Remove any previous snapshots.
		xml_node snapshotNode = _rootNode.select_node("snapshots").node();
		RemoveAllChildren(snapshotNode);
	}
}

void PvdDocument::set_VehicleType(std::string value)
{
	AddValidNode("vehicleType");
	xml_node node = _rootNode.select_node("vehicleType").node();
	RemoveAllChildren(node);
	node.append_child(value.c_str());
}

void PvdDocument::set_SegNum(std::string value)
{
	AddValidNode("segNum");
	_rootNode.select_node("segNum").node().text().set(value.c_str());
}

void PvdDocument::set_Name(std::string value)
{
	AddValidNode("probeID");
	_rootNode.select_node("probeID/name").node().text().set(value.c_str());
}

void PvdDocument::set_Vin(std::string value)
{
	AddValidNode("probeID");
	_rootNode.select_node("probeID/vin").node().text().set(value.c_str());
}
void PvdDocument::set_OwnerCode(std::string value)
{
	AddValidNode("probeID");
	_rootNode.select_node("probeID/ownerCode").node().text().set(value.c_str());
}

void PvdDocument::set_VehicleType(VehicleType_t value)
{
	std::string vehicleType;

	switch (value)
	{
	case VehicleType_none:
		vehicleType = "none";
		break;
	case VehicleType_special:
		vehicleType = "special";
		break;
	case VehicleType_moto:
		vehicleType = "moto";
		break;
	case VehicleType_car:
		vehicleType = "car";
		break;
	case VehicleType_carOther:
		vehicleType = "carOther";
		break;
	case VehicleType_bus:
		vehicleType = "bus";
		break;
	case VehicleType_axleCnt2:
		vehicleType = "axleCnt2";
		break;
	case VehicleType_axleCnt3:
		vehicleType = "axleCnt3";
		break;
	case VehicleType_axleCnt4:
		vehicleType = "axleCnt4";
		break;
	case VehicleType_axleCnt4Trailer:
		vehicleType = "axleCnt4Trailer";
		break;
	case VehicleType_axleCnt5Trailer:
		vehicleType = "axleCnt5Trailer";
		break;
	case VehicleType_axleCnt6Trailer:
		vehicleType = "axleCnt6Trailer";
		break;
	case VehicleType_axleCnt5MultiTrailer:
		vehicleType = "axleCnt5MultiTrailer";
		break;
	case VehicleType_axleCnt6MultiTrailer:
		vehicleType = "axleCnt6MultiTrailer";
		break;
	case VehicleType_axleCnt7MultiTrailer:
		vehicleType = "axleCnt7MultiTrailer";
		break;
	case VehicleType_unknown:
	default:
		vehicleType = "unknown";
		break;
	}
	set_VehicleType(vehicleType);
}

bool PvdDocument::is_LatLongValid()
{
	int lat = stoi(_rootNode.select_node("startVector/lat").node().text().get());
	int lon = stoi(_rootNode.select_node("startVector/long").node().text().get());

	return (lat != 900000001 && lon != 1800000001);
}

std::string PvdDocument::get_Name()
{
	return std::string(_rootNode.select_node("probeID/name").node().text().get());
}


} /* namespace utils */
} /* namespace tmx */
