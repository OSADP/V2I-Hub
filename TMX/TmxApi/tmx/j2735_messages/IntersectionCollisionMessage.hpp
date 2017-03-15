/*
 * @file IntersectionCollisionMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_INTERSECTIONCOLLISIONMESSAGE_HPP_
#define TMX_J2735_MESSAGES_INTERSECTIONCOLLISIONMESSAGE_HPP_

#include <asn_j2735_r41/IntersectionCollision.h>
#include <tmx/messages/TmxJ2735.hpp>

namespace tmx {
namespace messages {

typedef TmxJ2735Message<IntersectionCollision> IntersectionCollisionMessage;
typedef TmxJ2735EncodedMessage<IntersectionCollisionMessage> IntersectionCollisionEncodedMessage;

template <>
constexpr asn_TYPE_descriptor_t *IntersectionCollisionMessage::get_descriptor()
{
	return &asn_DEF_IntersectionCollision;
}

template <>
constexpr const char *IntersectionCollisionMessage::get_messageType()
{
	return api::MSGSUBTYPE_INTERSECTIONCOLLISION_STRING;
}

template <>
constexpr int IntersectionCollisionMessage::get_default_msgId()
{
	return (int)api::intersectionCollision_D; // DSRCmsgID_intersectionCollisionAlert;
}

} /* End namespace message */

// Template specializations for handling encoding and decoding the BsmMessage

template <> template <>
inline messages::IntersectionCollisionMessage routeable_message::get_payload<messages::IntersectionCollisionMessage>()
{
	messages::IntersectionCollisionEncodedMessage encMsg(*this);
	return encMsg.get_payload<messages::IntersectionCollisionMessage>();
}

} /* End namespace tmx */

#endif /* TMX_J2735_MESSAGES_INTERSECTIONCOLLISIONMESSAGE_HPP_ */
