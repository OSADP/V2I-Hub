/*
 * @file RtcmMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_RTCMMESSAGE_HPP_
#define TMX_J2735_MESSAGES_RTCMMESSAGE_HPP_

#include <asn_j2735_r41/RTCM-Corrections.h>
#include <tmx/messages/TmxJ2735.hpp>

#define RTCM_MESSAGETYPE "RTCM"

namespace tmx {
namespace messages {

typedef TmxJ2735Message<RTCM_Corrections> RtcmMessage;
typedef TmxJ2735EncodedMessage<RtcmMessage> RtcmEncodedMessage;

template <>
constexpr asn_TYPE_descriptor_t *RtcmMessage::get_descriptor()
{
	return &asn_DEF_RTCM_Corrections;
}

template <>
constexpr const char *RtcmMessage::get_messageType()
{
	return RTCM_MESSAGETYPE;
}

template <>
constexpr int RtcmMessage::get_default_msgId()
{
	return DSRCmsgID_rtcmCorrections;
}

} /* End namespace message */

// Template specializations for handling encoding and decoding the BsmMessage

template <> template <>
inline messages::RtcmMessage routeable_message::get_payload<messages::RtcmMessage>()
{
	messages::RtcmEncodedMessage encMsg(*this);
	return encMsg.get_payload<messages::RtcmMessage>();
}

} /* End namespace tmx */


#endif /* TMX_J2735_MESSAGES_RTCMMESSAGE_HPP_ */
