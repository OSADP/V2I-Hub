/*
 * @file RoadSideAlertMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_ROADSIDEALERTMESSAGE_HPP_
#define TMX_J2735_MESSAGES_ROADSIDEALERTMESSAGE_HPP_

#include <asn_j2735_r41/RoadSideAlert.h>
#include <tmx/messages/TmxJ2735.hpp>

#define RSA_MESSAGETYPE "RSA"

namespace tmx {
namespace messages {

typedef TmxJ2735Message<RoadSideAlert> RsaMessage;
typedef TmxJ2735EncodedMessage<RsaMessage> RsaEncodedMessage;

template <>
constexpr asn_TYPE_descriptor_t *RsaMessage::get_descriptor()
{
	return &asn_DEF_RoadSideAlert;
}

template <>
constexpr const char *RsaMessage::get_messageType()
{
	return RSA_MESSAGETYPE;
}

template <>
constexpr int RsaMessage::get_default_msgId()
{
	return DSRCmsgID_roadSideAlert;
}

} /* End namespace message */

// Template specializations for handling encoding and decoding the message

template <> template <>
inline messages::RsaMessage routeable_message::get_payload<messages::RsaMessage>()
{
	messages::RsaEncodedMessage encMsg(*this);
	return encMsg.get_payload<messages::RsaMessage>();
}

} /* End namespace tmx */

#endif /* TMX_J2735_MESSAGES_ROADSIDEALERTMESSAGE_HPP_ */
