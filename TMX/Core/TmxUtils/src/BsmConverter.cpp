/*
 * BsmConverter.cpp
 *
 *  Created on: Jun 10, 2016
 *      Author: ivp
 */

#include "BsmConverter.h"

namespace tmx {
namespace utils {

void BsmConverter::ToDecodedBsmMessage(BasicSafetyMessage_t &bsm, tmx::messages::DecodedBsmMessage &decoded)
{
	ToDecodedBsmMessage(bsm.blob1.buf, decoded);
}

void BsmConverter::ToDecodedBsmMessage(uint8_t *buf, tmx::messages::DecodedBsmMessage &decoded)
{
	decoded.set_IsLocationValid(false);
	decoded.set_IsElevationValid(false);
	decoded.set_IsSpeedValid(false);
	decoded.set_IsHeadingValid(false);
	decoded.set_IsSteeringWheelAngleValid(false);

	decoded.set_TemporaryId(ntohl(*(uint32_t*)(buf+1)));

	int32_t latitude = (int32_t)ntohl(*(uint32_t*)(buf+7));
	int32_t longitude = (int32_t)ntohl(*(uint32_t*)(buf+11));

	if (latitude != 900000001 && longitude != 1800000001)
	{
		decoded.set_Latitude((double)latitude/10000000);
		decoded.set_Longitude((double)longitude/10000000);
		decoded.set_IsLocationValid(true);
	}

	int16_t elevation = ntohs(*(uint16_t*)(buf+15));

	if (elevation != -4095)
	{
		decoded.set_Elevation_m((float)elevation/10.0);
		decoded.set_IsElevationValid(true);
	}

	uint16_t transmissionAndSpeed = ntohs(*(uint16_t*)(buf+21));

	// The speed is contained in bits 0-12.  Units are 0.02 meters/sec.
	// A value of 8191 is used when the speed is not known.
	uint16_t speed = (uint16_t)(transmissionAndSpeed & 0x1FFF);

	if (speed != 8191)
	{
		// Convert from .02 meters/sec to mph.
		decoded.set_Speed_mph((float)speed * 2.2369362920544 / 50);
		decoded.set_IsSpeedValid(true);
	}

	// Heading units are 0.0125 degrees.
	uint16_t heading = ntohs(*(uint16_t*)(buf+23));

	if (heading != 28800)
	{
		decoded.set_Heading(heading / 80.0);
		decoded.set_IsHeadingValid(true);
	}

	// Steering Wheel Angle units are 1.5 degrees.
	int16_t angle = *(int16_t*)(buf+25);

	if (angle != 127)
	{
		decoded.set_SteeringWheelAngle(angle * 1.5);
		decoded.set_IsSteeringWheelAngleValid(true);
	}
}

void BsmConverter::ToBasicSafetyMessage(tmx::messages::DecodedBsmMessage &decoded, BasicSafetyMessage_t &bsm)
{
	memset(&bsm, 0, sizeof(BasicSafetyMessage_t));

	bsm.msgID = DSRCmsgID_basicSafetyMessage;

	bsm.blob1.buf = (uint8_t *)calloc(1, 38);

	int8_t int8Temp;
	int16_t int16Temp;
	uint16_t uint16Temp;
	uint16_t uint16Temp2;
	uint32_t uint32Temp;
	uint32_t uint32Temp2;

	bsm.blob1.buf[0] = decoded.get_MsgCount();

	uint32Temp = htonl(decoded.get_TemporaryId());
	memcpy(bsm.blob1.buf + 1, &uint32Temp, 4);

	uint16Temp = htons(decoded.get_SecondMark());
	memcpy(bsm.blob1.buf + 5, &uint16Temp, 2);

	// Latitude and Longitude are expressed in 1/10th integer microdegrees, as a 31 bit value.
	// The value 900000001 indicates that latitude is not available.
	// The value 1800000001 indicates that longitude is not available.

	if (decoded.get_IsLocationValid())
		uint32Temp2 = (int32_t)(decoded.get_Latitude() * 10000000.0);
	else
		uint32Temp2 = 900000001;

	uint32Temp = htonl(uint32Temp2);
	memcpy(bsm.blob1.buf + 7,  &uint32Temp, 4);

	if (decoded.get_IsLocationValid())
		uint32Temp2 = (int32_t)(decoded.get_Longitude() * 10000000.0);
	else
		uint32Temp2 = 1800000001;

	uint32Temp = htonl(uint32Temp2);
	memcpy(bsm.blob1.buf + 11,  &uint32Temp, 4);

	// Elevation is in units of 10 cm steps.
	// max and min values are limited as set below.

	if (decoded.get_IsElevationValid())
	{
		if (decoded.get_Elevation_m() > 6143.9)
			int16Temp = 61439;
		else if (decoded.get_Elevation_m() < -409.5)
			int16Temp = -4095;
		int16Temp = decoded.get_Elevation_m() * 10.0;
	}
	else
		int16Temp = -4095;

	uint16Temp = htons(int16Temp);
	memcpy(bsm.blob1.buf + 15,  &uint16Temp, 2);

	// Positional Accuracy.
	// TODO: implement.
	uint32Temp = 0xFFFFFFFF;
	memcpy(bsm.blob1.buf + 17,  &uint32Temp, 4);

	// Speed and Transmission State.
	// TODO: implement transmission state.  currently set to 0.

	// Convert from mph to .02 meters/sec.
	if (decoded.get_IsSpeedValid())
		int16Temp = decoded.get_Speed_mph() * 50.0 / 2.2369362920544;
	else
		int16Temp = 8191;

	uint16Temp = htons(int16Temp);
	memcpy(bsm.blob1.buf + 21,  &uint16Temp, 2);

	// Heading units are 0.0125 degrees.
	if (decoded.get_IsHeadingValid())
	{
		if (decoded.get_Heading() > 359.9875)
			uint16Temp2 = 359.9875 * 80;
		else if (decoded.get_Heading() < 0)
			uint16Temp2 = 0;
		uint16Temp2 = decoded.get_Heading() * 80;
	}
	else
		uint16Temp2 = 28800;

	uint16Temp = htons(uint16Temp2);
	memcpy(bsm.blob1.buf + 23,  &uint16Temp, 2);

	// Steering Wheel Angle units are 1.5 degrees (-126 to 127).
	if (decoded.get_IsSteeringWheelAngleValid())
	{
		if (decoded.get_SteeringWheelAngle() > 189)
			int8Temp = 126;
		else if (decoded.get_SteeringWheelAngle() < -189)
			int8Temp = -126;
		int8Temp = decoded.get_SteeringWheelAngle() * 80;
	}
	else
		int8Temp = 127;

	memcpy(bsm.blob1.buf + 25,  &int8Temp, 1);

	// Acceleration Set 4 Way, Brake System Status, Vehicle Size.
	// TODO: implement.
	long longTemp = 0;
	uint32Temp = 0;
	memcpy(bsm.blob1.buf + 26,  &longTemp, 7);
	memcpy(bsm.blob1.buf + 33,  &uint32Temp, 2);
	memcpy(bsm.blob1.buf + 35,  &uint32Temp, 3);

	bsm.blob1.size = 38;
}

} /* namespace utils */
} /* namespace tmx */
