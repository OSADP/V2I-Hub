/*
 * VehicleBasicMessage.h
 *
 *  Created on: Apr 8, 2016
 *      Author: ivp
 */

#ifndef INCLUDE__VEHICLEBASICMESSAGE_H_
#define INCLUDE__VEHICLEBASICMESSAGE_H_

#include <tmx/messages/message.hpp>
#include "VehicleParameterTypes.h"

namespace tmx {
namespace messages {

/**
 * VehicleBasicMessage is the message type used to transmit information from the vehicle through TMX core.
 * It defines the message type and sub type and all data members.
 */
class VehicleBasicMessage : public tmx::message
{
public:
	VehicleBasicMessage() {}
	VehicleBasicMessage(const tmx::message_container_type &contents): tmx::message(contents) {}

	/// Message type for routing this message through TMX core.
	static constexpr const char* MessageType = "Vehicle";

	/// Message sub type for routing this message through TMX core.
	static constexpr const char* MessageSubType = "Basic";

	/// The gear shift position.
	std_attribute(this->msg, vehicleparam::GearState, GearPosition, vehicleparam::GearState::Park, )

	/// Indicates whether the brake is currently applied.
	std_attribute(this->msg, bool, BrakeApplied, false, )

	/// The speed of the vehicle in MPH.
	std_attribute(this->msg, int, SpeedMph, 0, )

	/// The turn signal position.
	std_attribute(this->msg, vehicleparam::TurnSignalState, TurnSignalPosition, vehicleparam::TurnSignalState::Off, )
};

} /* namespace messages */
} /* namespace tmx */

#endif /* INCLUDE__VEHICLEBASICMESSAGE_H_ */
