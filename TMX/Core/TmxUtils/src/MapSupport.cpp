/*
 * MapSupport.cpp
 *
 *  Created on: Apr 21, 2016
 *      Author: ivp
 */

#include "MapSupport.h"
#include <cmath>

//#include "DsrcUtils.h"
//#include "Queue.h"

//#include <stdio.h>
//#include <string.h>
//#include <pthread.h>
//#include <endian.h>

//#include <TravelerInformation.h>
//#include <AlaCarte.h>
//#include <wave.h>

using namespace std;

namespace tmx {
namespace utils {

MapSupport::MapSupport() {
	// TODO Auto-generated constructor stub

}

MapSupport::~MapSupport() {
	// TODO Auto-generated destructor stub
}

/**
 * Reference point at center of intersection. Determines if vehicle is approaching
 * or departing the intersection
 */
void IsApproachingRefPoint(WGS84Point previousPoint, WGS84Point currentPoint) {

}

bool MapSupport::IsVehicleLane(int laneId, ParsedMap map) {
	bool isVehicleLane = false;

	for (std::list<MapLane>::iterator i = map.Lanes.begin();
			i != map.Lanes.end(); i++) {
		//Check all three types of lanes that stand for vehicle lanes.
		if (laneId == i->LaneNumber
				&& (i->Type == Vehicle || i->Type == Computed
						|| i->Type == Egress)) {
			isVehicleLane = true;
			break;
		}
	}

	return isVehicleLane;
}

/*
 * MAP file does not always specify offsets in decimeters. Pull this factor out of xml and use it.
 */
void ExtractOffsetConversionFactor() {
	//todo
}

/**
 * Returns -2 if not on the map. -1 if not in a lane. 0 if in the intersection itself. else, lane id matched.
 */
MapMatchResult MapSupport::FindVehicleLaneForPoint(WGS84Point point,
		ParsedMap map) {
	MapMatchResult r;
	r.DistanceMeters = 0;
	r.IsInLane = false;
	r.LaneSegment = 0;
	r.IsEgress = false;

	//First see if the point is inside the loose bounds of the MAP at all.
	if(!IsPointOnMapUsa(point,map))
	{
		r.LaneNumber = -2; //Not on the map.
		return r;
	}

	//Iterate over all lanes.
	list<MapLane>::iterator i;
	//Iterate over all vehicle lanes in the region.
	for (i = map.Lanes.begin(); i != map.Lanes.end(); ++i) {
		if (IsVehicleLane(i->LaneNumber, map)) {
			MapMatchResult result = PointIsInLane(*i, point);
			if (result.IsInLane) {
				cout << "Found Lane " << result.LaneNumber;
				return result;
			}
		}
	}
	//We have not matched to a lane. See if we are actually within the intersection
	if (IsInCenterOfIntersection(point, map)) {

		r.LaneNumber = 0; //return 0 to represent being within the intersection itself.

		return r;
	}
	r.LaneNumber = -1; //return -1 to represent not found.
	return r;
}

bool MapSupport::IsPointOnMapUsa(WGS84Point point,ParsedMap map) {

	if(point.Latitude > map.MaxLat || point.Latitude < map.MinLat
			|| point.Longitude > map.MaxLong || point.Longitude < map.MinLong){
		//point exceeded bounds.
		return false;
}
return true;
}


int MapSupport::GetSignalGroupForVehicleLane(int laneId, ParsedMap map) {
	int signalGroup = -1;

	for (std::list<MapLane>::iterator i = map.Lanes.begin();
			i != map.Lanes.end(); i++) {
		if (laneId == i->LaneNumber
				&& (i->Type == Vehicle || i->Type == Computed)) {
			signalGroup = i->SignalGroupId;
			break;
		}
	}

	return signalGroup;
}
/*
 * When actually IN the intersection, the vehicle may not be in a "lane" but certainly
 * is still in important territory to monitor.
 */
bool MapSupport::IsInCenterOfIntersection(WGS84Point point, ParsedMap map) {
	//For draft 1, let's just take a simple distance from the point to the reference point,
	//and if it is less than X meters we'll assume the bus in in the intersection.
	//TODO - improve this.
	double dist = Conversions::DistanceMeters(map.ReferencePoint, point);
	if (dist < 30) {
		return true;
	}

	return false;
}
MapMatchResult MapSupport::PointIsInLane(MapLane lane, WGS84Point point) {
//Get all the lane Nodes
//--	MapLaneNode[] laneNodes = lane.getLaneNodes().toArray(new MapLaneNode[0]);

	//cout << "Lane: " << lane.ToString() << endl;

//Walk through all the lane nodes (actually this walks through the lane segments)
	//--for (int i = 1; i < laneNodes.length; i++) {
	list<LaneNode>::iterator n;
	//Iterate over all vehicle lanes in the region.
	n = lane.Nodes.begin();

	WGS84Point nodeStart = n->Point;
	int laneSegment = 0;
	//Compares node point to previous so start one in.
	for (n++; n != lane.Nodes.end(); ++n) {
		laneSegment++;
		WGS84Point nodeEnd = n->Point;
		// We need to build our triangle.  Get the distance of each lane segment, and the distance from
		// me to each end of the lane segment
		double laneDist_m = Conversions::DistanceMeters(nodeEnd, nodeStart);
		double dist1 = Conversions::DistanceMeters(nodeEnd, point);
		double dist2 = Conversions::DistanceMeters(nodeStart, point);
		double halfLaneWidth = lane.LaneWidthMeters / 2;

		//We want to narrow down the further math we need to do to only points in the vicinity of the
		//line segment.
		//We could check if each node-to-point distance is less than the length of the segment - this
		//would mean that we were "inside" the lane segment (think x dimension).
		//However, this check fails as the point moves to the left or right to the extreme that the
		//triangle becomes a right triangle.  Because for a right triangle, where dist1 is the halfLanewidth
		//then clearly dist2 is the hypotenuse of that right triangle and will be more than the segment length.

		//We want to allow this point in, but keep the math simple since this is just to narrow down the points,
		//so if we add the halfLaneWidth to teh allowance, this allows MORE than enough to cover how much longer
		//that hypotenuse would be if it were the extreme case. It still rejects a lot of points though.
		if ((laneDist_m + halfLaneWidth > dist1)
				&& (laneDist_m + halfLaneWidth > dist2)) {
			//However this does let in obtuse triangles where the point really lines up better with another
			//segment (e.g. the point is 'past' the segment in x.  So lets add another check to ensure the
			//triangle is not obtuse.
			//In triangle ABC, if c is the longest side of the triangle, then
			//Acute: c^2 < a^2 + b^2
			//Right: c^2 = a^2 + b^2
			//Obtuse: c^2 > a^2 + b^2
			bool isObtuse = false;
			double longest = dist1 > dist2 ? dist1 : dist2;
			double shortest = dist1 > dist2 ? dist2 : dist1;

			double ab = pow(laneDist_m, 2) + pow(shortest, 2);
			double c = pow(longest, 2);
			if (c > ab)
				isObtuse = true;

			if (!isObtuse) {

				/**
				 * A method for calculating the area of a triangle when you know the lengths of all three sides.

				 Let a,b,c be the lengths of the sides of a triangle. The area is given by:
				 Area	=	 √	 p	 (	p	−	a	) 	(	p	−	b	)	 (	p	−	c	)
				 where p is half the perimeter, or
				 a	+	b	+	c
				 _________________
				 2


				 */
				double s = (.5) * (laneDist_m + dist1 + dist2);
				double area = sqrt(
						(s * (s - dist1) * (s - dist2) * (s - laneDist_m)));

				// using geometry, find the perpendicular distance from me to the lane
				/**
				 * Usually called "half of base times height", the area of a triangle is given by the formula below.
				 Area	=	b*a
				 ----
				 2

				 where
				 b  is the length of the base
				 a  is the length of the corresponding altitude
				 */

				double dist_perp = 2 * (area / laneDist_m);

				// We need to be 1/2 of the lane width distance from the center line to be in the lane
				if (dist_perp <= halfLaneWidth) {
					// this built a return type with the lane number that I am in, with the distance from the lane
					// center line
//				MapMessage.MapLaneCheckReturnType
//				returnType = new MapLaneCheckReturnType();
//				returnType.setDistance_meters(dist_perp);
//				returnType.setIsInLane(true);
//				returnType.setLaneNumber(lane.getLaneNumber());

					MapMatchResult res;
					res.IsInLane = true;
					res.DistanceMeters = dist_perp;
					res.LaneNumber = lane.LaneNumber;
					res.IsEgress = lane.Direction == Egress_Computed ? true : false;
					res.LaneSegment = laneSegment;			//save laneSegment
					return res;
				}
			}
		}

		//Set up for next iteration
		nodeStart = n->Point;
	}

	//This returns not in lane
//	MapMessage.MapLaneCheckReturnType
//	returnType = new MapLaneCheckReturnType();
//	returnType.setDistance_meters(-1);
//	returnType.setIsInLane(false);
//	returnType.setLaneNumber(lane.getLaneNumber());

	MapMatchResult res;
	res.LaneNumber = lane.LaneNumber;
	return res;
}

//
//void *oncoming_run(void *arg)
//{
//	Oncoming *oncoming = (Oncoming *)arg;
//	assert(oncoming != NULL);
//
//
//
//	//State
//	int lastLaneChangeSignType = -1;
//
//	clock_gettime(CLOCK_MONOTONIC, &oncoming->lastTimReceiveTime);
//
//	//Timing
//
//	struct timespec lastGpsUpdateTime;
//
//	clock_gettime(CLOCK_MONOTONIC, &lastGpsUpdateTime);
//
//
//		struct timespec currentTime;
//		clock_gettime(CLOCK_MONOTONIC, &currentTime);
//
//
//		if (oncomingData_hasTimInformation(&oncoming->data) && tsDiff(lastCalculationTime, currentTime) >= 1.0/config.oncomingMaxCalculationRate)
//		{
//
//
//
//			clock_gettime(CLOCK_MONOTONIC, &lastCalculationTime);
//
//			int allowAcmBroadcast = tsDiff(lastAcmBroadcastTime, currentTime) >= 1.0/config.dsrcAcmMaxRate && config.dsrcAcmMaxRate > 0.0;
//			if (allowAcmBroadcast)
//				clock_gettime(CLOCK_MONOTONIC, &lastAcmBroadcastTime);
//
//			lock_lock(oncoming->data.lock);
//
//			/*
//			 * Match vehicle to lanes
//			 */
//			GPSData gpsData;
//			gpsDevice_getData(gps_getDefaultDevice(), &gpsData);
//
//			TimLaneMetaData *bestSpeedLane = NULL;
//			struct DsrcPointToPathMatchResults bestSpeedLaneMatchResults;
//
//			SingleList **iterator = &oncoming->data.tim.laneInformation;
//			while(*iterator)
//			{
//				TimLaneMetaData *laneInformation = (*iterator)->value;
//
//				struct DsrcPointToPathMatchResults matchResults = dsrc_matchPointToShapePointSet(laneInformation->shapePointSet, gpsData.latitude, gpsData.longitude, gpsData.course, config.dsrcMatchOptions);
//
//				if (matchResults.percentOffCenter < 10.0)
//				{
//					if ((bestClosedLane == NULL || matchResults.percentOffCenter < bestClosedLaneMatchResults.percentOffCenter) && (laneInformation->type == TimLaneType_Closed))
//					{
//						bestClosedLane = laneInformation;
//						bestClosedLaneMatchResults = matchResults;
//					}
//					else if ((bestSpeedLane == NULL || matchResults.percentOffCenter < bestSpeedLaneMatchResults.percentOffCenter)
//						&& (laneInformation->type == TimLaneType_SpeedRestriction))
//					{
//						bestSpeedLane = laneInformation;
//						bestSpeedLaneMatchResults = matchResults;
//					}
//				}
//
//				iterator = &(*iterator)->nextNode;
//			}
//
//
//
//
//
//
//			lock_unlock(oncoming->data.lock);
//		}
//
//
//
//
//
//	queue_destroy(snapshotQueue);
//
//	return NULL;
//}
//
//int oncoming_tryGetRawLanePosition(Oncoming *oncoming, double latitude, double longitude, double heading, double *out)
//{
//	assert(oncoming != NULL);
//	assert(out != NULL);
//
//	const int MAX_PERCENT_OFF_CENTER = 50.0;
//
//	int results = 0;
//	lock_lock(oncoming->data.lock);
//
//	if (oncoming->data.tim.laneInformation != NULL)
//	{
//		RescumeConfig config;
//		rescumeConfig_get(&config);
//
//		//Match the first lane...
//		SingleList *iterator = oncoming->data.tim.laneInformation;
//		TimLaneMetaData *lastLaneInformation = iterator->value;
//		struct DsrcPointToPathMatchResults lastMatchResults = dsrc_matchPointToShapePointSet(lastLaneInformation->shapePointSet, latitude, longitude, heading, config.dsrcMatchOptions);
//
//		if (lastMatchResults.percentOffCenter < MAX_PERCENT_OFF_CENTER)
//		{
//			*out = (lastMatchResults.percentOffCenter / 2.0) * lastMatchResults.sideOfPath * oncoming->data.tim.sideOfRoad * -1.0 + lastLaneInformation->laneNumber;
//			results = 1;
//		}
//
//		iterator = iterator->nextNode;
//
//		//Try to find a better match combination..
//		while(iterator)
//		{
//			TimLaneMetaData *laneInformation = iterator->value;
//			struct DsrcPointToPathMatchResults matchResults = dsrc_matchPointToShapePointSet(laneInformation->shapePointSet, latitude, longitude, heading, config.dsrcMatchOptions);
//
//			if(matchResults.sideOfPath != lastMatchResults.sideOfPath)
//			{
//				*out = ((lastMatchResults.percentOffCenter / 2.0) * lastMatchResults.sideOfPath * oncoming->data.tim.sideOfRoad * -1.0 + lastLaneInformation->laneNumber);
//				*out += ((matchResults.percentOffCenter / 2.0) * matchResults.sideOfPath * oncoming->data.tim.sideOfRoad * -1.0 + laneInformation->laneNumber);
//				*out /= 2.0;
//
//				results = 2;
//				break;
//			}
//			else if (matchResults.percentOffCenter < lastMatchResults.percentOffCenter && matchResults.percentOffCenter < MAX_PERCENT_OFF_CENTER)
//			{
//				*out = (matchResults.percentOffCenter / 2.0) * matchResults.sideOfPath * oncoming->data.tim.sideOfRoad * -1.0 + laneInformation->laneNumber;
//			}
//
//			lastLaneInformation = laneInformation;
//			lastMatchResults = matchResults;
//
//			iterator = iterator->nextNode;
//		}
//
//	}
//
//	lock_unlock(oncoming->data.lock);
//	return results;
//}

//OncomingAlgorithmSnapshot *oncoming_createOncomingAlgorithmSnapshot(Oncoming *oncoming, OncomingAlgorithmSnapshot *reuse)
//{
//	assert(oncoming != NULL);
//
//	if (!reuse)
//	{
//		reuse = (OncomingAlgorithmSnapshot *)calloc(1, sizeof(OncomingAlgorithmSnapshot));
//	}
//
//	if (!reuse)
//		return NULL;
//
//	clock_gettime(CLOCK_MONOTONIC, &reuse->timestamp);
//
//	GPSData gpsData;
//	gpsDevice_getData(gps_getDefaultDevice(), &gpsData);
//
//	RescumeConfig config;
//	rescumeConfig_get(&config);
//
//	reuse->speed = gpsData.speed;
//
//	int i;
//	for(i = 0; i < MAX_LANE_COUNT; i++)
//	{
//		reuse->laneLocations[i] = -1.0;
//	}
//
//	lock_lock(oncoming->data.lock);
//
//	SingleList *laneIterator = oncoming->data.tim.laneInformation;
//
//	while(laneIterator != NULL)
//	{
//		TimLaneMetaData *laneInformation = laneIterator->value;
//		assert(laneInformation != NULL);
//		assert(laneInformation->laneNumber < MAX_LANE_COUNT);
//
//		struct DsrcPointToPathMatchResults matchResults = dsrc_matchPointToShapePointSet(laneInformation->shapePointSet, gpsData.latitude, gpsData.longitude, gpsData.course, config.dsrcMatchOptions);
//
//		if (matchResults.percentOffCenter < 1000.0)
//		{
//			reuse->laneLocations[laneInformation->laneNumber] = matchResults.distanceAlongPath;
//		}
//
//		laneIterator = laneIterator->nextNode;
//	}
//
//	lock_unlock(oncoming->data.lock);
//
//	return reuse;
//}

//void oncoming_updateOncomingSnapshots(Oncoming *oncoming, Queue *snapshots)
//{
//	OncomingAlgorithmSnapshot *removedSnapshot = NULL;
//
//	struct timespec currentTime;
//	clock_gettime(CLOCK_MONOTONIC, &currentTime);
//
//	while(queue_front(snapshots) != NULL && tsDiff(((OncomingAlgorithmSnapshot *)queue_front(snapshots))->timestamp, currentTime) > 4.0)
//	{
//		if (removedSnapshot)
//			free(removedSnapshot);
//		removedSnapshot = queue_pop(snapshots);
//	}
//
//	if (!queue_back(snapshots) || tsDiff(((OncomingAlgorithmSnapshot *)queue_back(snapshots))->timestamp, currentTime) > 0.1)
//	{
//		OncomingAlgorithmSnapshot * newSnapshot = oncoming_createOncomingAlgorithmSnapshot(oncoming, removedSnapshot);
//		if (newSnapshot)
//			queue_push(snapshots, newSnapshot);
//	}
//}

}
} // namespace tmx::utils
