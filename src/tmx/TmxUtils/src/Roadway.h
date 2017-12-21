/*
 * Roadway.h
 *
 *  Created on: Apr 29, 2016
 *      Author: ivp
 */

#ifndef SRC_INTERSECTION_H_
#define SRC_INTERSECTION_H_
#include <array>
#include "Region.h"
#include "MapSupport.h"
#include <tmx/j2735_messages/MapDataMessage.hpp>
#include <tmx/j2735_messages/SpatMessage.hpp>
#include <tmx/pugixml/pugixml.hpp>

using namespace pugi;



namespace tmx {
namespace utils {


/**
 * contains the points of the four corners of a box.
 * Used to chop transit lanes into parts of interest.
 */
class Quadrant
{
	public:
	WGS84Point p1;
	 WGS84Point p2;
	 WGS84Point p3;
	 WGS84Point p4;
};
enum TransitStopZoneNumber
{
	LandingLane=1,  //Transit vehicle landing area - this one is chopped into quadrants
	ApproachLane=2, //Approach lane
	PassingLane=3,  //Passing Lane
	SidewalkLane=4  //Sidewalk lane
};
enum TransitStopQuadrant
{
	NoQuadrant = 0,
	Safe = 1, //Unused by logic.
	WaitingZone =2,//Sidewalk Danger Zone
	ForwardCurbZone=3,//In Roadway Forward Curb Zone
	RearCurbZone=4,//In Roadway Rear Curb Zone,
	ForwardCenterZone=5,//In Roadway Forward Center Zone
	RearCenterZone=6,//In Roadway Rear Center Zone

};


#define QUERYROADSEGMENTMAPID "MapData.roadSegments.RoadSegment.id.id"

class Roadway {
public:

	Roadway();
	virtual ~Roadway();

	///Compares the MAP message to the currently loaded version and updates all data/Regions accordingly if its new.
	///Returns true if the map was truly updated/new. Returns false if the map was not new and thus unchanged.
	bool LoadMap(tmx::messages::MapDataMessage &msg);
	///Returns the roadway id of the loaded/parsed MAP roadway.
	int GetMapId();
	/**
	 * Returns true if the spat message indicates that lane Id has a pedestrian.
	 */
	bool IsPedestrianReportedForLane(int laneId, tmx::messages::SpatMessage &msg);

	/// Returns quadrant for given point.
	TransitStopQuadrant FindQuadrantForPoint(WGS84Point& point);

	int GetMapRevision(tmx::messages::MapDataMessage &msg);
	WGS84Point GetMapRReferencePoint(tmx::messages::MapDataMessage &msg);
	std::list<MapLane> GetMapLanes(tmx::messages::MapDataMessage &msg);
	MapLane GetMapLane(tmx::messages::MapDataMessage &msg, int laneId);
	std::list<LaneNode> GetLaneNodes(tmx::messages::MapDataMessage &msg, int laneId, double totalXOffset, double totalYOffset);


	ParsedMap Map;
	///Returns lat/longs of all points in a list
	std::string MapToString();

	/**
	 * Transit stops have 5 defined areas of pedestrian danger.
	 * To reduce confusion, the enums (enum TransitStopQuadrant) mapped to the danger zones have been kept to match
	 * the design/conops documents. However, the zones we track start at 2, so we have two wasted spaces
	 * here. since the Quadrand object is small, that is acceptable for the clarity it will provide to testers, etc.
	 */
	std::array<Quadrant, 7> DangerZones;  // quadrants are numbered 2-6 ?, using enum as index requires 7 elements
	void UpdateDangerZonesForMap();

	/**
	 * Given a quadrant, calculate its midpoint.
	 * Possible use: PSM from TSPW roadside unit.
	 */
	WGS84Point getQuadrantMidpoint(TransitStopQuadrant qName);

private:

	/**
	 * Determines which lanes are computed and then compares the reference lane id of the computed lane to the vehicle
	 * lane with the nodes already computed and assgins the nodes with the given computed offset to the computed lane node list
	 */
	void SetComputedLanes(std::list<MapLane> &lanes);
	/**
	 * Get the offset values for the computed lanes nodes
	 */
	std::list<LaneNode> GetOffsetNodeList(std::list<LaneNode> &nodes, WGS84Point &laneNodeOffset);

	///Call after setting the reference point to begin building the bounding box for the MAP.
	void InitMapBoundaryBox();
	///Check point and update bounding box to include it.
	void UpdateMapBoundaryBox(WGS84Point point);

	/**Divides the lane specified by the two points into 4 equal parts
	 * Stores to member array of danger zones.
	 */
	void LoadLandingLaneQuadrants(WGS84Point laneNode1, WGS84Point laneNode2, double laneWidth);
	/**
	 * Loads sidewalk zone to member array of danger zones.
	 */
	void LoadSidewalkLaneQuadrant(WGS84Point laneNode1, WGS84Point laneNode2, double laneWidth);
	///This is the revision number from the MAP file that indicates if the contents have been updated
	///and should be reparsed, or if they are unchanged.
	int _mapVersion;
	///The Id for the roadway of the MAP file.
	int _mapId;
};

}} // namespace tmx::utils

#endif /* SRC_INTERSECTION_H_ */
