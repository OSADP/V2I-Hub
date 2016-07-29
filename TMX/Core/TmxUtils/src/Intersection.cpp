/*
 * Intersection.cpp
 *
 *  Created on: Apr 29, 2016
 *      Author: ivp
 */

#include "Intersection.h"
#include "ParsedMap.h"

#include <tmx/messages/message_document.hpp>
#include <tmx/pugixml/pugixml.hpp>
using namespace std;
using namespace tmx;
using namespace tmx::messages;
using namespace pugi;

namespace tmx {
namespace utils {

Intersection::Intersection() :
		_mapVersion(-1), _intersectionId(-1){


	//Region is a small class, add a dummy at 0 so that the indexes line up to the numbers to reduce errors.
	Region dummy(0);
	Regions.push_back(dummy);
	Region one(1);
	Regions.push_back(one);
	Region two(2);
	Regions.push_back(two);
	Region three(3);
	Regions.push_back(three);
	Region four(4);
	Regions.push_back(four);
}

Intersection::~Intersection() {
	//  Auto-generated destructor stub
}
///Wrapper to write node to PLOG, since it is a macro and cannot be one-lined inside the .print()
void PrintNodeToLog(PluginLogLevel level, xpath_node n)
{
	stringstream ss;
	n.node().print(ss);
	PLOG_I(level) << ss.str();
}

/**
 * Iterate over Map message and pull out the lanes of interest by region for the intersection.
 * Only loads if newer
 */
bool Intersection::LoadMap(MapDataMessage &msg) {
//	bool Intersection::LoadMap(ParsedMap& parsedMap) { //MapDataMessage &msg) {
	//Get version from the map message and compare to the value we've stored.  Only parse and update if the
	//map has been updated.
	int thisRev = msg.get<int>("MapData.msgIssueRevision", -1);	// Note the default value is -1 if nothing exists in the tree for that path
	if (thisRev != _mapVersion) {
		_mapVersion = thisRev;

		_intersectionId = msg.get<int>("MapData.intersections.IntersectionGeometry.id.id", -1);


		//Iterate over all lanes and get their id's.
		//Use the ids to map them over to the correct regions/parts. (OR could use the type specified in the file?)
		WGS84Point _mapReferencePoint = GetMapRReferencePoint(msg);
		//Set the variable with the Reference point
		//Reference point in file has no decimal point so function puts it back by dividing by 10000000
		Map.ReferencePoint.Latitude = _mapReferencePoint.Latitude;
		Map.ReferencePoint.Longitude = _mapReferencePoint.Longitude;
		InitMapBoundaryBox();
		//Function iterates over the lane set
		Map.Lanes = GetMapLanes(msg);
		//At this point only the vehicle and pedestrian lanes have nodes set. Must set the computed lane nodes
		SetComputedLanes(Map.Lanes);
		//Update the regions with the new map
		UpdateRegions();
		return true; //We really updated the data.
	}
	return false; //The map was what we already had and did not update anything.
}
int Intersection::GetIntersectionId()
{
	return _intersectionId;
}

bool Intersection::IsPedestrianReportedForLane(int laneId, SpatMessage &msg) {
	//Query spat for existence of node: <ManeuverAssistList><ConnectionManeuverAssist><LaneConnectionID>
	//Any part of path may be absent.
	message_document md(msg); // If we wanted to remove the line feeds , we'd want to overload message_document
								// to add parsing options( parse_wconv_attribute) onto the call to load_string.
							//However, this would make reading the larger nodes when debug/dev much more difficult.

	//Verify that this spat message data matches the intersection that we have mapped out.
	//int spatInterId = msg.get<int>("SPAT.intersections.IntersectionState.id.id", -1);
	//if(spatInterId != _intersectionId) return false;

//	PrintNodeToLog(logDEBUG2,md);
//
//	xpath_node_set lanes = md.select_nodes("/SPAT/intersections");
//
//	for (xpath_node_set::const_iterator i = lanes.begin(); i != lanes.end();
//			i++) {
//		PrintNodeToLog(logDEBUG,*i);
//	}

	pugi::xpath_variable_set vars;
	vars.add("varid", pugi::xpath_type_string);
	vars.set("varid", to_string(laneId).c_str());
	//Grab parent node "ConnectionManeuverAssist" for which there is a child node called connectionID where the value is
	//the loaded varid parameter.
	xpath_node connectionManeuverAssistForId =
			md.select_node(
					"/SPAT/intersections/IntersectionState/maneuverAssistList/ConnectionManeuverAssist[connectionID=string($varid)]",
					&vars);

	if (!connectionManeuverAssistForId.node().empty()) {
		PrintNodeToLog(logDEBUG,connectionManeuverAssistForId);
		//Now ensure that the pedestrian detection is really set for true for this Id.
		//It is not the value, it is a tag name.
		//<pedBicycleDetect>
		//<true/>
		//</pedBicycleDetect>

		string pedestrianBicycleDetect =
				connectionManeuverAssistForId.node().child("pedBicycleDetect").first_child().name();
		bool val = istringstream(pedestrianBicycleDetect);	//convert to bool
		if (val) {
			return true;

		}
	}
	//If the connectionID for that lane does not exist, it is false.
	return false;

}
bool Intersection::DoesSpatMatchMap(tmx::messages::SpatMessage &msg) {
	//Verify that this spat message data matches the intersection that we have mapped out.
	int spatInterId = msg.get<int>("SPAT.intersections.IntersectionState.id.id", -1);
	if(spatInterId == _intersectionId) return true;
	else return false;
}
bool Intersection::IsSignalForGroupRedLight(int signalGroup,
		tmx::messages::SpatMessage &msg, string &relevantFields) {
	relevantFields = "";
	//A true red light is spat field "stop-And-Remain".
	//In all other cases, we want to default to Green/NotRed.  This is because callers of
	//this function typically suppress on Red and want visibility for all other cases.
	bool isRed = false;
	message_document md(msg);
	//PrintNodeToLog(logDEBUG2,md);

	//Verify that this spat message data matches the intersection that we have mapped out.
	//int spatInterId = msg.get<int>("SPAT.intersections.IntersectionState.id.id", -1);
	//if(spatInterId != _intersectionId) return false;

	//Find Movement State For Signal Group.  Ex:
	// <MovementState>
	//<signalGroup>4</signalGroup>
	// <state-time-speed>
	//    <MovementEvent>
	//        <eventState><stop-And-Remain/></eventState>

	pugi::xpath_variable_set vars;
	vars.add("sigid", pugi::xpath_type_string);
	vars.set("sigid", to_string(signalGroup).c_str());
	//Grab parent node "MovementState" for which there is a child node called signalGroup where the value is
	//the loaded sigid parameter.
	xpath_node moveStateForSignalGroup =
			md.select_node(
					"/SPAT/intersections/IntersectionState/states/MovementState[signalGroup=string($sigid)]",
					&vars);



	if (moveStateForSignalGroup.node().empty()) {
		return false;	//We don't know, so we don't say its Red.
	}

	PrintNodeToLog(logDEBUG,moveStateForSignalGroup);
//	xpath_node eventState = moveStateForSignalGroup.node().select_node(
//			"/MovementState/state-time-speed/MovementEvent/eventState");//this finds nothing, don't know why.
	xpath_node eventState = moveStateForSignalGroup.node().child(
			"state-time-speed").child("MovementEvent").child("eventState");
	PrintNodeToLog(logDEBUG,eventState);
	stringstream ss; //save both the signal group and the event state to relevantFields to return to caller.
	moveStateForSignalGroup.node().child("signalGroup").print(ss);
	eventState.node().print(ss);
	relevantFields = ss.str();


	//In case node does not have a name or value or if the node handle is null, both functions return empty strings - they never return null pointers.
	string nodeName = eventState.node().first_child().name();

	//PLOG_I(logINFO) << eventState.node().first_child().name();

	if (nodeName == "stop-And-Remain") {
		isRed = true;
	}

	return isRed;
}

///This outputs all the points of the map into a csv format that can be utilized by a utility to
///load to Google Earth.
///lat,long,type,laneNumber, region
///lat,long are decimal. laneNumber and region are integer, and type is a character:
string Intersection::MapToString() {
	std::string s;

	//Write out reference point.
	s += to_string(Map.ReferencePoint.Latitude) + ","
			+ to_string(Map.ReferencePoint.Longitude) + "," + "r,"//type reference
			+ to_string(0) + ","	//lanenumber
			+ to_string(0)	//region
			+ "\r\n";

	list<MapLane>::iterator i;
	list<LaneNode>::iterator j;
	for (i = Map.Lanes.begin(); i != Map.Lanes.end(); ++i) {
		//if (!i->_laneDirectionEgress)
		//{
		//if (i->_directionalUse != 3)
		//{
		//Figure out type and region
		//vehicle=v,//vehicle lane
		//sidewalk=s,//curb
		//cross=c,//crosswalk

		int region = -1;
		string type = "o";

		region = GetRegionForVehicleLaneId(i->LaneNumber);
		if (region != -1) {
			//found type and region
			type = "v";		//vehicle
		} else {		//didin't find it in vehicles , keep looking
			region = GetRegionForCrosswalkLaneId(i->LaneNumber);
			if (region != -1) {
				//found type and region
				type = "c";		//crosswalk
			} else {//didin't find it in vehicles or crosswalks , keep looking
				region = GetRegionForCurbLaneId(i->LaneNumber);
				if (region != -1) {
					//found type and region
					type = "s";		//sidewalk
				}

			}
		}

		for (j = i->Nodes.begin(); j != i->Nodes.end(); ++j) {
//			if ((i->LaneNumber == 11) || (i->LaneNumber == 12)
//					|| (i->LaneNumber == 13) || (i->LaneNumber == 21)
//					|| (i->LaneNumber == 22) || (i->LaneNumber == 23)
//					|| (i->LaneNumber == 31) || (i->LaneNumber == 32)
//					|| (i->LaneNumber == 33) || (i->LaneNumber == 41)
//					|| (i->LaneNumber == 43) || (i->LaneNumber == 42))

			//Output in csv Lat,Long,Type,LaneNumber,Region
			s += to_string(j->Point.Latitude) + ","
					+ to_string(j->Point.Longitude) + "," + type + ","	//type
					+ to_string(i->LaneNumber) + ","		//lanenumber
					+ to_string(region)		//region
					+ "\r\n";
			//	}
			//}
		}
	}
	return s;
}

void Intersection::UpdateRegions() {
	//Clear out all previous stored region info.
	for (std::vector<Region>::iterator it = Regions.begin();
			it != Regions.end(); ++it) {
		it->VehicleLanes.clear();
		it->Crosswalks.clear();
		it->Curbs.clear();
	}

	//Iterate over all vehicle lanes in the region.
	list<MapLane>::iterator i;
	for (i = Map.Lanes.begin(); i != Map.Lanes.end(); ++i) {
		//Add each lane to the right  lists for each region.
		UpdateRegionForLaneId(i->LaneNumber);

	}

}
//return list of ints
list<int> Intersection::GetCurbLaneIdsForRegion(int region) const {

	return Regions[region].Curbs;

}
list<int> Intersection::GetCrosswalkLaneIdsForRegion(int region) const {
	return Regions[region].Crosswalks;

}

/**
 * Intersections are mapped to 4 regions assigned clockwise to the ways to enter the intersection.
 * It does not matter which entry is assigned to 1 as long as they are clockwise.
 *          |  2  |
 *          |     |
 * -------------------------
 *    1     |     |     3
 * -------------------------
 *          |  4  |
 *          |     |
 *
 * If a roadway is not a standard fourway, the region numbers are still assigned (in the MAP) as if it were a standard
 * fourway:
 *          |  2  |
 *          |     |
 * ----------------
 *    1     |     |
 * ----------------
 *          |  4  |
 *          |     |
 *
 *  In this example, Region 3 will just be empty/not populated.
 *
 * -------------------------
 *    1     |     |     3
 * -------------------------
 *
 *  In this example, Region 2,4 will just be empty/not populated.
 */
int Intersection::FindEgressRegion(int approachRegion,
		Direction direction) const {
	int egressRegion = -1;
	//Regions are assigned clockwise and always 'as if' it were a normal four-way intersection even if it is not.
	if (direction == Direction::Dir_Right) {
		egressRegion = approachRegion - 1;
	} else if (direction == Direction::Dir_Straight) {
		egressRegion = approachRegion - 2;
	} else if (direction == Direction::Dir_Left) {
		egressRegion = approachRegion + 1;
	}
	//Handle boundary/loop conditions, 1 thru 4 map to 4 sides of the intersection so after 4 loops around to 1 again.
	if (egressRegion > 4) {
		egressRegion = egressRegion - 4; //e.g. a 5 really is a region 1.
	}
	if (egressRegion < 1) {
		egressRegion = egressRegion + 4; //e.g. a 0 really is a region 4.
	}

	return egressRegion;
}

int Intersection::CheckRegionForVehicleLaneId(const Region& r,
		int laneId) const {
	list<int>::const_iterator v;
	//Iterate over all vehicle lanes in the region.
	for (v = r.VehicleLanes.begin(); v != r.VehicleLanes.end(); ++v) {
		if (laneId == *v) {
			//Return region number if found in region.
			return r.RegionId;
		}
	}
	return -1; //Return -1 if not found in region.
}
int Intersection::CheckRegionForCurbLaneId(const Region& r, int laneId) const {
	list<int>::const_iterator v;
	//Iterate over all curb/sidewalk lanes in the region.
	for (v = r.Curbs.begin(); v != r.Curbs.end(); ++v) {
		if (laneId == *v) {
			//Return region number if found in region.
			return r.RegionId;
		}
	}
	return -1; //Return -1 if not found in region.
}
int Intersection::CheckRegionForCrosswalkLaneId(const Region& r,
		int laneId) const {
	list<int>::const_iterator v;
	//Iterate over all Crosswalk lanes in the region.
	for (v = r.Crosswalks.begin(); v != r.Crosswalks.end(); ++v) {
		if (laneId == *v) {
			//Return region number if found in region.
			return r.RegionId;
		}
	}
	return -1; //Return -1 if not found in region.
}
int Intersection::GetRegionForCrosswalkLaneId(int laneId) const {

	//Check each region for the lane number. Return the region if it is found.

	for (std::vector<Region>::const_iterator it = Regions.begin() + 1;
			it != Regions.end(); ++it) {
		int reg = CheckRegionForCrosswalkLaneId(*it, laneId);
		if (reg != -1) {
			return reg; //found lane in this region. return region number.
		}
	}
	return -1;
}
int Intersection::GetRegionForCurbLaneId(int laneId) const {

	//Check each region for the lane number. Return the region if it is found.

	for (std::vector<Region>::const_iterator it = Regions.begin() + 1;
			it != Regions.end(); ++it) {
		int reg = CheckRegionForCurbLaneId(*it, laneId);
		if (reg != -1) {
			return reg; //found lane in this region. return region number.
		}
	}
	return -1;
}
int Intersection::GetRegionForVehicleLaneId(int laneId) const {

	//Check each region for the lane number. Return the region if it is found.

	for (std::vector<Region>::const_iterator it = Regions.begin() + 1;
			it != Regions.end(); ++it) {
		int reg = CheckRegionForVehicleLaneId(*it, laneId);
		if (reg != -1) {
			return reg; //found lane in this region. return region number.
		}
	}

	return -1;
}
int Intersection::UpdateRegionForLaneId(int laneId) {
	//Load vehicle lanes to the respective region.
	if (laneId >= 10 && laneId <= 19) {
		Regions[1].VehicleLanes.push_back(laneId);
		return 1;
	} else if (laneId >= 20 && laneId <= 29) {
		Regions[2].VehicleLanes.push_back(laneId);
		return 2;
	} else if (laneId >= 30 && laneId <= 39) {
		Regions[3].VehicleLanes.push_back(laneId);
		return 3;
	} else if (laneId >= 40 && laneId <= 49) {
		Regions[4].VehicleLanes.push_back(laneId);
		return 4;
	}
	//Handle actual crosswalks, region number matches lane Id.
	//Load crosswalks to the respective region.
	else if (laneId == 1) {
		Regions[1].Crosswalks.push_back(laneId);
		return 1;
	} else if (laneId == 2) {
		Regions[2].Crosswalks.push_back(laneId);
		return 2;
	} else if (laneId == 3) {
		Regions[3].Crosswalks.push_back(laneId);
		return 3;
	} else if (laneId == 4) {
		Regions[4].Crosswalks.push_back(laneId);
		return 4;
	}

	//Load curb/sidewalk to the respective region.
	//These zones are in the 100's and it is the tens place that maps to the region.
	if (laneId >= 101 && laneId < 200) {

		int zoneRem = laneId - 100;
		if (zoneRem >= 10 && zoneRem <= 19) {
			Regions[1].Curbs.push_back(laneId);
			return 1;
		} else if (zoneRem >= 20 && zoneRem <= 29) {
			Regions[2].Curbs.push_back(laneId);
			return 2;
		} else if (zoneRem >= 30 && zoneRem <= 39) {
			Regions[3].Curbs.push_back(laneId);
			return 3;
		} else if (zoneRem >= 40 && zoneRem <= 49) {
			Regions[4].Curbs.push_back(laneId);
			return 4;
		}

	}

	return -1;

}

/**
 * Gets the revision element from the MapData message
 */
int Intersection::GetMapRevision(tmx::messages::MapDataMessage &msg) {
	message_document md(msg);

	xpath_node version = md.select_node(
			"/MapData/msgIssueRevision");

	return version.node().text().as_int();
}
/**
 * Gets the reference point element from the MapData message
 */
WGS84Point Intersection::GetMapRReferencePoint(tmx::messages::MapDataMessage &msg) {
	message_document md(msg);

	xpath_node IntersectionReferencePoint = md.select_node(
			"/MapData/intersections/IntersectionGeometry/refPoint");

	//Get the ParsedMap data from the child objects
	double latitude = IntersectionReferencePoint.node().child("lat").text().as_double();
	double longitude = IntersectionReferencePoint.node().child("long").text().as_double();

	WGS84Point referencePoint = {latitude / 10000000.0, longitude / 10000000.0};

	return referencePoint;
}
/**
 * Gets the all of the lane data from the MapData message
 */
std::list<MapLane> Intersection::GetMapLanes(tmx::messages::MapDataMessage &msg) {

	//http://pugixml.org/docs/quickstart.html
	message_document md(msg);

	//PrintNodeToLog(logDEBUG,md);
	//Get the Intersection Geometry node and use child and select_nodes to get everything else we need
	xpath_node IntersectionGeometry = md.select_node(
			"/MapData/intersections/IntersectionGeometry");

	int laneWidth =
			IntersectionGeometry.node().child("laneWidth").text().as_int();

	std::list<MapLane> lanes;
	MapLane mapLane;

	//Set the lanes
	for (pugi::xml_node iterator : IntersectionGeometry.node().child(
			"laneSet").children("GenericLane")) {

		std::string directionalUseStr =
				iterator.child("laneAttributes").child("directionalUse").text().as_string();
		bool laneDirectionEgress =
				(directionalUseStr == "01") ? true : false;
		int laneID = iterator.child("laneID").text().as_int();
		//int laneWidth =iterator.child("laneWidth").text().as_int();//BUG - there is no lanewidth per lane.only the one at the root.
		int signalGroup;

		for (pugi::xml_node connectionsIterator : iterator.child(
				"connectsTo").children("Connection")) {
			signalGroup =
					connectionsIterator.child("signalGroup").text().as_int();
			//may need to capture all signal groups if a lane can have more than one
			break;
		}

		mapLane.LaneNumber = laneID;
		mapLane.LaneWidthMeters = (double) laneWidth / 100; //TODO auto detect conversion factor? is this always fixed or dynamic like offsets?
		mapLane.LaneDirectionEgress = laneDirectionEgress;
		mapLane.SignalGroupId = signalGroup;
		mapLane.Direction = GetDirectionalUse(directionalUseStr);
		mapLane.ReferenceLaneId = GetReferenceLaneId(iterator,
				mapLane.Direction);
		mapLane.Type = GetLaneType(iterator, mapLane.Direction);
		mapLane.LaneNodeOffset = GetComputedLaneOffset(iterator);

		double totalXOffset = 0.0;
		double totalYOffset = 0.0;
		mapLane.Nodes = GetLaneNodes(iterator, totalXOffset, totalYOffset);

		lanes.push_back(mapLane);
	}

	return lanes;
}
/**
 * Gets a lane data from the provided lane id from the MapData message
 */
MapLane Intersection::GetMapLane(tmx::messages::MapDataMessage &msg, int laneId) {

	//http://pugixml.org/docs/quickstart.html
	message_document md(msg);

	//Get the Intersection Geometry node and use child and select_nodes to get everything else we need
	xpath_node IntersectionGeometry = md.select_node(
			"/MapData/intersections/IntersectionGeometry");

	int laneWidth =
			IntersectionGeometry.node().child("laneWidth").text().as_int();

	pugi::xpath_variable_set vars;
	vars.add("landeid", pugi::xpath_type_string);
	vars.set("landeid", to_string(laneId).c_str());
	//Get the Intersection Geometry node and use child and select_nodes to get everything else we need
	IntersectionGeometry = md.select_node(
			"/MapData/intersections/IntersectionGeometry/laneSet/GenericLane[laneID=string($landeid)]",
			&vars);

	std::string directionalUseStr =
			IntersectionGeometry.node().child("laneAttributes").child("directionalUse").text().as_string();
	bool laneDirectionEgress =
			(directionalUseStr == "01") ? true : false;
	int laneID = IntersectionGeometry.node().child("laneID").text().as_int();
	int signalGroup;

	for (pugi::xml_node connectionsIterator : IntersectionGeometry.node().child(
			"connectsTo").children("Connection")) {
		signalGroup =
				connectionsIterator.child("signalGroup").text().as_int();
		//may need to capture all signal groups if a lane can have more than one
		break;
	}

	MapLane _mapLane;

	_mapLane.LaneNumber = laneID;
	_mapLane.LaneWidthMeters = (double) laneWidth / 100; //TODO auto detect conversion factor? is this always fixed or dynamic like offsets?
	_mapLane.LaneDirectionEgress = laneDirectionEgress;
	_mapLane.SignalGroupId = signalGroup;
	_mapLane.Direction = GetDirectionalUse(directionalUseStr);
	_mapLane.ReferenceLaneId = GetReferenceLaneId(IntersectionGeometry,
			_mapLane.Direction);
	_mapLane.Type = GetLaneType(IntersectionGeometry, _mapLane.Direction);
	_mapLane.LaneNodeOffset = GetComputedLaneOffset(IntersectionGeometry);

	double totalXOffset = 0.0;
	double totalYOffset = 0.0;
	_mapLane.Nodes = GetLaneNodes(IntersectionGeometry, totalXOffset, totalYOffset);

	return _mapLane;
}
/**
 * Gets the all of the lane nodes for a give lane id from the MapData message
 */
std::list<LaneNode> Intersection::GetLaneNodes(tmx::messages::MapDataMessage &msg, int laneId, double totalXOffset, double totalYOffset) {

	//http://pugixml.org/docs/quickstart.html
	message_document md(msg);

	pugi::xpath_variable_set vars;
	vars.add("landeid", pugi::xpath_type_string);
	vars.set("landeid", to_string(laneId).c_str());
	//Get the Intersection Geometry node and use child and select_nodes to get everything else we need
	xpath_node IntersectionGeometry = md.select_node(
			"/MapData/intersections/IntersectionGeometry/laneSet/GenericLane[laneID=string($landeid)]",
			&vars);

	std::list<LaneNode> laneNodes;

	for (pugi::xml_node nodesIterator : IntersectionGeometry.node().child("nodeList").child("nodes").children(
			"Node")) {
		int x = nodesIterator.select_node("delta//x").node().text().as_int();
		int y = nodesIterator.select_node("delta//y").node().text().as_int();
		LaneNode laneNode;

		laneNode.Point = GetPointFromNode(Map.ReferencePoint, x, y,
				totalXOffset, totalYOffset);
		laneNodes.push_back(laneNode);
	}

	return laneNodes;
}

/**
 * Get the offset values for the computed lanes
 */
WGS84Point Intersection::GetComputedLaneOffset(pugi::xml_node &node) {
	WGS84Point laneNodeOffset;

	laneNodeOffset.Latitude = node.child("nodeList").child("computed").child(
			"offsetXaxis").child("large").text().as_int();
	laneNodeOffset.Longitude = node.child("nodeList").child("computed").child(
			"offsetYaxis").child("large").text().as_int();

	return laneNodeOffset;
}

/**
 * Get the offset values for the computed lanes
 */
WGS84Point Intersection::GetComputedLaneOffset(pugi::xpath_node &laneNode) {
	WGS84Point laneNodeOffset;

	laneNodeOffset.Latitude = laneNode.node().child("nodeList").child("computed").child(
			"offsetXaxis").child("large").text().as_int();
	laneNodeOffset.Longitude = laneNode.node().child("nodeList").child("computed").child(
			"offsetYaxis").child("large").text().as_int();

	return laneNodeOffset;
}

/**
 * Determines which lanes are computed and then compares the reference lane id of the computed lane to the vehicle
 * lane with the nodes already computed and assgins the nodes with the given computed offset to the computed lane node list
 */
void Intersection::SetComputedLanes(std::list<MapLane> &lanes) {
	std::list<MapLane> ComputedLanes;

	for (std::list<MapLane>::iterator it = lanes.begin(); it != lanes.end();
			++it) {
		if (it->ReferenceLaneId > 0
				&& (it->Type == Computed || it->Type == Egress)) {
			ComputedLanes.push_back(*it);
		}
	}

	for (std::list<MapLane>::iterator i = ComputedLanes.begin();
			i != ComputedLanes.end(); ++i) {
		for (std::list<MapLane>::iterator j = lanes.begin(); j != lanes.end();
				++j) {
			if (i->ReferenceLaneId == j->LaneNumber) {
				i->Nodes = GetOffsetNodeList(j->Nodes, i->LaneNodeOffset);
				break;
			}
		}

		for (std::list<MapLane>::iterator j = lanes.begin(); j != lanes.end();
				++j) {
			if (i->LaneNumber == j->LaneNumber) {
				j->Nodes = i->Nodes;
				break;
			}
		}
	}
}

/**
 * Get the offset values for the computed lanes nodes
 */
std::list<LaneNode> Intersection::GetOffsetNodeList(std::list<LaneNode> &nodes,
		WGS84Point &laneNodeOffset) {
	std::list<LaneNode> computedNodes;

	for (std::list<LaneNode>::iterator j = nodes.begin(); j != nodes.end();
			++j) {
		LaneNode laneNode;
		laneNode.Point = GetPointFromNodeOffset(j->Point, laneNodeOffset);
		computedNodes.push_back(laneNode);
	}

	return computedNodes;
}

/**
 * Get the reference lane id from the computed node for compairison to the vehicle lane
 */
int Intersection::GetReferenceLaneId(pugi::xml_node &node,
		DirectionalUse &directionalUse) {
	if (directionalUse == Ingress_Vehicle_Computed
			|| directionalUse == Egress_Computed) {
		return node.child("nodeList").child("computed").child("referenceLaneId").text().as_int();
	}

	return -1;
}
/**
 * Get the reference lane id from the computed node for compairison to the vehicle lane
 */
int Intersection::GetReferenceLaneId(pugi::xpath_node &laneNode,
		DirectionalUse &directionalUse) {
	if (directionalUse == Ingress_Vehicle_Computed
			|| directionalUse == Egress_Computed) {
		return laneNode.node().child("nodeList").child("computed").child("referenceLaneId").text().as_int();
	}

	return -1;
}
/**
 * Get all of the nodes for a lane. Note will not get the computed lane nodes, they will be empty and set later.
 */
std::list<LaneNode> Intersection::GetLaneNodes(pugi::xml_node &node,
		double &totalXOffset, double &totalYOffset) {
	std::list<LaneNode> laneNodes;

	for (pugi::xml_node nodesIterator : node.child("nodeList").child("nodes").children(
			"Node")) {
		int x = nodesIterator.select_node("delta//x").node().text().as_int();
		int y = nodesIterator.select_node("delta//y").node().text().as_int();
		LaneNode laneNode;

		laneNode.Point = GetPointFromNode(Map.ReferencePoint, x, y,
				totalXOffset, totalYOffset);
		laneNodes.push_back(laneNode);

		UpdateMapBoundaryBox(laneNode.Point);

	}

	return laneNodes;
}

void Intersection::InitMapBoundaryBox()
{
	 Map.MaxLat=Map.ReferencePoint.Latitude;
	 Map.MinLat=Map.ReferencePoint.Latitude;
	 Map.MaxLong=Map.ReferencePoint.Longitude;
	 Map.MinLong=Map.ReferencePoint.Longitude;
}

void Intersection::UpdateMapBoundaryBox(WGS84Point point)
{
	//Build a bounding box for the entire MAP as we go. If this point is a new extreme in any dir, save it.
		if(point.Latitude > Map.MaxLat)Map.MaxLat = point.Latitude;
		else if(point.Latitude < Map.MinLat)Map.MinLat = point.Latitude;
		if(point.Longitude > Map.MaxLong)Map.MaxLong = point.Longitude;
		else if(point.Longitude < Map.MinLong)Map.MinLong = point.Longitude;
}
/**
 * Get all of the nodes for a lane. Note will not get the computed lane nodes, they will be empty and set later.
 */
std::list<LaneNode> Intersection::GetLaneNodes(pugi::xpath_node &laneNode,
		double &totalXOffset, double &totalYOffset) {
	std::list<LaneNode> laneNodes;

	for (pugi::xml_node nodesIterator : laneNode.node().child("nodeList").child("nodes").children(
			"Node")) {
		int x = nodesIterator.select_node("delta//x").node().text().as_int();
		int y = nodesIterator.select_node("delta//y").node().text().as_int();
		LaneNode laneNode;

		laneNode.Point = GetPointFromNode(Map.ReferencePoint, x, y,
				totalXOffset, totalYOffset);
		laneNodes.push_back(laneNode);
	}

	return laneNodes;
}
/**
 * Gets the nodes from the initial reference point then adds the offsets x and y
 */
WGS84Point Intersection::GetPointFromNode(WGS84Point &referencePoint,
		int &latval, int &longval, double& totalXOffset, double& totalYOffset) {
	WGS84Point nodeOffsetPoint;

	double xOffset;
	double yOffset;

	double scaleOffset = 0.01; //GetScaleFactor(); //TODO this should be .1 for decimeters but we seem to need .01 for our twelve mile file?
	xOffset = latval * scaleOffset;
	yOffset = longval * scaleOffset;

	totalXOffset += xOffset;
	totalYOffset += yOffset;

	double absLatitude = Conversions::NodeOffsetToLatitude(
			referencePoint.Latitude, totalYOffset);
	double absLongitude = Conversions::NodeOffsetToLongitude(
			referencePoint.Longitude, referencePoint.Latitude, totalXOffset);

	nodeOffsetPoint.Latitude = absLatitude;
	nodeOffsetPoint.Longitude = absLongitude;

	return nodeOffsetPoint;
}

/**
 * Get the nodes of the computed lanes based on the reference node of the vehicle lane and the computed lane offsets
 */
WGS84Point Intersection::GetPointFromNodeOffset(WGS84Point &referencePoint,
		WGS84Point &laneNodeOffset) {
	WGS84Point nodeOffsetPoint;

	double xOffset;
	double yOffset;

	double scaleOffset = 0.01; //GetScaleFactor(); //TODO this should be .1 for decimeters but we seem to need .01 for our twelve mile file?
	xOffset = laneNodeOffset.Latitude * scaleOffset;
	yOffset = laneNodeOffset.Longitude * scaleOffset;

	double absLatitude = Conversions::NodeOffsetToLatitude(
			referencePoint.Latitude, yOffset);
	double absLongitude = Conversions::NodeOffsetToLongitude(
			referencePoint.Longitude, referencePoint.Latitude, xOffset);

	nodeOffsetPoint.Latitude = absLatitude;
	nodeOffsetPoint.Longitude = absLongitude;

	return nodeOffsetPoint;
}

/**
 * Gets the directional use for the MapLane object
 */
DirectionalUse Intersection::GetDirectionalUse(std::string &directionalUse) {
	DirectionalUse directionalUseEnum;

	int directUseInt = std::stoi(directionalUse, nullptr, 2);

	if (directUseInt == 2)
		directionalUseEnum = Ingress_Vehicle_Computed;
	else if (directUseInt == 3)
		directionalUseEnum = Ingress_Pedestrian;
	else if (directUseInt == 1)
		directionalUseEnum = Egress_Computed;

	return directionalUseEnum;
}

/**
 * Gets the lane type based on the directional use for the MapLane object
 */
LaneType Intersection::GetLaneType(pugi::xml_node &node,
		DirectionalUse &directionalUse) {
	LaneType laneTypeEnum;

	if (!node.child("nodeList").child("nodes").empty()
			&& directionalUse == Ingress_Vehicle_Computed) {
		laneTypeEnum = Vehicle;
	} else if (!node.child("nodeList").child("computed").empty()
			&& directionalUse == Ingress_Vehicle_Computed) {
		laneTypeEnum = Computed;
	} else if (directionalUse == Ingress_Pedestrian) {
		laneTypeEnum = Pedestrian;
	} else if (directionalUse == Egress_Computed) {
		laneTypeEnum = Egress;
	}

	//TODO:Need to do side walk

	return laneTypeEnum;
}
/**
 * Gets the lane type based on the directional use for the MapLane object
 */
LaneType Intersection::GetLaneType(pugi::xpath_node &laneNode,
		DirectionalUse &directionalUse) {
	LaneType laneTypeEnum;

	if (!laneNode.node().child("nodeList").child("nodes").empty()
			&& directionalUse == Ingress_Vehicle_Computed) {
		laneTypeEnum = Vehicle;
	} else if (!laneNode.node().child("nodeList").child("computed").empty()
			&& directionalUse == Ingress_Vehicle_Computed) {
		laneTypeEnum = Computed;
	} else if (directionalUse == Ingress_Pedestrian) {
		laneTypeEnum = Pedestrian;
	} else if (directionalUse == Egress_Computed) {
		laneTypeEnum = Egress;
	}

	//TODO:Need to do side walk

	return laneTypeEnum;
}

}
} // namespace tmx::utils

