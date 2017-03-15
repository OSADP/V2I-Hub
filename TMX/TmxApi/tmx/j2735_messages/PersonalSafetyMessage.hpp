/*
 * PersonalSafetyMessage.hpp
 *
 *  Created on: Sep 15, 2016
 *      Author: gmb
 */

#ifndef TMX_J2735_MESSAGES_PERSONALSAFETYMESSAGE_HPP_
#define TMX_J2735_MESSAGES_PERSONALSAFETYMESSAGE_HPP_

#include <asn_j2735_r41/PersonalSafetyMessage.h>
#include <tmx/messages/TmxJ2735.hpp>

namespace tmx {
namespace messages {

typedef TmxJ2735Message<PersonalSafetyMessage> PsmMessage;
typedef TmxJ2735EncodedMessage<PsmMessage> PsmEncodedMessage;

template <>
constexpr asn_TYPE_descriptor_t *PsmMessage::get_descriptor()
{
	return &asn_DEF_PersonalSafetyMessage;
}

template <>
constexpr const char *PsmMessage::get_messageType()
{
	return api::MSGSUBTYPE_PERSONALSAFETYMESSAGE_STRING;
}

template <>
constexpr int PsmMessage::get_default_msgId()
{
	return (int)api::personalSafetyMessage;
}

} /* End namespace messages */

// Template specializations for handling encoding and decoding the message

template <> template <>
inline messages::PsmMessage routeable_message::get_payload<messages::PsmMessage>()
{
	messages::PsmEncodedMessage encMsg(*this);
	return encMsg.get_payload<messages::PsmMessage>();
}

} /* End namespace tmx */

#endif /* TMX_J2735_MESSAGES_PERSONALSAFETYMESSAGE_HPP_ */
