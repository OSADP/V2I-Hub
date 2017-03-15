/*
 * @file SignalStatusMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_SIGNALSTATUSMESSAGE_HPP_
#define TMX_J2735_MESSAGES_SIGNALSTATUSMESSAGE_HPP_

#include <asn_j2735_r41/SignalStatusMessage.h>
#include <tmx/messages/TmxJ2735.hpp>

namespace tmx {
namespace messages {

typedef TmxJ2735Message<SignalStatusMessage> SsmMessage;
typedef TmxJ2735EncodedMessage<SsmMessage> SsmEncodedMessage;

template <>
constexpr asn_TYPE_descriptor_t *SsmMessage::get_descriptor()
{
	return &asn_DEF_SignalStatusMessage;
}

template <>
constexpr const char *SsmMessage::get_messageType()
{
	return api::MSGSUBTYPE_SIGNALSTATUSMESSAGE_STRING;
}

template <>
constexpr int SsmMessage::get_default_msgId()
{
	return (int)api::signalStatusMessage_D; // DSRCmsgID_signalStatusMessage;
}

} /* End namespace message */

// Template specializations for handling encoding and decoding the BsmMessage

template <> template <>
inline messages::SsmMessage routeable_message::get_payload<messages::SsmMessage>()
{
	messages::SsmEncodedMessage encMsg(*this);
	return encMsg.get_payload<messages::SsmMessage>();
}

} /* End namespace tmx */

#endif /* TMX_J2735_MESSAGES_SIGNALSTATUSMESSAGE_HPP_ */
