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
#define MAPDATA_MESSAGETYPE "MAP-P"

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
	return MAPDATA_MESSAGETYPE;
}

template <>
constexpr int MapDataMessage::get_default_msgId()
{
	return MAPDATA_MSGID;
}

namespace codec
{

// Template specializations for BER/DER encoding for MapData
template <>
inline asn_dec_rval_t der<tmx::messages::MapDataMessage>::decode(void **obj, tmx::byte_stream &bytes,
		asn_TYPE_descriptor_t *typeDescriptor)
{
	// The DER encoding for MapData is actually an encoded UPER frame
	UPERframe *frame = 0;
	asn_dec_rval_t udRet = ber_decode(0, &asn_DEF_UPERframe, (void **)&frame, bytes.data(), bytes.size());
	if (udRet.code == RC_OK)
	{
		if (frame->contentID == MAPDATA_MSGID)
		{
			uper<tmx::messages::MapDataMessage> uperDecoder;
			tmx::byte_stream uperFrameBytes(frame->msgBlob.size);
			memcpy(uperFrameBytes.data(), frame->msgBlob.buf, frame->msgBlob.size);
			udRet = uperDecoder.decode(obj, uperFrameBytes);
		}
	}

	return udRet;
}

template<>
inline asn_enc_rval_t der<tmx::messages::MapDataMessage>::encode(MapData *obj, tmx::byte_stream &bytes,
		asn_TYPE_descriptor_t *typeDescriptor)
{
	tmx::byte_stream tmp(bytes);

	// Encode the message in an UPER frame, then encode the frame with BER/DER
	uper<tmx::messages::MapDataMessage> encoder;
	asn_enc_rval_t ueRet = encoder.encode(obj, tmp);
	if (ueRet.encoded <= 0)
		return ueRet;

	size_t encoded = (ueRet.encoded + 7) / 8;
	tmp.resize(encoded);

	static UPERframe frame;
	frame.msgID = MAPDATA_UPERFRAME_MSGID;
	frame.contentID = MAPDATA_MSGID;
	frame.msgBlob.buf = tmp.data();
	frame.msgBlob.size = encoded;

	ueRet = der_encode_to_buffer(&asn_DEF_UPERframe, &frame, bytes.data(), bytes.max_size());
	return ueRet;
}

} /* End namespace codec */

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
