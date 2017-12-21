/*
 * Intersection.cpp
 *
 *  Created on: Apr 29, 2016
 *      Author: ivp
 */

#include "Intersection.h"
#include "ParsedMap.h"


#include <mutex>

using namespace std;
using namespace tmx;
using namespace tmx::messages;

namespace tmx {
namespace utils {
mutex spatLock;
mutex mapLock;
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

	_isMapLoaded = false;
	_isSpatLoaded = false;
	//SpatMsg = NULL;
}

Intersection::~Intersection() {
	//  Auto-generated destructor stub
}

/**
 * Some helper functions, defined in a different source file
 */
IntersectionGeometryList *FindIntersections(MapDataMessage &msg);
IntersectionStateList *FindIntersections(SpatMessage &msg);
WGS84Point GetMapReferencePoint(IntersectionGeometryList *intersections, RoadSegmentList *roadSegments);
void FillMapLaneData(MapLane &mapLane, GenericLane *lane, int laneWidth);
std::list<LaneNode> ComputeLaneNodes(WGS84Point refPoint, int laneId, GenericLane **lanes, int laneCount, double totalXOffset, double totalYOffset);
WGS84Point GetPointFromNodeOffset(WGS84Point &referencePoint, WGS84Point &laneNodeOffset);

/**
 * Iterate over Map message and pull out the lanes of interest by region for the intersection.
 * Only loads if newer
 */
bool Intersection::LoadMap(MapDataMessage &msg) {
//	bool Intersection::LoadMap(ParsedMap& parsedMap) { //MapDataMessage &msg) {
	//Get version from the map message and compare to the value we've stored.  Only parse and update if the
	//map has been updated.

	lock_guard<mutex> lock(mapLock);
	lastMapUpdateAttemptTime = GetMsTimeSinceEpoch();
	auto geom = FindIntersections(msg);
	if (!geom)
		return false;

	int thisId = geom->list.array[0]->id.id;
	int thisRev = msg.get_j2735_data()->msgIssueRevision;

//	PLOG(logDEBUG) << "LoadMap Id:" << _intersectionId << " New Version:" << thisRev;
	if (thisRev != _mapVersion) {
//		PLOG(logDEBUG) << "Loading Map:" << _intersectionId << " Version:" << _mapVersion << " New Version:" << thisRev;

		_mapVersion = thisRev;

		_intersectionId = thisId;

		//Iterate over all lanes and get their id's.
		//Use the ids to map them over to the correct regions/parts. (OR could use the type specified in the file?)
		WGS84Point _mapReferencePoint = GetMapReferencePoint(geom, 0);

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


		MapMessage = msg; 

		_isMapLoaded = true;

		return true; //We really updated the data.
	}
	return false; //The map was what we already had and did not update anything.
}

bool Intersection::UpdateSpat(SpatMessage &msg)
{
	if (DoesSpatMatchMap(msg))
	{
			lock_guard<mutex> lock(spatLock);
			SpatMsg = msg;
			lastSpatTime = GetMsTimeSinceEpoch();
			_isSpatLoaded = true;
			return true;
	}

	return false;
}

void Intersection::ClearMap()
{
	_isMapLoaded = false;

}

void Intersection::ClearSpat()
{
	_isSpatLoaded = false;
}

int Intersection::GetMapId()
{
	if(_isMapLoaded)
		return _intersectionId;
	else
		return -1;
}

bool Intersection::IsPedestrianReportedForConnectionId(int connectionId) {
	//Query spat for existence of node: <ManeuverAssistList><ConnectionManeuverAssist><LaneConnectionID>
	//Any part of path may be absent.
	if (!_isSpatLoaded) return false;

	lock_guard<mutex> lock(spatLock);

	auto *states = FindIntersections(SpatMsg);
	if (!states)
		return false;

	for(int i=0; i < states->list.count; i++)
	{
		IntersectionState *intersectionState = states->list.array[i];
		if (!intersectionState->maneuverAssistList)
			continue;

		for(int j = 0; j < intersectionState->maneuverAssistList->list.count; j++)
		{
			ConnectionManeuverAssist *cma = intersectionState->maneuverAssistList->list.array[j];
			int connId = cma->connectionID;
			if(connectionId == connId)
			{
				bool isPed = cma->pedBicycleDetect;
				PLOG(logDEBUG) << "+++++ Pedestrian:" << isPed << " Id:" << connId;
				return isPed;
			}

		}
	}

	//If the connectionID for that lane does not exist, it is false.
	return false;
}

bool Intersection::DoesSpatMatchMap(tmx::messages::SpatMessage &msg) {
	//Verify that this spat message data matches the intersection that we have mapped out.

	if(!_isMapLoaded)
		return false;

	auto interX = FindIntersections(msg);
	if (!interX)
		return false;

	for (int i = 0; i < interX->list.count; i++)
	{
		if(interX->list.array[i]->id.id == _intersectionId)
		{
			return true;
		}
	}

	return false;
}

bool Intersection::IsSignalForGroupRedLight(tmx::messages::SpatMessage &msg, int signalGroup) {

	auto interX = FindIntersections(msg);
	for (int i = 0; interX && i < interX->list.count; i++)
	{
		for (int j = 0; interX->list.array[i]->states.list.count; i++)
		{
			MovementState *movement = interX->list.array[i]->states.list.array[j];
			if (movement && movement->signalGroup == signalGroup)
			{
				for (int k = 0; k < movement->state_time_speed.list.count; k++)
				{
					MovementEvent *event = movement->state_time_speed.list.array[k];
					if (event && event->eventState == MovementPhaseState_stop_And_Remain)
						return true;
				}
			}
		}
	}

	return false;
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
 * Gets a lane data from the provided lane id from the MapData message
 */
MapLane Intersection::GetMapLane(tmx::messages::MapDataMessage &msg, int laneId) {
	MapLane mapLane;

	auto geom = FindIntersections(msg);
	if (!geom || geom->list.count <= 0)
		return mapLane;

	int laneWidth = 0;
	if (geom->list.array[0]->laneWidth)
		laneWidth = *(geom->list.array[0]->laneWidth);

	//Set the lanes
	for (int i = 0; i < geom->list.array[0]->laneSet.list.count; i++) {

		GenericLane *lane = geom->list.array[0]->laneSet.list.array[i];
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
std::list<MapLane> Intersection::GetMapLanes(tmx::messages::MapDataMessage &msg) {
	std::list<MapLane> lanes;

	auto *geom = FindIntersections(msg);
	if (!geom)
		return lanes;

	for (int i = 0; geom->list.count > 0 && i < geom->list.array[0]->laneSet.list.count; i++)
	{
		lanes.push_back(GetMapLane(msg, geom->list.array[0]->laneSet.list.array[i]->laneID));

		for (list<LaneNode>::iterator j = lanes.back().Nodes.begin(); j != lanes.back().Nodes.end(); j++)
				UpdateMapBoundaryBox(j->Point);

	}

	return lanes;
}

/**
 * Gets the all of the lane nodes for a give lane id from the MapData message
 */
std::list<LaneNode> Intersection::GetLaneNodes(tmx::messages::MapDataMessage &msg, int laneId, double totalXOffset, double totalYOffset) {

	auto geom = FindIntersections(msg);
	if (!geom || geom->list.count <= 0)
		return std::list<LaneNode>();

	return ComputeLaneNodes(GetMapReferencePoint(geom, 0), laneId,
			geom->list.array[0]->laneSet.list.array,
			geom->list.array[0]->laneSet.list.count,
			totalXOffset, totalYOffset);
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
		laneNode.Point = tmx::utils::GetPointFromNodeOffset(j->Point, laneNodeOffset);
		computedNodes.push_back(laneNode);
	}

	return computedNodes;
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

double Intersection::TimeRemainingForLocation_Sec(double lat, double lon)
{
	if(!_isSpatLoaded || !_isMapLoaded)
		return -1;

	WGS84Point location(lat, lon);

	MapSupport mapSupp;

	//PLOG(logDEBUG) << "GetSignalForLocation: lat: " << lat << ", lon: " << lon;

	MapMatchResult r = mapSupp.FindVehicleLaneForPoint(location, Map);

	int laneSegment = r.LaneSegment;

	//PLOG(logDEBUG) << "GetSignalForLocation: IsInLane: " << r.IsInLane << ", LaneNumber: " << r.LaneNumber << ", LaneSegment: " << r.LaneSegment;

	if(r.IsInLane == false || r.LaneNumber < 0 || laneSegment < 1)
	{
		return -1;
	}

	int signalGroupId = mapSupp.GetSignalGroupForVehicleLane(r.LaneNumber, Map);

	return TimeRemainingForSignalGroup(signalGroupId);
}

uint64_t Intersection::GetMsTimeSinceEpoch()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t) ((double) (tv.tv_sec) * 1000
			+ (double) (tv.tv_usec) / 1000);
}

double Intersection::TimeRemainingForSignalGroup(int signalGroupId)
{
	if(!_isSpatLoaded || !_isMapLoaded)
		return -1;

	lock_guard<mutex> lock(spatLock);

	auto interX = FindIntersections(SpatMsg);

	for(int i=0; interX && i < interX->list.count; i++)
	{
		IntersectionState *intersectionState = interX->list.array[i];

		for(int j = 0; intersectionState && j < intersectionState->states.list.count ; j++)
		{
			MovementState *movement = intersectionState->states.list.array[j];
			int sgId = movement->signalGroup;
			if(sgId == signalGroupId)
			{
				//PLOG(logDEBUG) << "minEndTime: " << movement->state_time_speed.list.array[0]->timing->minEndTime;
				long minTime = movement->state_time_speed.list.array[0]->timing->minEndTime;
				long timeDiff;
				if (intersectionState->moy != NULL && intersectionState->timeStamp != NULL)
				{
					//use time data in spat
					uint64_t processTime = (GetMsTimeSinceEpoch() - lastSpatTime) / 100;
					ldiv_t dc = div(minTime, (long)600);
					long mohc = dc.quot;
					ldiv_t db = div((long)(*(intersectionState->moy)), (long)60);
					long mohb = *(intersectionState->moy) - (db.quot * 60);
					long dsohb = mohb * 60 * 10 + (*(intersectionState->timeStamp) / 100);
					long mdiff, dsdiff;
					if (mohb > mohc)
						mdiff = mohc + 60 - mohb;
					else
						mdiff = mohc - mohb;
					if (dsohb > minTime)
						dsdiff = (mdiff - 1) * 600 + (minTime + 600 - dsohb);
					else
						dsdiff = mdiff * 600 + minTime - dsohb;
					//now subtract processing time to get final time to change
					timeDiff = dsdiff - processTime;
					//PLOG(logDEBUG) << "dc: " << dc.quot << ", db: " << db.quot << ", minTime: " << minTime << ", mdiff: " << mdiff;
					//PLOG(logDEBUG) << "processTime: " << processTime << ", mohc: " << mohc << ", mohb: " << mohb << ", dsohb: " << dsohb;
					//PLOG(logDEBUG) << "moy: " << *(intersectionState->moy) << ", timeStamp: " << *(intersectionState->timeStamp) << ", dsdiff: " << dsdiff;
				}
				else
				{
					//use clock time
					long currentTime = Clock::getAdjustedTime(0);

					timeDiff = minTime - currentTime;

					if(timeDiff < 0)
						timeDiff = -1 * timeDiff;
				}

				double dTimeDiff = (double)timeDiff/10.0;
				return dTimeDiff;
			}
		}
	}

	return -1.0;
}

Signal Intersection::GetSignalForLocation(double lat, double lon)
{
	if(!_isSpatLoaded || !_isMapLoaded)
		return Signal::Unknown;

	WGS84Point location(lat, lon);

	MapSupport mapSupp;

//	PLOG(logDEBUG) << "GetSignalForLocation: lat: " << lat << ", lon: " << lon;

	MapMatchResult r = mapSupp.FindVehicleLaneForPoint(location, Map);

	int laneSegment = r.LaneSegment;

//	PLOG(logDEBUG) << "GetSignalForLocation: IsInLane: " << r.IsInLane << ", LaneNumber: " << r.LaneNumber << ", LaneSegment: " << r.LaneSegment;

	if(r.IsInLane == false || r.LaneNumber < 0 || laneSegment < 1)
	{
		return Signal::Unknown;
	}

	int signalGroupId = mapSupp.GetSignalGroupForVehicleLane(r.LaneNumber, Map);
//	PLOG(logDEBUG) << "GetSignalForLocation: signalGroupId: " << signalGroupId;

	return GetSignalForSignalGroup(signalGroupId);
}

Signal Intersection::GetSignalForSignalGroup(int signalGroupId)
{
	if (signalGroupId < 0)	return Signal::Unknown;

	lock_guard<mutex> lock(spatLock);

	auto interX = FindIntersections(SpatMsg);

	for(int i=0; interX && i < interX->list.count; i++)
	{
		IntersectionState *intersectionState = interX->list.array[i];

		for(int j=0; intersectionState && j < intersectionState->states.list.count ; j++)
		{
			MovementState *movement = intersectionState->states.list.array[j];
			int sgId = movement->signalGroup;
//			PLOG(logDEBUG) << "sgId:" << sgId << " signalGroup:" << signalGroupId;
			if(sgId == signalGroupId)
			{
				int eventState = movement->state_time_speed.list.array[0]->eventState;

				switch(eventState)
				{
				case MovementPhaseState::MovementPhaseState_permissive_Movement_Allowed:
				case MovementPhaseState::MovementPhaseState_protected_Movement_Allowed:
//					PLOG(logDEBUG) << "Signal --Green-- for Signal Group " << signalGroupId;
					return Signal::Green;
					break;
				case MovementPhaseState::MovementPhaseState_caution_Conflicting_Traffic:
				case MovementPhaseState::MovementPhaseState_permissive_clearance:
				case MovementPhaseState::MovementPhaseState_protected_clearance:
//					PLOG(logDEBUG) << "Signal --Yellow-- for Signal Group " << signalGroupId;
					return Signal::Yellow;
					break;
				case MovementPhaseState::MovementPhaseState_stop_And_Remain:
				case MovementPhaseState::MovementPhaseState_stop_Then_Proceed:
//					PLOG(logDEBUG) << "Signal --Red-- for Signal Group " << signalGroupId;
					return Signal::Red;
					break;
				case MovementPhaseState::MovementPhaseState_dark:
				case MovementPhaseState::MovementPhaseState_unavailable:
				default:
//					PLOG(logDEBUG) << "Signal --UNKNOWN-- for Signal Group " << signalGroupId;
					return Signal::Unknown;
					break;

				}
			}
		}

	}

	return Signal::Unknown;
}

double Intersection::GetCurrentGrade(double lat, double lon)
{

	if(!_isMapLoaded)
		return 0;

	MapDataMessage mapCopy;
	{
		lock_guard<mutex> lock(mapLock);
		mapCopy = MapMessage;
	}

	WGS84Point location(lat, lon);

	MapSupport mapSupp;

	MapMatchResult r = mapSupp.FindVehicleLaneForPoint(location, Map);

	int laneSegment = r.LaneSegment;

	if(r.IsInLane == false || r.LaneNumber < 0 || laneSegment < 1)
	{
		return 0;
	}

	double grade = 0.0;
	int nodeIndex = 0;
	WGS84Point point1;
	WGS84Point point2;
	std::list<LaneNode>::iterator it;
	std::list<LaneNode> t_nodes = GetLaneNodes(mapCopy, r.LaneNumber, 0.0, 0.0);
	for (it=t_nodes.begin(); it != t_nodes.end() && nodeIndex < laneSegment; ++it)
	{
		point1 = point2;
		point2 = it->Point;
		//PLOG(logDEBUG1) << "node: " << point2.Latitude << ", " << point2.Longitude;
		if (nodeIndex > 0)
		{

			grade = Conversions::GradeDegrees(point1, point2);

		}
		nodeIndex++;
	}

	return grade;
}


/**
 * Function finds the distance to the crossing based on the current
 * location and the data in the Map message
 *
 * @return distance to the crossing in meters, -1 indicates that the vehicle is not in a lane.
 */
double Intersection::GetDistanceToIntersection(double lat, double lon)
{
	if(!_isMapLoaded)
		return -1;

	MapDataMessage mapCopy;
	{
		lock_guard<mutex> lock(mapLock);
		mapCopy = MapMessage;
	}

	WGS84Point location(lat, lon);

	MapSupport mapSupp;

	//PLOG(logDEBUG) << "GetDistanceToIntersection: lat: " << lat << ", lon: " << lon;

	MapMatchResult r = mapSupp.FindVehicleLaneForPoint(location, Map);

	int laneSegment = r.LaneSegment;

	if(r.IsInLane == false || r.LaneNumber < 0 || laneSegment < 1)
	{
		return -1;
	}

	if(r.IsEgress)
		return -1;

	//PLOG(logDEBUG) << "GetDistanceToIntersection: IsInLane: " << r.IsInLane << ", LaneNumber: " << r.LaneNumber << ", LaneSegment: " << r.LaneSegment;

	// Calculate the distance to the crossing on a node by node basis
	// to account for curves when approaching the intersection.

//	double distance = 0.0;
//	int nodeIndex = 0;
//	WGS84Point point1;
//	WGS84Point point2;
//	std::list<LaneNode>::iterator it;
//	//PLOG(logDEBUG) << "GetDistanceToIntersection: MapMessage: " << MapMessage;
//	std::list<LaneNode> t_nodes = GetLaneNodes(mapCopy, r.LaneNumber, 0.0, 0.0);
//	//PLOG(logDEBUG) << "GetDistanceToIntersection: t_nodes.size: " << t_nodes.size();
//	for (it=t_nodes.begin(); it != t_nodes.end() && nodeIndex < laneSegment; ++it)
//	{
//		point1 = point2;
//		point2 = it->Point;
//		//PLOG(logDEBUG) << "GetDistanceToIntersection: node: " << point2.Latitude << ", " << point2.Longitude;
//		if (nodeIndex > 0)
//		{
//
//			distance += Conversions::DistanceMeters(point1, point2);
//			//PLOG(logDEBUG) << "GetDistanceToIntersection: distance: " << distance;
//		}
//		nodeIndex++;
//	}
//	distance += Conversions::DistanceMeters(point2, location);
	//PLOG(logDEBUG) << "GetDistanceToIntersection: final distance: " << distance;

	//return stop distance from MapMatchResult
	return r.StopDistanceMeters;
}

int Intersection::GetLaneIdForLocation(double lat, double lon)
{
	if(!_isMapLoaded)
		return -1;

	ParsedMap mapCopy;
	{
		lock_guard<mutex> lock(mapLock);
		mapCopy = Map;
	}

	WGS84Point location(lat, lon);

	MapSupport mapSupp;

	//PLOG(logDEBUG) << "GetDistanceToIntersection: lat: " << lat << ", lon: " << lon;

	MapMatchResult r = mapSupp.FindVehicleLaneForPoint(location, mapCopy);

	int laneSegment = r.LaneSegment;

	if(r.IsInLane == false || r.LaneNumber < 0 || laneSegment < 1)
	{
		return -1;
	}

	return r.LaneNumber;
}

MapMatchResult Intersection::GetLaneForLocation(double lat, double lon)
{
	if(!_isMapLoaded)
	{
		MapMatchResult r;
		r.PerpDistanceMeters = 0;
		r.StopDistanceMeters = 0;
		r.IsInLane = false;
		r.LaneSegment = 0;
		r.IsEgress = false;
		r.LaneNumber = -2;
		return r;

	}

	ParsedMap mapCopy;
	{
		lock_guard<mutex> lock(mapLock);
		mapCopy = Map;
	}
	WGS84Point location(lat, lon);

	MapSupport mapSupp;

	//PLOG(logDEBUG) << "GetDistanceToIntersection: lat: " << lat << ", lon: " << lon;

	MapMatchResult r = mapSupp.FindVehicleLaneForPoint(location, mapCopy);

	return r;
}

bool Intersection::IsLocationStillInLane(double lat, double lon, int laneId)
{
	if(!_isMapLoaded)
		return false;

	ParsedMap mapCopy;
	{
		lock_guard<mutex> lock(mapLock);
		mapCopy = Map;
	}

	WGS84Point location(lat, lon);

	MapSupport mapSupp;

	//PLOG(logDEBUG) << "GetDistanceToIntersection: lat: " << lat << ", lon: " << lon;

	return mapSupp.IsPointInLane(location, laneId, mapCopy);
}

double Intersection::GetStoppingDistance(double lat, double lon, double speed, double mu, double reactionTime)
{
	double incline = GetCurrentGrade(lat, lon); //TODO calculated based on MAP
	double distance = 0.0;
	distance = (reactionTime * speed) + ((speed * speed) / (2 * 9.8 * ((mu * cos(incline)) + sin(incline))));
	return distance;
}

bool Intersection::IsPointOnMap(double lat, double lon)
{
	if(!_isMapLoaded)
		return false;

	ParsedMap mapCopy;
	{
		lock_guard<mutex> lock(mapLock);
		mapCopy = Map;
	}

	WGS84Point location(lat, lon);

	MapSupport mapSupp;

	//PLOG(logDEBUG) << "GetDistanceToIntersection: lat: " << lat << ", lon: " << lon;

	return mapSupp.IsPointOnMapUsa(location, mapCopy);
}

//Return MAP and SPAT loaded status
bool Intersection::IsMapLoaded()
{
	return _isMapLoaded;
}

bool Intersection::IsSpatLoaded()
{
	return _isSpatLoaded;
}


}
} // namespace tmx::utils

