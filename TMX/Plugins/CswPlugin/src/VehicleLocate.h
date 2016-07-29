/*
 * VehicleLocate.h
 *
 *  Created on: Oct 14, 2014
 *      Author: ivp
 */

#ifndef VEHICLELOCATE_H_
#define VEHICLELOCATE_H_

#include <tmx/IvpPlugin.h>
#include <asn_j2735_r41/BasicSafetyMessage.h>
#include <asn_j2735_r41/BasicSafetyMessageVerbose.h>
#include "DsrcBuilder.h"
#include "WGS84Point.h"

class VehicleLocate
{
public:
	static bool GetOffsets(Offsets_t *offsets, double *x, double *y);
	static WGS84Point* GetPointArray(Offsets_t **nodes, int nodesLength, Position3D *nodesAnchor);
	static int FindRegion(TravelerInformation *tim, WGS84Point point, uint16_t heading);
	static int FindRegion(TiDataFrame *frame, WGS84Point point, uint16_t heading);
	static bool IsInPointList(WGS84Point* points, int pointsLength, WGS84Point point, uint16_t heading, double laneWidth);

	static void GetInt16(unsigned char *buf, int16_t *value);
	static void GetUInt16(unsigned char *buf, uint16_t *value);
	static void GetInt32(unsigned char *buf, int32_t *value);
	static void GetUInt32(unsigned char *buf, uint32_t *value);
	static bool ProcessBsmMessage(IvpMessage *msg, TravelerInformation *tim, int* regionNumber, float* speed_mph, int32_t *vehicleId);
};

#endif /* VEHICLELOCATE_H_ */
