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
#define SPAT_MESSAGETYPE "SPAT-P"


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
	return SPAT_MESSAGETYPE;
}

template <>
constexpr int SpatMessage::get_default_msgId()
{
	return SPAT_MSGID;
}

namespace codec
{

// Template specializations for BER/DER encoding for Spat
template <>
inline asn_dec_rval_t der<tmx::messages::SpatMessage>::decode(void **obj, tmx::byte_stream &bytes,
		asn_TYPE_descriptor_t *typeDescriptor)
{
	// The DER encoding for Spat is actually an encoded UPER frame
	UPERframe *frame = 0;
	asn_dec_rval_t udRet = ber_decode(0, &asn_DEF_UPERframe, (void **)&frame, bytes.data(), bytes.size());
	if (udRet.code == RC_OK)
	{
		if (frame->contentID == SPAT_MSGID)
		{
			uper<tmx::messages::SpatMessage> uperDecoder;
			tmx::byte_stream uperFrameBytes(frame->msgBlob.size);
			memcpy(uperFrameBytes.data(), frame->msgBlob.buf, frame->msgBlob.size);
			udRet = uperDecoder.decode(obj, uperFrameBytes);
		}
	}

	return udRet;
}

template<>
inline asn_enc_rval_t der<tmx::messages::SpatMessage>::encode(SPAT *obj, tmx::byte_stream &bytes,
		asn_TYPE_descriptor_t *typeDescriptor)
{
	tmx::byte_stream tmp(bytes);

	// Encode the message in an UPER frame, then encode the frame with BER/DER
	uper<tmx::messages::SpatMessage> encoder;
	asn_enc_rval_t ueRet = encoder.encode(obj, tmp);
	if (ueRet.encoded <= 0)
		return ueRet;

	size_t encoded = (ueRet.encoded + 7) / 8;
	tmp.resize(encoded);

	static UPERframe frame;
	frame.msgID = SPAT_UPERFRAME_MSGID;
	frame.contentID = SPAT_MSGID;
	frame.msgBlob.buf = tmp.data();
	frame.msgBlob.size = encoded;

	ueRet = der_encode_to_buffer(&asn_DEF_UPERframe, &frame, bytes.data(), bytes.max_size());
	return ueRet;
}

} /* End namespace codec */

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
