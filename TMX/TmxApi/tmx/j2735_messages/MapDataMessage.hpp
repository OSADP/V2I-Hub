/*
 * @file MapDataMessage.hpp
 *
 *  Created on: Apr 28, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_MAPDATAMESSAGE_HPP_
#define TMX_J2735_MESSAGES_MAPDATAMESSAGE_HPP_

#include <asn_j2735_r41/MapData.h>
#include <tmx/messages/TmxJ2735.hpp>

#define MAPDATA_UPERFRAME_MSGID 0x11
#define MAPDATA_MSGID 0x12

namespace tmx {
namespace messages {

typedef TmxJ2735Message<MapData> MapDataMessage;
typedef TmxJ2735EncodedMessage<MapDataMessage> MapDataEncodedMessage;

template <>
constexpr asn_TYPE_descriptor_t *MapDataMessage::get_descriptor()
{
	return &asn_DEF_MapData;
}

template <>
constexpr const char *MapDataMessage::get_messageType()
{
	return api::MSGSUBTYPE_MAPDATA_STRING;
}

template <>
constexpr int MapDataMessage::get_default_msgId()
{
	return (int)api::mapData;
}

} /* End namespace messages */

// Template specializations for handling encoding and decoding the MapDataMessage

template <> template<>
inline messages::MapDataMessage routeable_message::get_payload<messages::MapDataMessage>()
{
	messages::MapDataEncodedMessage encMsg(*this);
	return encMsg.get_payload<messages::MapDataMessage>();
}

} /* End namespace tmx */


#endif /* TMX_J2735_MESSAGES_MAPDATAMESSAGE_HPP_ */
