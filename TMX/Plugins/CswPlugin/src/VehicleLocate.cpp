/*
 * VehicleLocate.cpp
 *
 *  Created on: Oct 14, 2014
 *      Author: ivp
 */

#include <iostream>
#include <math.h>
#include "Conversions.h"
#include "VehicleLocate.h"

#define DSRC_POLAR_RADIUS 6356752.314247833
#define DSRC_EQUATORIAL_RADIUS 6378137
#define PI 3.141592653589793238462643383
#define PI_2 1.57079632679

bool VehicleLocate::GetOffsets(Offsets_t *offsets, double *x, double *y)
{
	if (offsets->size < 4)
	{
		*x = 0;
		*y = 0;
		return false;
	}

	*x = (int16_t)((offsets->buf[0] << 8) + offsets->buf[1]) / 10.0;
	*y = (int16_t)((offsets->buf[2] << 8) + offsets->buf[3]) / 10.0;

	return true;
}

WGS84Point* VehicleLocate::GetPointArray(Offsets_t **nodes, int nodesLength, Position3D *nodesAnchor)
{
	WGS84Point *points = (WGS84Point *)calloc(nodesLength, sizeof(WGS84Point));

	double baseLatitude = nodesAnchor->lat / 10000000.0;
	double baseLongitude = nodesAnchor->Long / 10000000.0;

	double totalXOffset = 0.0;
	double totalYOffset = 0.0;
	double xOffset;
	double yOffset;

	for (int i = 0; i < nodesLength; i++)
	{
		GetOffsets(nodes[i], &xOffset, &yOffset);
		//std::cout << "xOffset " << xOffset << ", yOffset " << yOffset << std::endl;
		totalXOffset += xOffset;
		totalYOffset += yOffset;
		points[i].Latitude = baseLatitude + (totalYOffset * 360.0 / (2 * PI * DSRC_POLAR_RADIUS));
		points[i].Longitude = baseLongitude + (totalXOffset * 360.0 / (2 * PI * DSRC_EQUATORIAL_RADIUS * cos(baseLatitude * PI / 180)));
	}

	return points;
}

int VehicleLocate::FindRegion(TravelerInformation *tim, WGS84Point point, uint16_t heading)
{
	for (int i = 0; i < tim->dataFrames.list.count; i++)
	{
		TiDataFrame *frame = tim->dataFrames.list.array[i];

		int regionNumber = FindRegion(frame, point, heading);
		if (regionNumber != -1)
			return regionNumber;
	}
	return -1;
}

int VehicleLocate::FindRegion(TiDataFrame *frame, WGS84Point point, uint16_t heading)
{
	for (int i = 0; i < frame->regions.list.count; i++)
	{
		ValidRegion *region = frame->regions.list.array[i];

		Position3D *nodesAnchor = region->area.choice.shapePointSet.anchor;
		long laneWidth = *(region->area.choice.shapePointSet.laneWidth);

		Offsets_t **nodes = region->area.choice.shapePointSet.nodeList.list.array;
		int nodesLength = region->area.choice.shapePointSet.nodeList.list.count;

		WGS84Point* points = GetPointArray(nodes, nodesLength, nodesAnchor);

		if (IsInPointList(points, nodesLength, point, heading, laneWidth/100.0))
			return i + 1;

		free(points);
	}

	return -1;
}

