/*
 * Roadway.cpp
 *
 *  Created on: Apr 29, 2016
 *      Author: ivp
 */

#include "Roadway.h"
#include "Intersection.h"
#include "ParsedMap.h"
#include "GeoVector.h"
#include "WGS84Polygon.h"
#include "PluginLog.h"


#include <tmx/messages/message_document.hpp>
#include <tmx/pugixml/pugixml.hpp>
using namespace std;
using namespace tmx;
using namespace tmx::messages;
using namespace pugi;

namespace tmx {
namespace utils {

Roadway::Roadway() :
		_mapVersion(-1), _mapId(-1){


//RoadSections
}

Roadway::~Roadway() {
	//  Auto-generated destructor stub
}

/**
 * Some helper functions, defined in a different source file
 */
RoadSegmentList *FindRoadSegments(MapDataMessage &msg);
WGS84Point GetMapReferencePoint(IntersectionGeometryList *intersections, RoadSegmentList *roadSegments);
void FillMapLaneData(MapLane &mapLane, GenericLane *lane, int laneWidth);
std::list<LaneNode> ComputeLaneNodes(WGS84Point refPoint, int laneId, GenericLane **lanes, int laneCount, double totalXOffset, double totalYOffset);
WGS84Point GetPointFromNodeOffset(WGS84Point &referencePoint, WGS84Point &laneNodeOffset);

/**
 * Iterate over Map message and pull out the lanes of interest by region for the roadway.
 * Only loads if newer
 */
bool Roadway::LoadMap(MapDataMessage &msg) {
//	bool Roadway::LoadMap(ParsedMap& parsedMap) { //MapDataMessage &msg) {
	//Get version from the map message and compare to the value we've stored.  Only parse and update if the
	//map has been updated.
	auto segs = FindRoadSegments(msg);
	if (!segs)
		return false;

	int thisId = segs->list.array[0]->id.id;
	int thisRev = msg.get_j2735_data()->msgIssueRevision;

	if (thisRev != _mapVersion) {
		_mapVersion = thisRev;
		_mapId = thisId;

		//Iterate over all lanes and get their id's.
		//Use the ids to map them over to the correct regions/parts. (OR could use the type specified in the file?)
		WGS84Point _mapReferencePoint = GetMapReferencePoint(0, segs);
		//Set the variable with the Reference point
		//Reference point in file has no decimal point so function puts it back by dividing by 10000000
		Map.ReferencePoint.Latitude = _mapReferencePoint.Latitude;
		Map.ReferencePoint.Longitude = _mapReferencePoint.Longitude;
		InitMapBoundaryBox();
		//Function iterates over the lane set
		Map.Lanes = GetMapLanes(msg);
		//At this point only the vehicle and pedestrian lanes have nodes set. Must set the computed lane nodes
		SetComputedLanes(Map.Lanes);
		//Update transit stop danger zones for new map
		UpdateDangerZonesForMap();
		return true; //We really updated the data.
	}
	return false; //The map was what we already had and did not update anything.
}
void Roadway::UpdateDangerZonesForMap(){

	//Iterate over all  lanes in the map to find the lanes (lane numbers) that correspond to special 'danger' sections of the map.
	list<MapLane>::iterator i;
	for ( i = Map.Lanes.begin(); i != Map.Lanes.end(); ++i) {
		if (i->LaneNumber==LandingLane) {
			//Take the two nodes defining the forward Curb Zone and pass to function to chop into quadrants.
			LoadLandingLaneQuadrants(i->Nodes.front().Point,i->Nodes.back().Point,i->LaneWidthMeters);
		}
		else if(i->LaneNumber==SidewalkLane){
			//Take the two nodes defining the sidewalk danger zone and add to quadrants.
			LoadSidewalkLaneQuadrant(i->Nodes.front().Point,i->Nodes.back().Point,i->LaneWidthMeters);
		}
	}

}
int Roadway::GetMapId()
{
	return _mapId;
}

TransitStopQuadrant Roadway::FindQuadrantForPoint(WGS84Point& point)
{
	WGS84Polygon poly;
	int qName = WaitingZone;
	while (qName <= RearCenterZone)
	{
		Quadrant q = DangerZones[qName];
		//Corners of polygon must be submitted in contiguous order, so 4 goes before 3.
		if (poly.IsPointInsidePoly(point, q.p1, q.p2, q.p4, q.p3))
			return (TransitStopQuadrant)qName;
		else
			qName++;
	}
	return NoQuadrant;
}

bool Roadway::IsPedestrianReportedForLane(int laneId, SpatMessage &msg) {

	message_document md(msg);

	//toDO

	//If the connectionID for that lane does not exist, it is false.
	return false;

}

///This outputs all the points of the map into a csv format that can be utilized by a utility to
///load to Google Earth.
/// todo
///lat,long are decimal. laneNumber and region are integer, and type is a character:
string Roadway::MapToString() {
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
		for (j = i->Nodes.begin(); j != i->Nodes.end(); ++j) {

				//Output in csv Lat,Long,Type,LaneNumber,0
				s += to_string(j->Point.Latitude) + ","
						+ to_string(j->Point.Longitude) + ",o" + ","	//type not used. leave as o.
						+ to_string(i->LaneNumber) 		//lanenumber
						+ ",0\r\n"; //utility for processing these expects one more column that we don't use here: leave as 0.

			}
	}
	int ind=0;
	for(auto q = DangerZones.begin(); q!= DangerZones.end(); ++q){
		s += to_string(q->p1.Latitude) + ","
				+ to_string(q->p1.Longitude) + ",s" + ","	//load zone boundaries as 's' icon
				+ to_string(ind) 		//put danger zone index in for lane number
				+ ",1\r\n";
		s += to_string(q->p2.Latitude) + ","
				+ to_string(q->p2.Longitude) + ",s" + ","	//load zone boundaries as 's' icon
				+ to_string(ind) 		//put danger zone index in for lane number
				+ ",2\r\n";
		s += to_string(q->p3.Latitude) + ","
				+ to_string(q->p3.Longitude) + ",s" + ","	//load zone boundaries as 's' icon
				+ to_string(ind) 		//put danger zone index in for lane number
				+ ",3\r\n";
		s += to_string(q->p4.Latitude) + ","
				+ to_string(q->p4.Longitude) + ",s" + ","	//load zone boundaries as 's' icon
				+ to_string(ind) 		//put danger zone index in for lane number
				+ ",4\r\n";

	ind++;
	}
	return s;
}

/**
 * Gets a lane data from the provided lane id from the MapData message
 */
MapLane Roadway::GetMapLane(tmx::messages::MapDataMessage &msg, int laneId)
{
	MapLane mapLane;

	auto segs = FindRoadSegments(msg);
	if (!segs)
		return mapLane;

	int laneWidth = 0;
	if (segs->list.array[0]->laneWidth)
		laneWidth = *(segs->list.array[0]->laneWidth);

	//Set the lanes
	for (int i = 0; i < segs->list.array[0]->roadLaneSet.list.count; i++) {

		GenericLane *lane = segs->list.array[0]->roadLaneSet.list.array[i];
		if (lane->laneID != laneId)
			continue;

		FillMapLaneData(mapLane, lane, laneWidth);

		double totalXOffset = 0.0;
		double totalYOffset = 0.0;
		mapLane.Nodes = GetLaneNodes(msg, laneId, totalXOffset, totalYOffset);
	}

	return mapLane;
}

/**
 * Gets the all of the lane data from the MapData message
 */
std::list<MapLane> Roadway::GetMapLanes(tmx::messages::MapDataMessage &msg) {
	std::list<MapLane> lanes;

	auto *segs = FindRoadSegments(msg);
	if (!segs)
		return lanes;

	for (int i = 0; segs->list.count > 0 && i < segs->list.array[0]->roadLaneSet.list.count; i++)
	{
		lanes.push_back(GetMapLane(msg, segs->list.array[0]->roadLaneSet.list.array[i]->laneID));

		for (list<LaneNode>::iterator j = lanes.back().Nodes.begin(); j != lanes.back().Nodes.end(); j++)
				UpdateMapBoundaryBox(j->Point);

	}

	return lanes;
}

/**
 * Gets the all of the lane nodes for a give lane id from the MapData message
 */
std::list<LaneNode> Roadway::GetLaneNodes(tmx::messages::MapDataMessage &msg, int laneId, double totalXOffset, double totalYOffset) {

	auto segs = FindRoadSegments(msg);
	if (!segs || segs->list.count <= 0)
		return std::list<LaneNode>();

	return ComputeLaneNodes(GetMapReferencePoint(0, segs), laneId,
			segs->list.array[0]->roadLaneSet.list.array,
			segs->list.array[0]->roadLaneSet.list.count,
			totalXOffset, totalYOffset);
}

/**
 * Determines which lanes are computed and then compares the reference lane id of the computed lane to the vehicle
 * lane with the nodes already computed and assgins the nodes with the given computed offset to the computed lane node list
 */
void Roadway::SetComputedLanes(std::list<MapLane> &lanes) {
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
std::list<LaneNode> Roadway::GetOffsetNodeList(std::list<LaneNode> &nodes,
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

void Roadway::InitMapBoundaryBox()
{
	 Map.MaxLat=Map.ReferencePoint.Latitude;
	 Map.MinLat=Map.ReferencePoint.Latitude;
	 Map.MaxLong=Map.ReferencePoint.Longitude;
	 Map.MinLong=Map.ReferencePoint.Longitude;
}

void Roadway::UpdateMapBoundaryBox(WGS84Point point)
{
	//Build a bounding box for the entire MAP as we go. If this point is a new extreme in any dir, save it.
		if(point.Latitude > Map.MaxLat)Map.MaxLat = point.Latitude;
		else if(point.Latitude < Map.MinLat)Map.MinLat = point.Latitude;
		if(point.Longitude > Map.MaxLong)Map.MaxLong = point.Longitude;
		else if(point.Longitude < Map.MinLong)Map.MinLong = point.Longitude;
}

/*
 *  For a given road lane with two nodes specified with given road lane width:
 *  _____________________________________
 *
 *  X                                  X
 *  _____________________________________
 *
 *We need to chop this into 4 equal quadrants.       A   B
 *                                                  C   D
 *
 * Where each of the 4 corners of each box are as follows:
 *  _____________________________________
 *  A1               A2/B1             B2
 *  X/A3/C1       A4/C2/B3/D1       B4/D2/X
 *  C3               C4/D3             D4
 *  _____________________________________
 * */
void Roadway::LoadLandingLaneQuadrants(WGS84Point laneNode1, WGS84Point laneNode2, double laneWidth)
{
	//laneNode1 == A3 and C1
	//laneNode2 == B4 and D2
	//A1 is (1/2*lanewidth) meters perpendicular to the line between laneNode1 and laneNode2, etc

	//Add four quadrants in the order specified A,B,C,D where
	//ForwardCurbZone=A
	//RearCurbZone=B
	//ForwardCenterZone=C
	//RearCenterZone=D

	WGS84Point midpoint;
	double b1; // bearing laneNode1 to laneNode2
	double b2;  // bearing laneNode2 to laneNode1
	double bMidpoint;  // bearing midpoint to laneNode2
	double bearingToNewPoint;  // calculated bearing to new point
	WGS84Point cornerPoint;  // temporary storage for calculated corner points
	WGS84Point commonA2B1;
	WGS84Point commonC4D3;
	Quadrant q;

	//calculate midpoint and bearings
	midpoint = GeoVector::MidpointBetween(laneNode1, laneNode2);
	b1 = GeoVector::BearingInDegrees(laneNode1, laneNode2);
	b2 = GeoVector::BearingInDegrees(laneNode2, laneNode1);
	bMidpoint = GeoVector::BearingInDegrees(midpoint, laneNode2);

	//calculate common points
	bearingToNewPoint = bMidpoint - 90.0;
	if (bearingToNewPoint < 0.0)
		bearingToNewPoint = 360.0 + bearingToNewPoint;
	commonA2B1 = GeoVector::DestinationPoint(midpoint, bearingToNewPoint, laneWidth / 2.0);
	bearingToNewPoint = bMidpoint + 90.0;
	if (bearingToNewPoint > 360.0)
		bearingToNewPoint = bearingToNewPoint - 360.0;
	commonC4D3 = GeoVector::DestinationPoint(midpoint, bearingToNewPoint, laneWidth / 2.0);

	//quadrant A
	bearingToNewPoint = b1 - 90.0;
	if (bearingToNewPoint < 0.0)
		bearingToNewPoint = 360.0 + bearingToNewPoint;
	cornerPoint = GeoVector::DestinationPoint(laneNode1, bearingToNewPoint, laneWidth / 2.0);
	q.p1 = cornerPoint;
	q.p2 = commonA2B1;
	q.p3 = laneNode1;
	q.p4 = midpoint;
	DangerZones[ForwardCurbZone] = q;

	//quadrant B
	bearingToNewPoint = b2 + 90.0;
	if (bearingToNewPoint > 360.0)
		bearingToNewPoint = bearingToNewPoint - 360.0;
	cornerPoint = GeoVector::DestinationPoint(laneNode2, bearingToNewPoint, laneWidth / 2.0);
	q.p1 = commonA2B1;
	q.p2 = cornerPoint;
	q.p3 = midpoint;
	q.p4 = laneNode2;
	DangerZones[RearCurbZone] = q;

	//quadrant C
	bearingToNewPoint = b1 + 90.0;
	if (bearingToNewPoint > 360.0)
		bearingToNewPoint = bearingToNewPoint - 360.0;
	cornerPoint = GeoVector::DestinationPoint(laneNode1, bearingToNewPoint, laneWidth / 2.0);
	q.p1 = laneNode1;
	q.p2 = midpoint;
	q.p3 = cornerPoint;
	q.p4 = commonC4D3;
	DangerZones[ForwardCenterZone] = q;


	//quadrant D
	bearingToNewPoint = b2 - 90.0;
	if (bearingToNewPoint < 0.0)
		bearingToNewPoint = 360.0 + bearingToNewPoint;
	cornerPoint = GeoVector::DestinationPoint(laneNode2, bearingToNewPoint, laneWidth / 2.0);
	q.p1 = midpoint;
	q.p2 = laneNode2;
	q.p3 = commonC4D3;
	q.p4 = cornerPoint;
	DangerZones[RearCenterZone] = q;
}

void Roadway::LoadSidewalkLaneQuadrant(WGS84Point laneNode1, WGS84Point laneNode2, double laneWidth)
{
	//The sidewalk danger zone does not need to be split up, we utlize it as a whole danger zone in
	//entirety.
	//However, for continuity, lets transform its two lane nodes into its 4 corners as the other zones are done

	double b1; // bearing laneNode1 to laneNode2
	double b2;  // bearing laneNode2 to laneNode1
	double bearingToNewPoint;  // calculated bearing to new point
	Quadrant sidewalk;

	//calculate bearings
	b1 = GeoVector::BearingInDegrees(laneNode1, laneNode2);
	b2 = GeoVector::BearingInDegrees(laneNode2, laneNode1);

	//calculate p1
	bearingToNewPoint = b1 - 90.0;
	if (bearingToNewPoint < 0.0)
		bearingToNewPoint = 360.0 + bearingToNewPoint;
	sidewalk.p1 = GeoVector::DestinationPoint(laneNode1, bearingToNewPoint, laneWidth / 2.0);

	//calculate p2
	bearingToNewPoint = b2 + 90.0;
	if (bearingToNewPoint > 360.0)
		bearingToNewPoint = bearingToNewPoint - 360.0;
	sidewalk.p2 = GeoVector::DestinationPoint(laneNode2, bearingToNewPoint, laneWidth / 2.0);

	//calculate p3
	bearingToNewPoint = b1 + 90.0;
	if (bearingToNewPoint > 360.0)
		bearingToNewPoint = bearingToNewPoint - 360.0;
	sidewalk.p3 = GeoVector::DestinationPoint(laneNode1, bearingToNewPoint, laneWidth / 2.0);

	//calculate p4
	bearingToNewPoint = b2 - 90.0;
	if (bearingToNewPoint < 0.0)
		bearingToNewPoint = 360.0 + bearingToNewPoint;
	sidewalk.p4 = GeoVector::DestinationPoint(laneNode2, bearingToNewPoint, laneWidth / 2.0);

	DangerZones[WaitingZone] = sidewalk; //Store into postion SidewalkZone
}

WGS84Point Roadway::getQuadrantMidpoint(TransitStopQuadrant qName)
{
	return GeoVector::Intersection(  // intersection of two diagonals
			DangerZones[qName].p1,   // path from top left corner
			DangerZones[qName].p4,   // to bottom right corner
			DangerZones[qName].p2,   // path from top right corner
			DangerZones[qName].p3 ); // to bottom left corner
}

}
} // namespace tmx::utils

