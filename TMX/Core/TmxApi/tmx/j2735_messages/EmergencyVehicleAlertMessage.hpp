/*
 * @file EmergencyVehicleAlertMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_EMERGENCYVEHICLEALERTMESSAGE_HPP_
#define TMX_J2735_MESSAGES_EMERGENCYVEHICLEALERTMESSAGE_HPP_

#include <asn_j2735_r41/EmergencyVehicleAlert.h>
#include <tmx/messages/TmxJ2735.hpp>

#define EVA_MESSAGETYPE "EVA"

namespace tmx {
namespace messages {

typedef TmxJ2735Message<EmergencyVehicleAlert> EvaMessage;
typedef TmxJ2735EncodedMessage<EvaMessage> EvaEncodedMessage;

template <>
constexpr asn_TYPE_descriptor_t *EvaMessage::get_descriptor()
{
	return &asn_DEF_EmergencyVehicleAlert;
}

template <>
constexpr const char *EvaMessage::get_messageType()
{
	return EVA_MESSAGETYPE;
}

template <>
constexpr int EvaMessage::get_default_msgId()
{
	return DSRCmsgID_emergencyVehicleAlert;
}

} /* End namespace message */

// Template specializations for handling encoding and decoding the message

template <> template <>
inline messages::EvaMessage routeable_message::get_payload<messages::EvaMessage>()
{
	messages::EvaEncodedMessage encMsg(*this);
	return encMsg.get_payload<messages::EvaMessage>();
}

} /* End namespace tmx */

#endif /* TMX_J2735_MESSAGES_EMERGENCYVEHICLEALERTMESSAGE_HPP_ */
