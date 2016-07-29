/*
 * PedestrianMessage.h
 *
 *  Created on: May 25, 2016
 *      Author: ivp
 */

#ifndef INCLUDE_PEDESTRIANMESSAGE_H_
#define INCLUDE_PEDESTRIANMESSAGE_H_


#include <tmx/messages/message.hpp>

namespace tmx {
namespace messages {


/**
 * LocationMessage is the message type used to send information messages about plugin status/activities.
 * It defines the message type and sub type and all data members.
 */
class PedestrianMessage : public tmx::message {
public:
	PedestrianMessage() {}
	PedestrianMessage(const tmx::message_container_type &contents): tmx::message(contents) {}

	/// Message type for routing this message through TMX core.
	static constexpr const char* MessageType = "Application";

	/// Message sub type for routing this message through TMX core.
	static constexpr const char* MessageSubType = "PedestrianDetection";

	/**
	 * DetectionZones.
	 */
	std_attribute(this->msg, std::string, DetectionZones, "", )
};

} /* namespace messages */
} /* namespace tmx */


#endif /* INCLUDE_PEDESTRIANMESSAGE_H_ */

