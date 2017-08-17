/*
 * Intersection.h
 *
 *  Created on: Apr 29, 2016
 *      Author: ivp
 */

#ifndef SRC_INTERSECTION_H_
#define SRC_INTERSECTION_H_

#include "Region.h"
#include "MapSupport.h"
#include <tmx/j2735_messages/MapDataMessage.hpp>
#include <tmx/j2735_messages/SpatMessage.hpp>
#include <tmx/pugixml/pugixml.hpp>
#include "PluginLog.h"

using namespace pugi;

//Forward declaration of unit test class required to friend it for testing.
namespace unit_test{
 class IntersectionTest_UpdateRegions_Test;
 class IntersectionTest_CheckRegionForVehicleLaneId_Test;
 class IntersectionTest_GetPointFromNodeOffset_Test;
}

namespace tmx {
namespace utils {

/**
 * Direction(path) of intended traversal through the intersection.
 */
enum class Direction
{
	Dir_Straight=0,
	Dir_Left,
	Dir_Right
};

enum class Signal
{
	Unknown=0,
	Green,
	Yellow,
	Red
};
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
 *Vehicles:
 * Lane Ids 10-19 are region1 , LaneIds 20-29 are region 2, LaneIds 30-39 are region 3,
 * Lane Ids 40-49 are region 4
 * Crosswalks:
 * Lane Ids 1 are region1 , LaneIds 2 are region 2, LaneIds 3 are region 3,
 * Lane Ids 4 are region 4
 * Curbs:
 * Lane Ids 110-119 are region1 , LaneIds 120-129 are region 2, LaneIds 130-139 are region 3,
 * Lane Ids 140-149 are region 4.
 * As a vehicle approaches an intersection, odd curb areas are to the left and even curb areas are on the right.
 *
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
 *  In this example, Region 3 will just be empty/not populated.
 */


#define QUERYINTERSECTIONMAPID "MapData.intersections.IntersectionGeometry.id.id"
class Intersection {
public:
	friend class unit_test::IntersectionTest_UpdateRegions_Test;//friend unit test for testing private members.
	friend class unit_test::IntersectionTest_CheckRegionForVehicleLaneId_Test;//friend unit test for testing private members.
	friend class unit_test::IntersectionTest_GetPointFromNodeOffset_Test;//friend unit test for testing private members.
	Intersection();
	virtual ~Intersection();

	///Compares the MAP message to the currently loaded version and updates all data/Regions accordingly if its new.
	///Returns true if the map was truly updated/new. Returns false if the map was not new and thus unchanged.
	bool LoadMap(tmx::messages::MapDataMessage &msg);
	///Returns the intersection id of the loaded/parsed MAP intersection.
	int GetMapId();
	/**
	 * Returns true if the spat message indicates that lane Id has a pedestrian.
	 */
	bool IsPedestrianReportedForLane(int laneId, tmx::messages::SpatMessage &msg);

	/**
	 * Compares the Intersection id of the passed in spat message to the object's active map intersection id.
	 */
	bool DoesSpatMatchMap(tmx::messages::SpatMessage &msg);
	/**
	 * Some applications do certain actions for all cases except a red light. This simplified
	 * call examines the signal phase for the signalGroup provided and returns true for red
	 * and false for all other permutations. False also returned if unknown/undetermined.
	 * relevantFields is populated with the excerpts from the xml used to make the determination.
	 */
	bool IsSignalForGroupRedLight(int signalGroup,
			tmx::messages::SpatMessage &msg, std::string &relevantFields);
	//bool LoadMap(ParsedMap& parsedMap);
	/**
	 * Returns a list of all the lane IDs that correspond to curbs in relevant region,
	 * based on the MAP data.
	 * @param region Integer of region of interest.
	 */
	std::list<int>  GetCurbLaneIdsForRegion(int region) const;
	/**
	 * Returns a list of all the lane IDs that correspond to crosswalks in relevant region,
	 * based on the MAP data.
	 * @param region Integer of region of interest.
	 */
	std::list<int> GetCrosswalkLaneIdsForRegion(int region) const;
	/**
	 * Checks for vehicle lane among the loaded regions.  Used for determining ingress region after
	 *current lane has been discovered.
	 *current @param laneId current lane of vehicle
	 */
	int GetRegionForVehicleLaneId(int laneId) const;
	int GetRegionForCrosswalkLaneId(int laneId) const;
	int GetRegionForCurbLaneId(int laneId) const;
	/*
	 * Determines which region number the vehicle is destined to exit from by using the region of approach, the
	 * intersection configuration rules, and the intended direction of travel.
	 * @param approachRegion region number vehicle is entering intersection from.
	 * @param direction Vehicle is either traveling left, straight, or right through the intersection.
	 */
	int FindEgressRegion(int approachRegion, Direction direction) const;

