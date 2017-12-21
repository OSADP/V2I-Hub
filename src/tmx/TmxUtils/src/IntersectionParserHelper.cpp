/*
 * IntersectionHelpers.cpp
 *
 *  Created on: Jul 28, 2017
 *      Author: gmb
 */

#include "Intersection.h"

// Some differences in the GenericLane types
#if SAEJ2735_SPEC < 63
#define DEFINED_NODE NodeList2_PR_nodes
#define COMPUTED_NODE NodeList2_PR_computed
#define ENUM_NAME(X) NodeOffsetPoint_PR_node_ ## X
#else
#define DEFINED_NODE NodeListXY_PR_nodes
#define COMPUTED_NODE NodeListXY_PR_computed
#define ENUM_NAME(X) NodeOffsetPointXY_PR_node_ ## X
typedef NodeXY Node;
#endif

using namespace tmx::messages;

namespace tmx {
namespace utils {

IntersectionGeometryList *FindIntersections(MapDataMessage &msg)
{
	if (!msg.get_j2735_data())
		return 0;
	else
		return msg.get_j2735_data()->intersections;
}

RoadSegmentList *FindRoadSegments(MapDataMessage &msg)
{
	if (!msg.get_j2735_data())
		return 0;
	else
		return msg.get_j2735_data()->roadSegments;
}

IntersectionStateList *FindIntersections(SpatMessage &msg)
{
	if (!msg.get_j2735_data())
		return 0;
	else
		return &msg.get_j2735_data()->intersections;
}

/**
 * Gets the reference point element from the MapData message
 */
WGS84Point GetMapReferencePoint(IntersectionGeometryList *intersections, RoadSegmentList *roadSegments) {
	double latitude = 0.0;
	double longitude = 0.0;
	double elevation = 0.0;

	if (intersections && intersections->list.count > 0)
	{
		//Get the ParsedMap data from the child objects
		latitude = intersections->list.array[0]->refPoint.lat;
		longitude = intersections->list.array[0]->refPoint.Long;
		if (intersections->list.array[0]->refPoint.elevation)
			elevation = *(intersections->list.array[0]->refPoint.elevation);
	}
	else if (roadSegments && roadSegments->list.count > 0)
	{
		latitude = roadSegments->list.array[0]->refPoint.lat;
		longitude = roadSegments->list.array[0]->refPoint.Long;
		if (roadSegments->list.array[0]->refPoint.elevation)
			elevation = *(roadSegments->list.array[0]->refPoint.elevation);
	}

	latitude /= 10000000.0;
	longitude /= 10000000.0;
	elevation /= 10.0;

	return WGS84Point(latitude, longitude, elevation);
}

/**
 * Gets the directional use for the MapLane object
 */
DirectionalUse GetDirectionalUse(LaneDirection_t &use) {
	uint8_t ch = 0;
	if (use.size > 0)
		ch = (use.buf[0] >> 6);

	switch (ch)
	{
	case 1:
		return Egress_Computed;
	case 2:
		return Ingress_Vehicle_Computed;
	case 3:
		return Ingress_Pedestrian;
	default:
		return NotApplicable;
	}
}

/**
 * Get the reference lane id from the computed node for comparison to the vehicle lane
 */
int GetReferenceLaneId(GenericLane *lane, DirectionalUse &directionalUse) {
	if (!lane || lane->nodeList.present != COMPUTED_NODE)
		return -1;

	if (directionalUse == Ingress_Vehicle_Computed || directionalUse == Egress_Computed) {
		return static_cast<int>(lane->nodeList.choice.computed.referenceLaneId);
	}

	return -1;
}

/**
 * Gets the lane type based on the directional use for the MapLane object
 */
LaneType GetLaneType(GenericLane *lane, DirectionalUse &directionalUse) {
	switch(directionalUse)
	{
	case Ingress_Vehicle_Computed:
		if (lane->nodeList.present == DEFINED_NODE)
			return LaneType::Vehicle;
		else if (lane->nodeList.present == COMPUTED_NODE)
			return LaneType::Computed;
		else
			return LaneType::Other;
	case Ingress_Pedestrian:
		return LaneType::Pedestrian;
	case Egress_Computed:
		return LaneType::Egress;
	default:
		return LaneType::Other;
	}

	//TODO:Need to do side walk
}

/**
 * Get the offset values for the computed lanes
 */
WGS84Point GetComputedLaneOffset(GenericLane *lane) {
	WGS84Point empty;

	if (!lane || lane->nodeList.present != COMPUTED_NODE)
		return empty;

	auto x = lane->nodeList.choice.computed.offsetXaxis;
	auto y = lane->nodeList.choice.computed.offsetYaxis;

	long xOffset = 0;
	long yOffset = 0;

	if (x.present == ComputedLane__offsetXaxis_PR_large)
		xOffset = x.choice.large;
	else if (x.present == ComputedLane__offsetXaxis_PR_small)
		xOffset = x.choice.small;

	if (y.present == ComputedLane__offsetYaxis_PR_large)
		yOffset = y.choice.large;
	else if (y.present == ComputedLane__offsetYaxis_PR_small)
		yOffset = y.choice.small;

	return WGS84Point(xOffset, yOffset);
}

/**
 * Gets the nodes from the initial reference point then adds the offsets x and y
 */
WGS84Point GetPointFromNode(WGS84Point &referencePoint, long &latval, long &longval, double& totalXOffset, double& totalYOffset) {
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
WGS84Point GetPointFromNodeOffset(WGS84Point &referencePoint, WGS84Point &laneNodeOffset) {
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

void FillMapLaneData(MapLane &mapLane, GenericLane *lane, int laneWidth) {
	if (!lane)
		return;

	mapLane.LaneNumber = lane->laneID;
	mapLane.Direction = GetDirectionalUse(lane->laneAttributes.directionalUse);
	mapLane.LaneDirectionEgress = (mapLane.Direction == DirectionalUse::Egress_Computed);

	if (laneWidth > 0)
		mapLane.LaneWidthMeters = (double) laneWidth / 100; //TODO auto detect conversion factor? is this always fixed or dynamic like offsets?

	mapLane.ReferenceLaneId = GetReferenceLaneId(lane, mapLane.Direction);
	mapLane.Type = GetLaneType(lane, mapLane.Direction);
	mapLane.LaneNodeOffset = GetComputedLaneOffset(lane);

	for (int j = 0; lane->connectsTo && j < lane->connectsTo->list.count; j++)
	{
		if (lane->connectsTo->list.array[j]->signalGroup)
		{
			//may need to capture all signal groups if a lane can have more than one
			mapLane.SignalGroupId = *(lane->connectsTo->list.array[j]->signalGroup);
			break;
		}
	}

}

/**
 * Gets the all of the lane nodes for a give lane id from the MapData message
 */
std::list<LaneNode> ComputeLaneNodes(WGS84Point refPoint, int laneId, GenericLane **lanes, int laneCount,
		double totalXOffset, double totalYOffset) {

	std::list<LaneNode> laneNodes;

	if (!lanes)
		return laneNodes;

#define ENUM_NAMEXY(X) ENUM_NAME(XY ## X)
#define FIELD_NAME(X) node->delta.choice.node_XY ## X
#define CHECK_AND_SET(X) \
			case ENUM_NAMEXY(X): \
				x = (FIELD_NAME(X)).x; \
				y = (FIELD_NAME(X)).y; \
				break

	for (int i = 0; i < laneCount; i++)
	{
		GenericLane *lane = lanes[i];
		if (lane->laneID != laneId)
			continue;

		if (lane->nodeList.present == DEFINED_NODE)
		{
			for (int j = 0; j < lane->nodeList.choice.nodes.list.count; j++)
			{
				Node *node = lane->nodeList.choice.nodes.list.array[j];
				long x;
				long y;

				switch (node->delta.present)
				{
				CHECK_AND_SET(1);
				CHECK_AND_SET(2);
				CHECK_AND_SET(3);
				CHECK_AND_SET(4);
				CHECK_AND_SET(5);
				CHECK_AND_SET(6);
				case ENUM_NAME(LatLon):
					x = node->delta.choice.node_LatLon.lat;
					y = node->delta.choice.node_LatLon.lon;
					break;
				default:
					continue;
				}

				LaneNode laneNode;
				laneNode.Point = tmx::utils::GetPointFromNode(refPoint, x, y, totalXOffset, totalYOffset);
				laneNodes.push_back(laneNode);
			}
		}
		else if (lane->nodeList.present == COMPUTED_NODE)
		{
			ComputedLane *node = &lane->nodeList.choice.computed;
			int referenceLaneId = node->referenceLaneId;

			long xOffset = 0; //in cm
			long yOffset = 0; //in cm

			if (node->offsetXaxis.present == ComputedLane__offsetXaxis_PR_small)
				xOffset = node->offsetXaxis.choice.small;
			else if (node->offsetXaxis.present == ComputedLane__offsetXaxis_PR_large)
				xOffset = node->offsetXaxis.choice.large;

			if (node->offsetYaxis.present == ComputedLane__offsetYaxis_PR_small)
				yOffset = node->offsetYaxis.choice.small;
			else if (node->offsetYaxis.present == ComputedLane__offsetYaxis_PR_large)
				yOffset = node->offsetYaxis.choice.large;

			for (int j = 0; j < laneCount; j++)
			{
				GenericLane *referenceLane = lanes[j];
				if (referenceLane->laneID != referenceLaneId)
					continue;

				for (int k = 0; k < referenceLane->nodeList.choice.nodes.list.count; k++)
				{
					Node *node = referenceLane->nodeList.choice.nodes.list.array[k];
						long x;
						long y;

						switch (node->delta.present)
						{
						CHECK_AND_SET(1);
						CHECK_AND_SET(2);
						CHECK_AND_SET(3);
						CHECK_AND_SET(4);
						CHECK_AND_SET(5);
						CHECK_AND_SET(6);
						case ENUM_NAME(LatLon):
							x = node->delta.choice.node_LatLon.lat;
							y = node->delta.choice.node_LatLon.lon;
							break;
						default:
							continue;
						}

						LaneNode laneNode;
						double totalComputedXOffset = totalXOffset + (xOffset * .01);
						double totalComputedYOffset = totalYOffset + (yOffset * .01);
						laneNode.Point = tmx::utils::GetPointFromNode(refPoint, x, y, totalComputedXOffset, totalComputedYOffset);
						laneNodes.push_back(laneNode);
				}
			}
		}
	}

#undef CHECK_AND_SET
#undef FIELD_NAME

	return laneNodes;
}



} /* namespace utils */
} /* namespace tmx */


