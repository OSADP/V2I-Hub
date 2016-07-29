/*
 * DsrcBuilder.h
 *
 *  Created on: Sep 25, 2014
 *      Author: ivp
 */

#ifndef DSRCBUILDER_H_
#define DSRCBUILDER_H_

#include <string>
#include <stdexcept>

#include <asn_j2735_r41/TravelerInformation.h>

typedef TravelerInformation::TravelerInformation__dataFrames::TravelerInformation__dataFrames__List::TravelerInformation__dataFrames__Member TiDataFrame;
typedef ITIScodesAndText::ITIScodesAndText__List::ITIScodesAndText__Member ItisMember;

class DsrcBuilder
{
public:
	static void AddCurveSpeedAdvisory(TiDataFrame *frame, unsigned int speedLimit);
	static void AddItisCode(ITIScodesAndText::ITIScodesAndText__List *list, long code);
	static void AddItisText(ITIScodesAndText::ITIScodesAndText__List *list, std::string text);
	static void SetPacketId(TravelerInformation *tim);
	static void SetStartTimeToYesterday(TiDataFrame *frame);
};

#endif /* DSRCBUILDER_H_ */