	int GetMapRevision(tmx::messages::MapDataMessage &msg);
	WGS84Point GetMapRReferencePoint(tmx::messages::MapDataMessage &msg);
	std::list<MapLane> GetMapLanes(tmx::messages::MapDataMessage &msg);
	MapLane GetMapLane(tmx::messages::MapDataMessage &msg, int laneId);
	std::list<LaneNode> GetLaneNodes(tmx::messages::MapDataMessage &msg, int laneId, double totalXOffset, double totalYOffset);

	///Populated from MAP data with lanes for all regions.
	///Index 0 is a dummy region placeholder (TODO could be used for any 'leftovers' that don't follow our rules)
	std::vector<Region> Regions;
	ParsedMap Map;
	///Returns lat/longs of all points in a list
	std::string MapToString();

private:
	/*
	 * Parses MAP data lanes into their respective 'Region' buckets based on the MAP intersection agreement.
	 * @param laneId MAP message lane identifier.
	 *
	 */
	int UpdateRegionForLaneId(int laneId);
	/*
	 * Iterates over all vehicle lanes in the region to see if the lane is within it.
	 * @param laneId lane that is being searched for in the region by lane identifier.
	 */
	int CheckRegionForVehicleLaneId(const Region& r, int laneId) const;
	int CheckRegionForCurbLaneId(const Region& r, int laneId) const;
	int CheckRegionForCrosswalkLaneId(const Region& r, int laneId) const;
	/**Updates region info based on updated map data
	 *
	 */
	void UpdateRegions();
	/**
	 * Gets the nodes from the initial reference point then adds the offsets x and y
	 */
	WGS84Point GetPointFromNode(WGS84Point &referencePoint, int &latval, int &longval, double &totalXOffset, double &totalYOffset);
	/**
	 * Get the nodes of the computed lanes based on the reference node of the vehicle lane and the computed lane offsets
	 */
	WGS84Point GetPointFromNodeOffset(WGS84Point &referencePoint, WGS84Point &laneNodeOffset);
	/**
	 * Gets the directional use for the MapLane object
	 */
	DirectionalUse GetDirectionalUse(std::string &directionalUse);
	/**
	 * Gets the lane type based on the directional use for the MapLane object
	 */
	LaneType GetLaneType(pugi::xml_node &node, DirectionalUse &directionalUse);
	LaneType GetLaneType(pugi::xpath_node &laneNode, DirectionalUse &directionalUse);
	/**
	 * Get all of the nodes for a lane. Note will not get the computed lane nodes, they will be empty and set later.
	 */
	std::list<LaneNode> GetLaneNodes(pugi::xml_node &node, double &totalXOffset, double &totalYOffset);
	std::list<LaneNode> GetLaneNodes(pugi::xpath_node &laneNode, double &totalXOffset, double &totalYOffset);
	/**
	 * Get the reference lane id from the computed node for compairison to the vehicle lane
	 */
	int GetReferenceLaneId(pugi::xml_node &node, DirectionalUse &directionalUse);
	int GetReferenceLaneId(pugi::xpath_node &node, DirectionalUse &directionalUse);
	/**
	 * Determines which lanes are computed and then compares the reference lane id of the computed lane to the vehicle
	 * lane with the nodes already computed and assgins the nodes with the given computed offset to the computed lane node list
	 */
	void SetComputedLanes(std::list<MapLane> &lanes);
	/**
	 * Get the offset values for the computed lanes
	 */
	WGS84Point GetComputedLaneOffset(pugi::xml_node &node);
	WGS84Point GetComputedLaneOffset(pugi::xpath_node &laneNode);
	/**
	 * Get the offset values for the computed lanes nodes
	 */
	std::list<LaneNode> GetOffsetNodeList(std::list<LaneNode> &nodes, WGS84Point &laneNodeOffset);

	///Call after setting the reference point to begin building the bounding box for the MAP.
	void InitMapBoundaryBox();
	///Check point and update bounding box to include it.
	void UpdateMapBoundaryBox(WGS84Point point);

	///This is the revision number from the MAP file that indicates if the contents have been updated
	///and should be reparsed, or if they are unchanged.
	int _mapVersion;
	///The Id for the intersection of the MAP file.
	int _intersectionId;
};

}} // namespace tmx::utils

#endif /* SRC_INTERSECTION_H_ */