bool VehicleLocate::IsInPointList(WGS84Point* points, int pointsLength, WGS84Point point, uint16_t heading, double laneWidth)
{
	for (int i = 1; i < pointsLength; i++)
	{
		double pointsHeading = Conversions::GetBearingDegrees(points[i], points[i-1]);

		//std::cout << "CSW IsInPointList - Vehicle Heading: " << heading << ", Points Heading: " << pointsHeading << std::endl;

		// Continue if the two headings are not within +-90 degrees of each other.
		double headingDiff = abs(heading - pointsHeading);
		if (headingDiff > 90 && headingDiff < 270)
			continue;

		double laneDist_m = Conversions::DistanceMeters(points[i], points[i-1]);
		double dist1 = Conversions::DistanceMeters(points[i], point);
		double dist2 = Conversions::DistanceMeters(points[i-1], point);

		//std::cout << "CSW IsInPointList - laneDist_m: " << laneDist_m << ", dist1: " << dist1 << ", dist2: " << dist2 << std::endl;

		if ((laneDist_m > dist1) && (laneDist_m > dist2))
		{
			double s = (.5) * (laneDist_m + dist1 + dist2);
			double area = sqrt((s * (s - dist1) * (s - dist2) * (s - laneDist_m)));

			double dist_perp = 2 * (area / laneDist_m);

			//std::cout << "CSW IsInPointList - laneWidth: " << laneWidth << ", dist_perp: " << dist_perp << std::endl;

			if (laneWidth / 2 > dist_perp)
			{
				return true;
			}
		}
	}

	return false;
}

void VehicleLocate::GetInt16(unsigned char *buf, int16_t *value)
{
	*value = (int16_t)((buf[0] << 8) + buf[1]);
}

void VehicleLocate::GetUInt16(unsigned char *buf, uint16_t *value)
{
	*value = (uint16_t)((buf[0] << 8) + buf[1]);
}

void VehicleLocate::GetInt32(unsigned char *buf, int32_t *value)
{
	*value = (int32_t)((buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3]);
}

void VehicleLocate::GetUInt32(unsigned char *buf, uint32_t *value)
{
	*value = (uint32_t)((buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3]);
}

// Process a BSM.
// Extract and return in the parameters:
// - regionNumber: The number of the TIM region the vehicle is within or 0 if not in a region.
// - speed_mph: The current speed of the vehicle.
// - vehicleId: The ID of the vehicle.
// Returns true on success; false if the BSM is invalid or other error occurs.
bool VehicleLocate::ProcessBsmMessage(IvpMessage *msg, TravelerInformation *tim, int* regionNumber, float* speed_mph, int32_t *vehicleId)
{
	bool isSuccess = false;
	uint8_t asnbuf[2000];

	int payloadLength = strlen(msg->payload->valuestring)/2;
	if (payloadLength <= (int)sizeof(asnbuf))
	{
		int i;
		for (i = 0; i < payloadLength; i++)
			sscanf(msg->payload->valuestring + (2 * i), "%2hhx", &asnbuf[i]);

		BasicSafetyMessage_t *bsm = NULL;
		asn_dec_rval_t rval = ber_decode(NULL, &asn_DEF_BasicSafetyMessage, (void **)&bsm, asnbuf, payloadLength);
		if (rval.code == RC_OK && bsm)
		{
			//asn_fprint(stdout, &asn_DEF_BasicSafetyMessage, bsm);

			int32_t latitude;
			int32_t longitude;
			uint16_t transmissionAndSpeed;
			uint16_t rawHeading;
			GetInt32(bsm->blob1.buf+1, vehicleId);
			GetInt32(bsm->blob1.buf+7, &latitude);
			GetInt32(bsm->blob1.buf+11, &longitude);
			GetUInt16(bsm->blob1.buf+21, &transmissionAndSpeed);
			GetUInt16(bsm->blob1.buf+23, &rawHeading);

			WGS84Point vehiclePoint;
			vehiclePoint.Latitude = (double)latitude/10000000;
			vehiclePoint.Longitude = (double)longitude/10000000;

			// Heading units are 0.0125 degrees.
			float heading = rawHeading / 80.0;

			// The speed is contained in bits 0-12.  Units are 0.02 meters/sec.
			// A value of 8191 is used when the speed is not known.
			uint16_t rawSpeed = (uint16_t)(transmissionAndSpeed & 0x1FFF);

			if (rawSpeed != 8191)
			{
				// Convert from .02 meters/sec to mph.
				*speed_mph = rawSpeed / 50 * 2.2369362920544;

				//std::cout << "Vehicle Lat/Long/Heading/Speed: " << vehiclePoint.Latitude << ", " << vehiclePoint.Longitude << ", " << heading << ", " << speed << std::endl;

				*regionNumber = FindRegion(tim, vehiclePoint, heading);

				if (*regionNumber < 0)
					*regionNumber = 0;

				//std::cout << "Vehicle Speed: " << *speed_mph << ", Region: " << *regionNumber << std::endl;

				isSuccess = true;
			}
			ASN_STRUCT_FREE(asn_DEF_BasicSafetyMessage, bsm);
		}
	}

	return isSuccess;
}

