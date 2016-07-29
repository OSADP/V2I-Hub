/*
 * @file BsmMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_BASICSAFETYMESSAGE_HPP_
#define TMX_J2735_MESSAGES_BASICSAFETYMESSAGE_HPP_

#include <asn_j2735_r41/BasicSafetyMessage.h>
#include <tmx/messages/TmxJ2735.hpp>

#define BSM_MESSAGETYPE "BSM"

namespace tmx {
namespace messages {

typedef TmxJ2735Message<BasicSafetyMessage> BsmMessage;
typedef TmxJ2735EncodedMessage<BsmMessage> BsmEncodedMessage;

template <>
constexpr asn_TYPE_descriptor_t *BsmMessage::get_descriptor()
{
	return &asn_DEF_BasicSafetyMessage;
}

template <>
constexpr const char *BsmMessage::get_messageType()
{
	return BSM_MESSAGETYPE;
}

template <>
constexpr int BsmMessage::get_default_msgId()
{
	return DSRCmsgID_basicSafetyMessage;
}

} /* End namespace message */

// Template specializations for handling encoding and decoding the message

template <> template <>
inline messages::BsmMessage routeable_message::get_payload<messages::BsmMessage>()
{
	messages::BsmEncodedMessage encMsg(*this);
	return encMsg.get_payload<messages::BsmMessage>();
}

} /* End namespace tmx */



#endif /* TMX_J2735_MESSAGES_BASICSAFETYMESSAGE_HPP_ */
