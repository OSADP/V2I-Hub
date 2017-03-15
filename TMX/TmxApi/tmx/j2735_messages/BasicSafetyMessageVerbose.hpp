/*
 * @file BasicSafetyMessageVerbose.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_BASICSAFETYMESSAGEVERBOSE_HPP_
#define TMX_J2735_MESSAGES_BASICSAFETYMESSAGEVERBOSE_HPP_

#include <asn_j2735_r41/BasicSafetyMessageVerbose.h>
#include <tmx/messages/TmxJ2735.hpp>

namespace tmx {
namespace messages {

typedef TmxJ2735Message<BasicSafetyMessageVerbose> BsmvMessage;
typedef TmxJ2735EncodedMessage<BsmvMessage> BsmvEncodedMessage;

template <>
constexpr asn_TYPE_descriptor_t *BsmvMessage::get_descriptor()
{
	return &asn_DEF_BasicSafetyMessageVerbose;
}

template <>
constexpr const char *BsmvMessage::get_messageType()
{
	return api::MSGSUBTYPE_BASICSAFETYMESSAGEVERBOSE_D_STRING;
}

template <>
constexpr int BsmvMessage::get_default_msgId()
{
	return api::basicSafetyMessageVerbose_D; //DSRCmsgID_basicSafetyMessageVerbose;
}

} /* End namespace message */

// Template specializations for handling encoding and decoding the message

template <> template <>
inline messages::BsmvMessage routeable_message::get_payload<messages::BsmvMessage>()
{
	messages::BsmvEncodedMessage encMsg(*this);
	return encMsg.get_payload<messages::BsmvMessage>();
}

} /* End namespace tmx */

#endif /* TMX_J2735_MESSAGES_BASICSAFETYMESSAGEVERBOSE_HPP_ */