/*
int VehicleLocate::ProcessBsmMessage(IvpMessage *msg, TravelerInformation *tim, unsigned int speedLimit, int32_t *vehicleId)
{
	uint8_t asnbuf[2000];

	int payloadLength = strlen(msg->payload->valuestring)/2;
	if (payloadLength <= (int)sizeof(asnbuf))
	{
		int i;
		for (i = 0; i < payloadLength; i++)
			sscanf(msg->payload->valuestring + (2 * i), "%2hhx", &asnbuf[i]);

		BasicSafetyMessage_t *bsm = NULL;
		asn_dec_rval_t rval = ber_decode(NULL, &asn_DEF_BasicSafetyMessage, (void **)&bsm, asnbuf, payloadLength);
		if (rval.code == RC_OK && bsm)
		{
			//asn_fprint(stdout, &asn_DEF_BasicSafetyMessage, bsm);

			int32_t latitude;
			int32_t longitude;
			uint16_t transmissionAndSpeed;
			uint16_t rawHeading;
			GetInt32(bsm->blob1.buf+1, vehicleId);
			GetInt32(bsm->blob1.buf+7, &latitude);
			GetInt32(bsm->blob1.buf+11, &longitude);
			GetUInt16(bsm->blob1.buf+21, &transmissionAndSpeed);
			GetUInt16(bsm->blob1.buf+23, &rawHeading);

			WGS84Point vehiclePoint;
			vehiclePoint.Latitude = (double)latitude/10000000;
			vehiclePoint.Longitude = (double)longitude/10000000;

			// Heading units are 0.0125 degrees.
			float heading = rawHeading / 80.0;

			// The speed is contained in bits 0-12.  Units are 0.02 meters/sec.
			// A value of 8191 is used when the speed is not known.
			uint16_t rawSpeed = (uint16_t)(transmissionAndSpeed & 0x1FFF);

			if (rawSpeed != 8191)
			{
				// Convert from .02 meters/sec to mph.
				float speed = rawSpeed / 50 * 2.2369362920544;

				//std::cout << "Vehicle Lat/Long/Heading/Speed: " << vehiclePoint.Latitude << ", " << vehiclePoint.Longitude << ", " << heading << ", " << speed << std::endl;

				int regionNumber = FindRegion(tim, vehiclePoint, heading);

				//if (regionNumber != -1 && speed > speedLimit)
				//	std::cout << "Vehicle is speeding (" << speed << " > " << speedLimit << ") in region " << regionNumber << std::endl;

				if (regionNumber == 1 && speed > speedLimit)
				{
					return 1;
				}
				else if (regionNumber == 2 && speed > speedLimit)
				{
					return 2;
				}
				else if(regionNumber == 3 && speed > speedLimit)
				{
					return 3;
				}
				else if(regionNumber == 4 && speed > speedLimit)
				{
					return 4;
				}

				//std::cout << "Return 0.  Speed: " << speed << ", Speed Limit: " << speedLimit << std::endl;

				return 0;
			}
			ASN_STRUCT_FREE(asn_DEF_BasicSafetyMessage, bsm);
		}
	}

	//std::cout << "Return 0.  Invalid BSM?" << std::endl;
	return 0;
}
*/
