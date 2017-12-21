/*
 * PmmDocument.h
 *
 *  Created on: Nov 17, 2016
 *      Author: ivp
 */

#ifndef PMMDOCUMENT_H_
#define PMMDOCUMENT_H_

#include <vector>
#include <PersonalMobilityMessage.h>
#include "WGS84Point.h"
#include "XmlDocument.h"

namespace tmx
{
namespace utils
{

struct MobilityNeed
{
	std::string Type;
	int Count;
};

/**
 * Creates a PMM from a base XML string for manipulation using xpath.
 *
 * Note that tmx/messages/message_document.hpp could not be used because std11 is not supported on MIPS.
 */
class PmmDocument : public XmlDocument
{
public:
	PmmDocument();
	virtual ~PmmDocument();

	void set_GroupId(const std::string groupId);

	void set_RequestId(const std::string requestId);

	void set_Status(const std::string status);

	void set_Position(int latitude, int longitude);

	void set_RequestDate(int year, const int month, int day, int hour, int minute);

	void set_PickupDate(int year, const int month, int day, int hour, int minute);

	void set_Destination(int latitude, int longitude);

	void set_MobilityNeeds(std::vector<MobilityNeed> mobilityNeeds);

	void set_ModeOfTransport(const std::string modeOfTransport);

	void set_Eta(int eta);

	void set_IsDsrcEquipped(bool isDSRCEquipped);

	void set_VehicleDesc(std::string vehicleDesc);
};

} /* namespace utils */
} /* namespace tmx */

#endif /* PMMDOCUMENT_H_ */
