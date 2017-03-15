/*
 * @file TravelerInformationMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_TRAVELERINFORMATIONMESSAGE_HPP_
#define TMX_J2735_MESSAGES_TRAVELERINFORMATIONMESSAGE_HPP_

#include <asn_j2735_r41/TravelerInformation.h>
#include <tmx/messages/TmxJ2735.hpp>

namespace tmx {
namespace messages {

typedef TmxJ2735Message<TravelerInformation> TimMessage;
typedef TmxJ2735EncodedMessage<TimMessage> TimEncodedMessage;

template <>
constexpr asn_TYPE_descriptor_t *TimMessage::get_descriptor()
{
	return &asn_DEF_TravelerInformation;
}

template <>
constexpr const char *TimMessage::get_messageType()
{
	return api::MSGSUBTYPE_TRAVELERINFORMATION_STRING;
}

template <>
constexpr int TimMessage::get_default_msgId()
{
	return (int)api::travelerInformation_D; // DSRCmsgID_travelerInformation;
}

} /* End namespace message */

// Template specializations for handling encoding and decoding the BsmMessage

template <> template <>
inline messages::TimMessage routeable_message::get_payload<messages::TimMessage>()
{
	messages::TimEncodedMessage encMsg(*this);
	return encMsg.get_payload<messages::TimMessage>();
}

} /* End namespace tmx */

#endif /* TMX_J2735_MESSAGES_TRAVELERINFORMATIONMESSAGE_HPP_ */
