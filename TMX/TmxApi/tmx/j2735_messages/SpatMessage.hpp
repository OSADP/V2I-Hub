/*
 * @file SpatMessage.hpp
 *
 *  Created on: Apr 28, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_SPATMESSAGE_HPP_
#define TMX_J2735_MESSAGES_SPATMESSAGE_HPP_

#include <asn_j2735_r41/SPAT.h>
#include <tmx/messages/TmxJ2735.hpp>

#define SPAT_UPERFRAME_MSGID 0x11
#define SPAT_MSGID 0x13

namespace tmx {
namespace messages {

typedef TmxJ2735Message<SPAT> SpatMessage;
typedef TmxJ2735EncodedMessage<SpatMessage> SpatEncodedMessage;

template <>
constexpr asn_TYPE_descriptor_t *SpatMessage::get_descriptor()
{
	return &asn_DEF_SPAT;
}

template <>
constexpr const char *SpatMessage::get_messageType()
{
	return api::MSGSUBTYPE_SIGNALPHASEANDTIMINGMESSAGE_STRING;
}

template <>
constexpr int SpatMessage::get_default_msgId()
{
	return (int)api::signalPhaseAndTimingMessage;
}

} /* End namespace messages */

// Template overrides for handling encoding and decoding the SpatMessage

template <> template<>
inline messages::SpatMessage routeable_message::get_payload<messages::SpatMessage>()
{
	messages::SpatEncodedMessage encMsg(*this);
	return encMsg.get_payload<messages::SpatMessage>();
}

} /* End namespace tmx */


#endif /* TMX_J2735_MESSAGES_SPATMESSAGE_HPP_ */
