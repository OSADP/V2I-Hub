/*
 * @file SignalRequestMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_SIGNALREQUESTMESSAGE_HPP_
#define TMX_J2735_MESSAGES_SIGNALREQUESTMESSAGE_HPP_

#include <asn_j2735_r41/SignalRequestMsg.h>
#include <tmx/messages/TmxJ2735.hpp>

#define SRM_MESSAGETYPE "SRM"

namespace tmx {
namespace messages {

typedef TmxJ2735Message<SignalRequestMsg> SrmMessage;
typedef TmxJ2735EncodedMessage<SrmMessage> SrmEncodedMessage;

template <>
constexpr asn_TYPE_descriptor_t *SrmMessage::get_descriptor()
{
	return &asn_DEF_SignalRequestMsg;
}

template <>
constexpr const char *SrmMessage::get_messageType()
{
	return SRM_MESSAGETYPE;
}

template <>
constexpr int SrmMessage::get_default_msgId()
{
	return DSRCmsgID_signalRequestMessage;
}

} /* End namespace message */

// Template specializations for handling encoding and decoding the message

template <> template <>
inline messages::SrmMessage routeable_message::get_payload<messages::SrmMessage>()
{
	messages::SrmEncodedMessage encMsg(*this);
	return encMsg.get_payload<messages::SrmMessage>();
}

} /* End namespace tmx */

#endif /* TMX_J2735_MESSAGES_SIGNALREQUESTMESSAGE_HPP_ */
