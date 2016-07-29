#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <asn_j2735_r41/MapData.h>
#include <asn_j2735_r41/UPERframe.h>


#include "ConvertToJ2735r41.h"

namespace MapPlugin
{

volatile int gMapMessageCount = 0;

ConvertToJ2735r41::ConvertToJ2735r41()
{
	std::cout << "ConvertToJ2735r41::ConvertToJ2735r41 Constructor Called" << std::endl;
	encodedByteCount = 0;
	derEncodedByteCount = 0;
}

ConvertToJ2735r41::~ConvertToJ2735r41()
{

}

int ConvertToJ2735r41::convertMap(MapData *MapJ2735, map *mapMessage, unsigned char* encodedBytes)
{
	memset(MapJ2735, 0, sizeof(MapData));

	// TODO: Map-P Not defined from ASN.1 using literal instead
	MapJ2735->msgID = 0x12; // Map-P

	// Counter for message that is issued
	MapJ2735->msgIssueRevision = gMapMessageCount++;
	if (gMapMessageCount > 127) gMapMessageCount = 0;

	// Layer type
	MapJ2735->layerType = (LayerType_t *)calloc(1, sizeof(LayerType_t));
	*(MapJ2735->layerType) = LayerType_intersectionData;
	// Layer - not used
//	MapJ2735->layerID = (LayerID_t *)calloc(1, sizeof(LayerID_t));
//	MapJ2735->layerID = mapMessage->intersectionid;


	map_lane *curLane;
	int g,l,n;
	int groups;
	// TODO: Check for end of geometries and do not use max
	for (g=0; g<map_maxgeometries; g++)
	{
		map_geometry *geom = &mapMessage->geometry[g];
		// CHeck for more geometries
    	if ((geom->refpoint.longitude == 0) && (geom->refpoint.latitude == 0)) break;

		IntersectionGeometry *intersection = (IntersectionGeometry *)calloc(1, sizeof(IntersectionGeometry));
		intersection->id.id = (IntersectionID_t) mapMessage->intersectionid;
		// TODO: Include Version from XML - Not defined in structure but it is in the message
		intersection->revision = (MsgCount_t) 1;
		intersection->refPoint.Long = geom->refpoint.longitude;
		intersection->refPoint.lat = geom->refpoint.latitude;

		// Check for elevation attribute
		if (mapMessage->attributes & 0x01)
		{
			intersection->refPoint.elevation = (Elevation2_t *)calloc(1, sizeof(Elevation2_t));
			*(intersection->refPoint.elevation) = geom->refpoint.elevation;
		}

//		std::cout << "Reference Point Lat:" << intersection->refPoint.lat << " Long:" << intersection->refPoint.lat << std::endl;
		map_group *approach = &geom->approach;

		MapJ2735->intersections = (IntersectionGeometryList *)calloc(1, sizeof(IntersectionGeometryList));

		// TODO: Lane width needs to be for intersection. Taking width of first lane until width is available
		intersection->laneWidth = (LaneWidth_t *)calloc(1, sizeof(LaneWidth_t));
		*(intersection->laneWidth) = approach->lane[0].width;

		for (groups = 0; groups < 2; groups++) 
		{
	        for (l=0; l<map_maxlanes; l++)
	        {

	    		if (approach->lane[l].number == 0x00)
	    		{
	//    			std::cout << "Done with Lanes" << std::endl;
	    			break;
	    		}
	    		GenericLane *lane = (GenericLane *)calloc(1, sizeof(GenericLane));
	    		lane->laneID = (LaneID_t) approach->lane[l].number;
	    		std::cout << "Creating Lane:" << lane->laneID << std::endl;

	    		// Add Connects to
	    		if (approach->lane[l].connection[0].lanenumber !=0 )
	    		{
	    			lane->connectsTo = (ConnectsToList *)calloc(1, sizeof(ConnectsToList));

	    			int ct;
	    			for (ct=0; ct < map_maxconnections; ct++)
	    			{
	    				if (approach->lane[l].connection[ct].lanenumber !=0 )
	    				{
	    					Connection *newConnection = (Connection *)calloc(1, sizeof(Connection));
	    					newConnection->connectingLane.lane = approach->lane[l].connection[ct].lanenumber;

	    					if (approach->lane[l].connection[ct].maneuver != 0)
	    					{
//	    						newConnection->connectingLane.maneuver  = (AllowedManeuvers_t *)calloc(1, sizeof(AllowedManeuvers_t));
//	    						*(newConnection->connectingLane.maneuver) = approach->lane[l].connection[ct].maneuver;

	    						newConnection->connectingLane.maneuver = (AllowedManeuvers_t *) calloc(1, sizeof(AllowedManeuvers_t));
	    						newConnection->connectingLane.maneuver->buf = (uint8_t *)calloc(2, sizeof(uint8_t));
	    						newConnection->connectingLane.maneuver->size = 2 * sizeof(uint8_t);
	    						newConnection->connectingLane.maneuver->bits_unused = 4;
	    						int shift = 8 - newConnection->connectingLane.maneuver->bits_unused;
	    						newConnection->connectingLane.maneuver->buf[1] |= approach->lane[l].connection[ct].maneuver << shift;
	    						newConnection->connectingLane.maneuver->buf[0] |= (approach->lane[l].connection[ct].maneuver >> shift);
	    						printf(" Maneuver [1] %x [0] %x \n", newConnection->connectingLane.maneuver->buf[1], newConnection->connectingLane.maneuver->buf[0]);
	    						//std::cout << "Maneuver [1] " << newConnection->connectingLane.maneuver->buf[1] << " [0] " << newConnection->connectingLane.maneuver->buf[0] << std::endl;
	    					}


							newConnection->signalGroup = (SignalGroupID_t *)calloc(1, sizeof(SignalGroupID_t));
							*(newConnection->signalGroup) = approach->lane[l].connection[ct].signalGroup;

	    					//added connectionID
//							newConnection->connectionID = (LaneConnectionID_t *)calloc(1, sizeof(LaneConnectionID_t));
//							*(newConnection->connectionID) = approach->lane[l].connection[ct].connectionID;

	    					ASN_SEQUENCE_ADD(&lane->connectsTo->list, newConnection);
	    				}
	    				else
	    				{
	    					break;
	    				}
	    			}
	    		}
				lane->laneAttributes.directionalUse.buf = (uint8_t *) calloc(1, sizeof(uint8_t));
				lane->laneAttributes.directionalUse.size = sizeof(uint8_t);
				lane->laneAttributes.directionalUse.bits_unused = 6;

				if ((approach->lane[l].type == vehicle) || (approach->lane[l].type == computed))
				{
					if (groups == 0)
						lane->laneAttributes.directionalUse.buf[0] |= 1 << (7 - LaneDirection_ingressPath);
					else if (groups == 1)
						lane->laneAttributes.directionalUse.buf[0] |= 1 << (7 - LaneDirection_egressPath);

					uint16_t attributes = createVehicleLaneAttributes(approach->lane[l].attributes);

					addLaneType(lane, (BIT_STRING_s*)&lane->laneAttributes.laneType.choice.vehicle, LaneTypeAttributes_PR_vehicle, attributes);

					addManeuvers(lane, approach->lane[l].attributes);
				}
				else if (approach->lane[l].type == pedestrian || approach->lane[l].type == crosswalk)
				{
					lane->laneAttributes.directionalUse.buf[0] |= 1 << (7 - LaneDirection_ingressPath);
					lane->laneAttributes.directionalUse.buf[0] |= 1 << (7 - LaneDirection_egressPath);

					uint16_t attributes = createCrosswalkLaneAttributes(approach->lane[l].attributes);

					addLaneType(lane, (BIT_STRING_s*)&lane->laneAttributes.laneType.choice.crosswalk, LaneTypeAttributes_PR_crosswalk, attributes);
				}
				else if (approach->lane[l].type == trackedVehicle)
				{
					lane->laneAttributes.directionalUse.buf[0] |= 1 << (7 - LaneDirection_ingressPath);
					lane->laneAttributes.directionalUse.buf[0] |= 1 << (7 - LaneDirection_egressPath);

					uint16_t attributes = createTrackedVehicleAttributes(approach->lane[l].attributes);

					addLaneType(lane, (BIT_STRING_s*)&lane->laneAttributes.laneType.choice.trackedVehicle, LaneTypeAttributes_PR_trackedVehicle, attributes);

					addManeuvers(lane, approach->lane[l].attributes);
				}
				else if (approach->lane[l].type == bike)
				{
					lane->laneAttributes.directionalUse.buf[0] |= 1 << (7 - LaneDirection_ingressPath);
					lane->laneAttributes.directionalUse.buf[0] |= 1 << (7 - LaneDirection_egressPath);

					// TODO: Add bike lane attributes.
					uint16_t attributes = 0;

					addLaneType(lane, (BIT_STRING_s*)&lane->laneAttributes.laneType.choice.bikeLane, LaneTypeAttributes_PR_bikeLane, attributes);
				}
				else if (approach->lane[l].type == sidewalk)
				{
					lane->laneAttributes.directionalUse.buf[0] |= 1 << (7 - LaneDirection_ingressPath);
					lane->laneAttributes.directionalUse.buf[0] |= 1 << (7 - LaneDirection_egressPath);

					// TODO: Add sidewalk attributes.
					uint16_t attributes = 0;

					addLaneType(lane, (BIT_STRING_s*)&lane->laneAttributes.laneType.choice.sidewalk, LaneTypeAttributes_PR_sidewalk, attributes);
				}
				else if (approach->lane[l].type == barrier)
				{
					lane->laneAttributes.directionalUse.buf[0] |= 1 << (7 - LaneDirection_ingressPath);
					lane->laneAttributes.directionalUse.buf[0] |= 1 << (7 - LaneDirection_egressPath);

					// TODO: Add barrier attributes.
					uint16_t attributes = 0;

					addLaneType(lane, (BIT_STRING_s*)&lane->laneAttributes.laneType.choice.median, LaneTypeAttributes_PR_median, attributes);
				}
				else if (approach->lane[l].type == striping)
				{
					lane->laneAttributes.directionalUse.buf[0] |= 1 << (7 - LaneDirection_ingressPath);
					lane->laneAttributes.directionalUse.buf[0] |= 1 << (7 - LaneDirection_egressPath);

					// TODO: Add striping attributes.
					uint16_t attributes = 0;

					addLaneType(lane, (BIT_STRING_s*)&lane->laneAttributes.laneType.choice.striping, LaneTypeAttributes_PR_striping, attributes);
				}
				else if (approach->lane[l].type == parking)
				{
					lane->laneAttributes.directionalUse.buf[0] |= 1 << (7 - LaneDirection_ingressPath);
					lane->laneAttributes.directionalUse.buf[0] |= 1 << (7 - LaneDirection_egressPath);

					// TODO: Add parking attributes.
					uint16_t attributes = 0;

					addLaneType(lane, (BIT_STRING_s*)&lane->laneAttributes.laneType.choice.parking, LaneTypeAttributes_PR_parking, attributes);
				}

				// Currently don't define lane sharing so set it to 0
				lane->laneAttributes.sharedWith.buf = (uint8_t *)calloc(2, sizeof(uint8_t));
				lane->laneAttributes.sharedWith.size = 2 * sizeof(uint8_t);
				lane->laneAttributes.sharedWith.bits_unused = 6;
				lane->laneAttributes.sharedWith.buf[0] = (uint8_t) 0;
				std::cout << "Added Lane Attributes" << std::endl;


	    		if (approach->lane[l].type == computed)
	    		{
	    			std::cout << "Computed Lane" << std::endl;
	    			lane->nodeList.present = NodeList2_PR_computed;
	    			lane->nodeList.choice.computed.referenceLaneId = approach->lane[l].referencelane.lanenumber;
	    			lane->nodeList.choice.computed.offsetXaxis.present = ComputedLane__offsetXaxis_PR_large;
	    			lane->nodeList.choice.computed.offsetXaxis.choice.small = approach->lane[l].referencelane.xoffset * 10;
	    			lane->nodeList.choice.computed.offsetYaxis.present = ComputedLane__offsetYaxis_PR_large;
	    			lane->nodeList.choice.computed.offsetYaxis.choice.large = approach->lane[l].referencelane.yoffset * 10;
	    		}
	    		else
				{
					lane->nodeList.present = NodeList2_PR_nodes;
					for (n=0; n<map_maxnodes; n++)
					{
						if ((approach->lane[l].node[n].eastern == 0) && (approach->lane[l].node[n].northern == 0)  && (approach->lane[l].node[n].elevation == 0)  && (approach->lane[l].node[n].width == 0))
						{
							std::cout << "End of nodes" << std::endl;
							std::cout << "Eastern Node:" << approach->lane[l].node[n].eastern << " " << l << " " << n << std::endl;
							std::cout << "Northern Node:" << approach->lane[l].node[n].northern << std::endl;
							std::cout << "Elevation Node:" << approach->lane[l].node[n].elevation << std::endl;
							std::cout << "Width Node:" << approach->lane[l].node[n].width << std::endl;
							break;
						}
						Node *newNode = (Node *)calloc(1, sizeof(Node));

						int modX = approach->lane[l].node[n].eastern * 10;
						int modY = approach->lane[l].node[n].northern * 10;
						int largestOffset;

						if (abs(modX) > abs(modY)) largestOffset = modX;
						else largestOffset = modY;

						if ((largestOffset >= -512) && (largestOffset <= 511))
						{
							newNode->delta.present = NodeOffsetPoint_PR_node_XY1;
							newNode->delta.choice.node_XY1.x = modX;
							newNode->delta.choice.node_XY1.y = modY;
						}
						else if ((largestOffset >= -1024) && (largestOffset <=1023))
						{
							newNode->delta.present = NodeOffsetPoint_PR_node_XY2;
							newNode->delta.choice.node_XY2.x = modX;
							newNode->delta.choice.node_XY2.y = modY;
						}
						else if ((largestOffset >= -2048) && (largestOffset <= 2047))
						{
							newNode->delta.present = NodeOffsetPoint_PR_node_XY3;
							newNode->delta.choice.node_XY3.x = modX;
							newNode->delta.choice.node_XY3.y = modY;
						}
						else if ((largestOffset >= -4096) && (largestOffset <= 4095))
						{
							newNode->delta.present = NodeOffsetPoint_PR_node_XY4;
							newNode->delta.choice.node_XY4.x = modX;
							newNode->delta.choice.node_XY4.y = modY;
						}
						else if ((largestOffset >= -8192) && (largestOffset <= 8191))
						{
							newNode->delta.present = NodeOffsetPoint_PR_node_XY5;
							newNode->delta.choice.node_XY5.x = modX;
							newNode->delta.choice.node_XY5.y = modY;
						}
						else if ((largestOffset >= -32768) && (largestOffset <= 32767))
						{
							newNode->delta.present = NodeOffsetPoint_PR_node_XY6;
							newNode->delta.choice.node_XY6.x = modX;
							newNode->delta.choice.node_XY6.y = modY;
						}

						// Lane 0 width used as the default width. If remaining widths are different
						// add them as an attribute.
						if (approach->lane[0].width != approach->lane[l].width)
						{
							newNode->attributes = (NodeAttributeSet *)calloc(1, sizeof(NodeAttributeSet));
							newNode->attributes->dWidth = (Offset_B10_t *)calloc(1, sizeof(Offset_B10_t));
							*(newNode->attributes->dWidth) = approach->lane[l].width - approach->lane[0].width;
						}

						ASN_SEQUENCE_ADD(&lane->nodeList.choice.nodes, newNode);
					}
				}
				ASN_SEQUENCE_ADD(&intersection->laneSet, lane);
			}
			approach = &geom->egress;
		}

		ASN_SEQUENCE_ADD(&MapJ2735->intersections->list, intersection);
	}

	std::cout << std::endl << "R41 XML:" << std::endl;
	xer_fprint(stdout, &asn_DEF_MapData, MapJ2735);

	asn_enc_rval_t rvalenc = uper_encode_to_buffer(&asn_DEF_MapData, MapJ2735, encodedBytes, 4000);

	encodedByteCount = 0;

	if (rvalenc.encoded == -1)
	{
		fprintf(stderr, "Cannot encode %s\n", rvalenc.failed_type->name);
		return -1;
	}
	else
	{
		int i;
		encodedByteCount = ((rvalenc.encoded + 7) / 8);
#if 1
		for(i = 0; i < encodedByteCount; i++)
		{
			printf("%02x ", (unsigned char) encodedBytes[i]);
		}
		printf("\n");
#endif
	}

	return encodedByteCount;
}

void ConvertToJ2735r41::addLaneType(GenericLane *lane, BIT_STRING_t *choice, LaneTypeAttributes_PR laneType, uint16_t attributes)
{
	lane->laneAttributes.laneType.present = laneType;
	choice->buf = (uint8_t *)calloc(2, sizeof(uint8_t));
	choice->size = 2* sizeof(uint8_t);
	choice->bits_unused = 0;
	choice->buf[1] |= attributes;
	choice->buf[0] |= (attributes >> 8);
}

void ConvertToJ2735r41::addManeuvers(GenericLane *lane, uint16_t attributes)
{
	//uint16_t ManueversBitString = createAllowedManeuvers(approach->lane[l].attributes);
	// TODO: Add error checking

	lane->maneuvers = (AllowedManeuvers_t *) calloc(1, sizeof(AllowedManeuvers_t));
	lane->maneuvers->buf = (uint8_t *)calloc(2, sizeof(uint8_t));
	lane->maneuvers->size = 2 * sizeof(uint8_t);
	lane->maneuvers->bits_unused = 4;
	int shift = 8 - lane->maneuvers->bits_unused;
	lane->maneuvers->buf[1] |= attributes << shift;
	lane->maneuvers->buf[0] |= (attributes >> shift);
	//lane->maneuvers->buf[1] |= ManueversBitString;
	//lane->maneuvers->buf[0] |= (ManueversBitString >> 8);
}

bool ConvertToJ2735r41::decodePerMapData(MapData* map, char* messageBuf, int bytes)
{

	std::cout << "ConvertToJ2735r41::decodePerMapData - Starting Decode" << std::endl;
	asn_dec_rval_t rval = uper_decode_complete(NULL, &asn_DEF_MapData, (void **)&map, messageBuf, bytes);

	if (rval.code == RC_OK)
	{
		xer_fprint(stdout, &asn_DEF_MapData, map);
	}
	else
	{
		std::cout << "Could not Decode Message Message" << std::endl;
		return false;
	}

	return true;
}

uint16_t ConvertToJ2735r41::createVehicleLaneAttributes(unsigned short xmlAttributes)
{

	// Add attributes
	uint16_t laneAttributes = 0;
	if (xmlAttributes & LANE_ATTR_HOV_LANE)
	{
//		std::cout << "HOV lane" << std::endl;
		laneAttributes |= 1 << (15 - LaneAttributes_Vehicle_hovLaneUseOnly);

	}
	if (xmlAttributes & LANE_ATTR_BUS_ONLY_LANE)
	{
//		std::cout << "Bus only lane" << std::endl;
		laneAttributes |= 1 << (15 - LaneAttributes_Vehicle_restrictedToBusUse);
	}
	if (xmlAttributes & LANE_ATTR_BUS_AND_TAXI_ONLY_LANE)
	{
//		std::cout << "Bus and taxi only lane" << std::endl;
		laneAttributes |= 1 << (15 - LaneAttributes_Vehicle_restrictedToBusUse);
		laneAttributes |= 1 << (15 - LaneAttributes_Vehicle_restrictedToTaxiUse);
	}
//	std::cout << "Lane Attributes Bits:" << std::endl;

//	printUINT16(laneAttributes);
	return laneAttributes;


}

uint16_t ConvertToJ2735r41::createAllowedManeuvers(unsigned short xmlAttributes)
{
	// Add attributes
	uint16_t allowedManeuvers = 0;
	// Two way lane
	if (xmlAttributes & LANE_ATTR_STRAIGHT_MANEUVER_PERMITTED)
	{
//		std::cout << "Straight maneuver permitted" << std::endl;
		allowedManeuvers |= 1 << (15 - AllowedManeuvers_maneuverStraightAllowed);
	}
	if (xmlAttributes & LANE_ATTR_LEFT_TURN_MANEUVER_PERMITTED)
	{
//		std::cout << "Left turn maneuver permitted" << std::endl;
		allowedManeuvers |= 1 << (15 - AllowedManeuvers_maneuverLeftAllowed);
	}
	if (xmlAttributes & LANE_ATTR_RIGHT_TURN_MANEUVER_PERMITTED)
	{
//		std::cout << "Right turn maneuver permitted" << std::endl;
		allowedManeuvers |= 1 << (15 - AllowedManeuvers_maneuverRightAllowed);

	}
	if (xmlAttributes & LANE_ATTR_YIELD)
	{
//		std::cout << "Yield" << std::endl;
		allowedManeuvers |= 1 << (15 - AllowedManeuvers_yieldAllwaysRequired);
	}

	if (!(xmlAttributes & LANE_ATTR_NO_U_TURN))
	{
//		std::cout << "U-turn" << std::endl;
		allowedManeuvers |= 1 << (15 - AllowedManeuvers_maneuverUTurnAllowed);
	}

	if (!(xmlAttributes & LANE_ATTR_NO_TURN_ON_RED))
	{
//		std::cout << "Turn on red" << std::endl;
		allowedManeuvers |= 1 << (15 - AllowedManeuvers_maneuverRightTurnOnRedAllowed);
	}
	if (xmlAttributes & LANE_ATTR_NO_STOPPING)
	{
//		std::cout << "No stopping" << std::endl;
		allowedManeuvers |= 1 << (15 - AllowedManeuvers_maneuverNoStoppingAllowed);
	}

//	std::cout << "Allowed Maneuvers Bits:" << std::endl;
//	printUINT16(allowedManeuvers);
	return allowedManeuvers;
}


uint16_t ConvertToJ2735r41::createCrosswalkLaneAttributes(unsigned short xmlAttributes)
{


	// Add attributes
	uint16_t laneAttributes = 0;

	if (xmlAttributes & LANE_ATTR_BIKE_LANE)
	{
//		std::cout << "Bike lane" << std::endl;
		laneAttributes |= 1 << (15 - LaneAttributes_Crosswalk_bicyleUseAllowed);
	}
//	std::cout << "Crosswalk Lane Attributes Bits:" << std::endl;

//	printUINT16(laneAttributes);
	return laneAttributes;


}

uint16_t ConvertToJ2735r41::createTrackedVehicleAttributes(unsigned short xmlAttributes)
{
	// Add attributes
	uint16_t laneAttributes = 0;

	// TODO: r36 does not contain attributes to properly determine the trackedVehicle attributes.
	// Hard coded to heavy rail for now.

	laneAttributes |= 1 << (15 - LaneAttributes_TrackedVehicle_spec_heavyRailRoadTrack);

	return laneAttributes;
}

bool ConvertToJ2735r41::createUPERframe_DERencoded_msg()
{
	derEncodedByteCount = 0;

	if (encodedByteCount <= 0)
	{
		std::cout << "No bytes to der encode." << std::endl;
		return false;
	}

	UPERframe *frame;
	frame = (UPERframe *) calloc(1, sizeof(UPERframe));
	frame->msgID = 0x11;
	frame->contentID = 0x12;

//	std::cout << "msgID:" << frame->msgID << " contentID:" << frame->contentID << std::endl;

	frame->msgBlob.buf = (uint8_t *) calloc(encodedByteCount, sizeof(uint8_t));
	frame->msgBlob.size = encodedByteCount;
	memcpy(frame->msgBlob.buf, encoded, encodedByteCount);

	xer_fprint(stdout, &asn_DEF_UPERframe, frame);


	asn_enc_rval_t encResults = der_encode_to_buffer(&asn_DEF_UPERframe, frame, &derEncoded, 4000);
	if (encResults.encoded == 0)
	{
		std::cout << "Did not Successfully DER encode message" << std::endl;
		return false;
	}
	else
	{
		std::cout << "Mape Successfully DER encoded:" << encResults.encoded << std::endl;

		derEncodedByteCount = encResults.encoded;
	}

	// Print Encoded Message
	int i;
	printf("MAP successfully DER encoded size: %d\nPayload: ", derEncodedByteCount);
	for (i=0; i < derEncodedByteCount; i++) { printf("%02x ", derEncoded[i]); }
	printf("\n");


	return true;
}

// TODO move to common file
void ConvertToJ2735r41::printUINT16(uint16_t num)
{
    uint16_t x = num;
    for (int i = 0; i < 16; i++) {
        printf("%d", (x & 0x8000) >> 15);
        x <<= 1;
    }
    printf("\n");
}



} /* End namespace MapPlugin */
