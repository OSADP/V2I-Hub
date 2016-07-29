/*
 * DsrcBuilder.cpp
 *
 *  Created on: Sep 25, 2014
 *      Author: ivp
 */

#include <string>
#include <sstream>
#include "DsrcBuilder.h"
#include "TimeHelper.h"

void DsrcBuilder::AddCurveSpeedAdvisory(TiDataFrame *frame, unsigned int speedLimit)
{
	frame->content.present = content_PR_advisory;

	std::stringstream speedText;
	speedText << speedLimit << " MPH";

	AddItisCode(&frame->content.choice.advisory.list, 27); // "warning advice"
	AddItisText(&frame->content.choice.advisory.list, "curve ahead");
	AddItisCode(&frame->content.choice.advisory.list, 2564); // "speed restriction"
	AddItisText(&frame->content.choice.advisory.list, speedText.str());
}

void DsrcBuilder::AddItisCode(ITIScodesAndText::ITIScodesAndText__List *list, long code)
{
	ItisMember *itisMember = (ItisMember*)calloc(1, sizeof(ItisMember));
	itisMember->item_itis.present = item_itis_PR_itis;
	itisMember->item_itis.choice.itis = code;
	asn_set_add(list, itisMember);
}

void DsrcBuilder::AddItisText(ITIScodesAndText::ITIScodesAndText__List *list, std::string text)
{
	int textLength = text.length() + 1;

	ItisMember *itisMember = (ItisMember*)calloc(1, sizeof(ItisMember));
	itisMember->item_itis.present = item_itis_PR_itis;
	itisMember->item_itis.choice.text.buf = (uint8_t*)calloc(textLength, sizeof(uint8_t));
	itisMember->item_itis.choice.text.size = textLength;

	for (int i = 0; i < textLength - 1; i++)
	{
		itisMember->item_itis.choice.text.buf[i] = text[i];
	}

	itisMember->item_itis.choice.text.buf[textLength - 1] = '\0';

	asn_set_add(list, itisMember);
}

void DsrcBuilder::SetPacketId(TravelerInformation *tim)
{
	// Allocate memory if the packet ID has never been set before.
	if (tim->packetID == NULL)
	{
		tim->packetID = (UniqueMSGID_t *)calloc(1, sizeof(UniqueMSGID_t));
		tim->packetID->buf = (uint8_t *)calloc(4, sizeof(uint8_t));
		tim->packetID->size = 4 * sizeof(uint8_t);
	}

	// Recommended packet ID is Agency ID in the first byte and Publish time of packet (MinuteOfTheYear) in bytes 2-4.
	// Don't know what Agency ID to use.  Specify 0 for now.

	uint32_t minuteOfYear = TimeHelper::GetMinuteOfYear();

	tim->packetID->buf[0] = 0x00;
	tim->packetID->buf[1] = (minuteOfYear & 0xFF0000) >> 16;
	tim->packetID->buf[2] = (minuteOfYear & 0x00FF00) >> 8;
	tim->packetID->buf[3] = minuteOfYear & 0x0000FF;
}

void DsrcBuilder::SetStartTimeToYesterday(TiDataFrame *frame)
{
	// Set the start time (minutes of the year) to the start of yesterday.
	int dayOfYear = TimeHelper::GetDayOfYear() - 1;
	if (dayOfYear < 0)
		dayOfYear = 364;
	frame->startTime = dayOfYear * 24 * 60;
}
