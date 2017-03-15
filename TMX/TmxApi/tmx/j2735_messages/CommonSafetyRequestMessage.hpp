/*
 * @file CommonSafetyRequestMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_COMMONSAFETYREQUESTMESSAGE_HPP_
#define TMX_J2735_MESSAGES_COMMONSAFETYREQUESTMESSAGE_HPP_

#include <asn_j2735_r41/CommonSafetyRequest.h>
#include <tmx/messages/TmxJ2735.hpp>

namespace tmx {
namespace messages {

typedef TmxJ2735Message<CommonSafetyRequest> CsrMessage;
typedef TmxJ2735EncodedMessage<CsrMessage> CsrEncodedMessage;

template <>
constexpr asn_TYPE_descriptor_t *CsrMessage::get_descriptor()
{
	return &asn_DEF_CommonSafetyRequest;
}

template <>
constexpr const char *CsrMessage::get_messageType()
{
	return api::MSGSUBTYPE_COMMONSAFETYREQUEST_STRING;
}

template <>
constexpr int CsrMessage::get_default_msgId()
{
	return (int)api::commonSafetyRequest_D; // DSRCmsgID_travelerInformation;
}

} /* End namespace message */

// Template specializations for handling encoding and decoding the message

template <> template <>
inline messages::CsrMessage routeable_message::get_payload<messages::CsrMessage>()
{
	messages::CsrEncodedMessage encMsg(*this);
	return encMsg.get_payload<messages::CsrMessage>();
}

} /* End namespace tmx */

#endif /* TMX_J2735_MESSAGES_COMMONSAFETYREQUESTMESSAGE_HPP_ */
