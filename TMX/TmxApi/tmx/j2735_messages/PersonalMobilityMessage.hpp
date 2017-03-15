/*
 * PersonalMobilityMessage.hpp
 *
 *  Created on: Sep 15, 2016
 *      Author: gmb
 */

#ifndef TMX_J2735_MESSAGES_PERSONALMOBILITYMESSAGE_HPP_
#define TMX_J2735_MESSAGES_PERSONALMOBILITYMESSAGE_HPP_

#include <asn_j2735_r41/PersonalMobilityMessage.h>
#include <tmx/messages/TmxJ2735.hpp>

namespace tmx {
namespace messages {

typedef TmxJ2735Message<PersonalMobilityMessage> PmmMessage;
typedef TmxJ2735EncodedMessage<PmmMessage> PmmEncodedMessage;

template <>
constexpr asn_TYPE_descriptor_t *PmmMessage::get_descriptor()
{
	return &asn_DEF_PersonalMobilityMessage;
}

template <>
constexpr const char *PmmMessage::get_messageType()
{
	return api::MSGSUBTYPE_PERSONALMOBILITYMESSAGE_STRING;
}

template <>
constexpr int PmmMessage::get_default_msgId()
{
	return (int)api::personalMobilityMessage;
}

} /* End namespace messages */

// Template specializations for handling encoding and decoding the message

template <> template <>
inline messages::PmmMessage routeable_message::get_payload<messages::PmmMessage>()
{
	messages::PmmEncodedMessage encMsg(*this);
	return encMsg.get_payload<messages::PmmMessage>();
}

} /* End namespace tmx */

#endif /* TMX_J2735_MESSAGES_PERSONALMOBILITYMESSAGE_HPP_ */
