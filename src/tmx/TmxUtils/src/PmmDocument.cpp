/*
 * PmmDocument.cpp
 *
 *  Created on: Nov 17, 2016
 *      Author: ivp
 */

#include "PmmDocument.h"

using namespace std;
using namespace pugi;

namespace tmx
{
namespace utils
{

const int TenthMicroDegree = 10000000;

const char* BasePmmMessageXml =
"<PersonalMobilityMessage>\
  <groupId>01234567-89ab-cdef-0123-456789abcdef</groupId>\
  <requestId>1</requestId>\
  <status>\
    <update/>\
  </status>\
  <position>\
    <lat>43495678</lat>\
    <long>-83134275</long>\
  </position>\
  <requestDate>\
    <year>1970</year>\
    <month>1</month>\
    <day>1</day>\
    <hour>1</hour>\
    <minute>0</minute>\
  </requestDate>\
  <pickupDate>\
    <year>1970</year>\
    <month>1</month>\
    <day>1</day>\
    <hour>1</hour>\
    <minute>0</minute>\
  </pickupDate>\
  <destination>\
    <lon>-83777777</lon>\
    <lat>43111111</lat>\
  </destination>\
  <mobilityNeeds>\
    <MobilityNeeds>\
      <type><noSpecialNeeds/></type>\
      <count>5</count>\
    </MobilityNeeds>\
  </mobilityNeeds>\
  <modeOfTransport>\
    <noPreference/>\
  </modeOfTransport>\
  <eta>0</eta>\
  <isDSRCEquipped>\
    <false/>\
  </isDSRCEquipped>\
  <vehicleDesc>TBD</vehicleDesc>\
  <regional/>\
</PersonalMobilityMessage>";


PmmDocument::PmmDocument()
{
	LoadXml(BasePmmMessageXml, "/PersonalMobilityMessage");

	// Mark all non-optional nodes as valid.
	AddValidNode("groupId");
	AddValidNode("requestId");
	AddValidNode("status");
	AddValidNode("position");
	AddValidNode("regional");
}

PmmDocument::~PmmDocument()
{
}

void PmmDocument::set_GroupId(const std::string groupId)
{
	AddValidNode("groupId");
	_rootNode.select_node("groupId").node().text().set(groupId.c_str());
}

void PmmDocument::set_RequestId(const std::string requestId)
{
	AddValidNode("requestId");
	_rootNode.select_node("requestId").node().text().set(requestId.c_str());
}

void PmmDocument::set_Status(const std::string status)
{
	AddValidNode("status");
	xml_node node = _rootNode.select_node("status").node();
	RemoveAllChildren(node);
	node.append_child(status.c_str());
}

void PmmDocument::set_Position(int latitude, int longitude)
{
	AddValidNode("position");
	_rootNode.select_node("position/lat").node().text().set(latitude);
	_rootNode.select_node("position/long").node().text().set(longitude);
}

void PmmDocument::set_RequestDate(int year, const int month, int day, int hour, int minute)
{
	SetDateNode("requestDate", year, month, day, hour, minute);
}

void PmmDocument::set_PickupDate(int year, const int month, int day, int hour, int minute)
{
	SetDateNode("pickupDate", year, month, day, hour, minute);
}

void PmmDocument::set_Destination(int latitude, int longitude)
{
	AddValidNode("destination");
	_rootNode.select_node("destination/lat").node().text().set(latitude);
	_rootNode.select_node("destination/lon").node().text().set(longitude);
}

void PmmDocument::set_MobilityNeeds(std::vector<MobilityNeed> mobilityNeeds)
{
	AddValidNode("mobilityNeeds");
	xml_node node = _rootNode.select_node("mobilityNeeds").node();
	RemoveAllChildren(node);

	for (uint i=0; i < mobilityNeeds.size(); i++)
	{
		xml_node child = node.append_child("MobilityNeeds");
		child.append_child("type").append_child(mobilityNeeds[i].Type.c_str());
		child.append_child("count").text().set(mobilityNeeds[i].Count);
	}
}

void PmmDocument::set_ModeOfTransport(const std::string modeOfTransport)
{
	AddValidNode("modeOfTransport");
	xml_node node = _rootNode.select_node("modeOfTransport").node();
	RemoveAllChildren(node);
	node.append_child(modeOfTransport.c_str());
}

void PmmDocument::set_Eta(int eta)
{
	AddValidNode("eta");
	_rootNode.select_node("eta").node().text().set(eta);
}

void PmmDocument::set_IsDsrcEquipped(bool isDSRCEquipped)
{
	AddValidNode("isDSRCEquipped");
	xml_node node = _rootNode.select_node("isDSRCEquipped").node();
	RemoveAllChildren(node);
	string value = isDSRCEquipped ? "true" : "false";
	node.append_child(value.c_str());
}

void PmmDocument::set_VehicleDesc(std::string vehicleDesc)
{
	AddValidNode("vehicleDesc");
	_rootNode.select_node("vehicleDesc").node().text().set(vehicleDesc.c_str());
}

} /* namespace utils */
} /* namespace tmx */
