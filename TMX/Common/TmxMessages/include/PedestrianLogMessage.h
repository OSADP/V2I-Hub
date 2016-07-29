/*
 * PedestrianLogMessage.h
 *
 *  Created on: Jul 25, 2016
 *      Author: ivp
 */

#ifndef INCLUDE_PEDESTRIANLOGMESSAGE_H_
#define INCLUDE_PEDESTRIANLOGMESSAGE_H_


#include <tmx/messages/message.hpp>
#include "VehicleParameterTypes.h"

namespace tmx {
namespace messages {

/**
 * ApplicationEventMessage is the message type used to send information messages about plugin status/activities.
 * It defines the message type and sub type and all data members.
 */
class PedestrianLogMessage : public tmx::message
{
public:
	PedestrianLogMessage() {}
	PedestrianLogMessage(const tmx::message_container_type &contents): tmx::message(contents) {}
	PedestrianLogMessage(std::string id, std::string detectionTime, std::string type, int zoneID, int64_t eventNumber,
			std::string ipAddress) {
		set_Id(id);
		set_DetectionTime(detectionTime);
		set_Type(type);
		set_ZoneID(zoneID);
		set_EventNumber(eventNumber);
		set_IpAddress(ipAddress);
	}

	/// Message type for routing this message through TMX core.
	static constexpr const char* MessageType = "Application";

	/// Message sub type for routing this message through TMX core.
	static constexpr const char* MessageSubType = "PedestrianLog";

	std_attribute(this->msg, std::string, Id, "", )
	std_attribute(this->msg, std::string, DetectionTime, "", )
	std_attribute(this->msg, std::string, Type, "", )
	std_attribute(this->msg, int, ZoneID, 0, )
	std_attribute(this->msg, int64_t, EventNumber, 0, )
	std_attribute(this->msg, std::string, IpAddress, "", )
};

} /* namespace messages */
} /* namespace tmx */



#endif /* INCLUDE_PEDESTRIANLOGMESSAGE_H_ */
