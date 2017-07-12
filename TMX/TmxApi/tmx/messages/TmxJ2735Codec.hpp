/*
 * TmxJ2735Codec.hpp
 *
 *  Created on: Sep 16, 2016
 *      Author: gmb
 */

#ifndef TMX_MESSAGES_TMXJ2735CODEC_HPP_
#define TMX_MESSAGES_TMXJ2735CODEC_HPP_

#include <string>

#include <tmx/messages/TmxJ2735.hpp>
#include <asn_j2735_r41/UPERframe.h>

namespace tmx {
namespace messages {

typedef boost::error_info<struct tag_codec, std::string> codecerr_info;

typedef TmxJ2735Message<UPERframe> UperFrameMessage;

template <>
constexpr asn_TYPE_descriptor_t *UperFrameMessage::get_descriptor()
{
	return &asn_DEF_UPERframe;
}

template <>
constexpr const char *UperFrameMessage::get_messageType()
{
	return api::MSGSUBTYPE_UPERFRAME_D_STRING;
}

template <>
constexpr int UperFrameMessage::get_default_msgId()
{
	return (int)api::uperFrame_D;
}

namespace codec {

/**
 * A structure for ASN.1 UPER encoding/decoding.  Attached to encoding type "asn.1-uper/hexstring"
 */
template <typename MsgType>
struct uper
{
	typedef tmx::messages::codec::uper<MsgType> type;

	static constexpr const char *Encoding = api::ENCODING_ASN1_UPER_STRING;

	asn_dec_rval_t decode(void **obj, tmx::byte_stream &bytes,
			asn_TYPE_descriptor_t *typeDescriptor = MsgType::get_descriptor())
	{
		return uper_decode_complete(0, typeDescriptor, obj, bytes.data(), bytes.size());
	}

	asn_enc_rval_t encode(typename MsgType::message_type *obj, tmx::byte_stream &bytes,
			asn_TYPE_descriptor_t *typeDescriptor = MsgType::get_descriptor())
	{
		return uper_encode_to_buffer(typeDescriptor, obj, bytes.data(), bytes.max_size());
	}
};

template <typename MsgType>
struct der
{
	typedef tmx::messages::codec::der<MsgType> type;

	static constexpr const char *Encoding = api::ENCODING_ASN1_BER_STRING;

	asn_dec_rval_t decode(void **obj, const tmx::byte_stream &bytes,
			asn_TYPE_descriptor_t *typeDescriptor = MsgType::get_descriptor())
	{
		size_t start;
		int count = 0;
		for (start = 0; start < bytes.size() && bytes[start] != 0x30 && count <= 16; start++, count++);
		if (count > 15) start = 0;  // Beginning not found, just try it
		return ber_decode(0, typeDescriptor, obj, &bytes.data()[start], bytes.size() - start);
	}

	asn_enc_rval_t encode(typename MsgType::message_type *obj, tmx::byte_stream &bytes,
			asn_TYPE_descriptor_t *typeDescriptor = MsgType::get_descriptor())
	{
		return der_encode_to_buffer(typeDescriptor, obj, bytes.data(), bytes.max_size());
	}

	static int decode_msgId(const tmx::byte_stream &bytes)
	{
		int count = 0;

		// Read the DER encoded bytes to obtain the message ID
		byte_stream::const_iterator it;
		for (it = bytes.begin(); it != bytes.end() && *it != 0x30 && count <= 16; it++, count++);
		if (count < 16)
		{
			// Should be the 5th segment
			ber_tlv_len_t len;
			ssize_t lenBytes = 1;
			for (int idx = count, count = 0; count < 5 && idx <= bytes.size(); idx += lenBytes, count++)
			{
				lenBytes = ber_fetch_length(1, &(bytes.data()[idx]), bytes.size(), &len);
				if (lenBytes == 0 || lenBytes == -1)
				{
					len = 0;
					break;
				}
			}

			if (len > api::J2735 && len < api::J2735_end)
				return static_cast<int>(len);
		}

		return -1;
	}

	static int decode_contentId(const tmx::byte_stream &bytes)
	{
		static type derDecoder;

		int id = decode_msgId(bytes);
		if (id == api::uperFrame_D)
		{
			UPERframe *frame = NULL;
			asn_dec_rval_t ret = derDecoder.decode((void **)&frame, bytes, UperFrameMessage::get_descriptor());
			if (ret.code != RC_OK || frame == NULL)
				return -1;

			return frame->contentID;
		}

		return id;
	}
};

template <typename MsgType, int MsgId = MsgType::get_default_msgId()>
struct uperframe
{
	typedef tmx::messages::codec::uperframe<MsgType> type;

	static constexpr const char *Encoding = api::ENCODING_ASN1_BER_STRING;

	asn_dec_rval_t decode(void **obj, tmx::byte_stream &bytes,
			asn_TYPE_descriptor_t *typeDescriptor = MsgType::get_descriptor())
	{
		// The DER encoding is actually an encoded UPER frame
		der<UperFrameMessage> derDecoder;
		UPERframe *frame = 0;
		asn_dec_rval_t udRet = derDecoder.decode((void **)&frame, bytes);
		if (frame->msgID == MsgId)
		{
			*obj = frame;
			return udRet;
		}
		else if (udRet.code == RC_OK)
		{
			if (frame->contentID == MsgId)
			{
				uper<MsgType> uperDecoder;
				tmx::byte_stream uperFrameBytes(frame->msgBlob.size);
				memcpy(uperFrameBytes.data(), frame->msgBlob.buf, frame->msgBlob.size);
				udRet = uperDecoder.decode(obj, uperFrameBytes);
			}
			else
			{
				std::string content = std::to_string(frame->contentID);
				J2735Exception err("UPER bytes were decoded to the invalid " + content + " message identifier.");
				err << codecerr_info{Encoding};
				BOOST_THROW_EXCEPTION(err);
			}
		}

		return udRet;
	}

	asn_enc_rval_t encode(typename MsgType::message_type *obj, tmx::byte_stream &bytes,
			asn_TYPE_descriptor_t *typeDescriptor = MsgType::get_descriptor())
	{
		tmx::byte_stream tmp(bytes);

		// Encode the message in an UPER frame, then encode the frame with BER/DER
		uper<MsgType> encoder;
		asn_enc_rval_t ueRet = encoder.encode(obj, tmp);
		if (ueRet.encoded <= 0)
			return ueRet;

		size_t encoded = (ueRet.encoded + 7) / 8;
		tmp.resize(encoded);

		static UPERframe frame;
		frame.msgID = (int)api::uperFrame_D;
		frame.contentID = MsgId;
		frame.msgBlob.buf = tmp.data();
		frame.msgBlob.size = encoded;

		der<UperFrameMessage> derEncoder;
		return derEncoder.encode(&frame, bytes);
	}
};

} /* End namespace codec */

/**
 * A base class for all J2735 messages
 */
class TmxJ2735EncodedMessageBase: public tmx::routeable_message
{
public:
	TmxJ2735EncodedMessageBase():
		tmx::routeable_message(), init_payload_attribute(data, "") {}
	TmxJ2735EncodedMessageBase(tmx::message_container_type &contents):
		tmx::routeable_message(contents), init_payload_attribute(data, "") {}
	TmxJ2735EncodedMessageBase(tmx::routeable_message &other):
		tmx::routeable_message(other), init_payload_attribute(data, "") {}
	TmxJ2735EncodedMessageBase(IvpMessage *other):
		tmx::routeable_message(other), init_payload_attribute(data, "") {}
	virtual ~TmxJ2735EncodedMessageBase() {}

	virtual tmx::xml_message get_payload() = 0;
	virtual int get_msgId() = 0;

	/**
	 * The data attribute is an encoded byte stream which is serialized under the "payload" tag
	 */
	payload_attribute(this->msg, tmx::byte_stream, data, )
};

/**
 * A template class for an encoded J2735 message that can be routed through the TMX core.
 */
template <typename MsgType>
class TmxJ2735EncodedMessage: public TmxJ2735EncodedMessageBase
{
public:
	static constexpr const char *DefaultCodec = codec::der<MsgType>::Encoding;

	/**
	 * Construct an empty message whose contents will be supplied by another means.  The
	 * default codec is used for this type.
	 */
	TmxJ2735EncodedMessage(): TmxJ2735EncodedMessageBase()
	{
		this->set_type(MsgType::MessageType);
		this->set_subtype(MsgType::MessageSubType);
		this->set_encoding(DefaultCodec);
	}

	/**
	 * Construct a message from a message container
	 * @param contents The contents to load
	 */
	TmxJ2735EncodedMessage(tmx::message_container_type &contents):
		TmxJ2735EncodedMessageBase(contents) { }

	/**
	 * Construct a message from a copy of another routeable message
	 * @param contents The contents to load
	 */
	TmxJ2735EncodedMessage(tmx::routeable_message &other):
		TmxJ2735EncodedMessageBase(other) { }

	/**
	 * Create a message from the (old) IVP message structure.  This is accomplished by serializing the
	 * message to a string format, and loading that string as the contents.
	 * @param other The message to copy from
	 */
	TmxJ2735EncodedMessage(IvpMessage *other):
		TmxJ2735EncodedMessageBase(other) { }
public:
	/**
	 * Decode the J2735 message from the given bytes using a specific decoder type, which must
	 * implement the decode() function.
	 * @param bytes The byte stream to decode
	 * @return The decoded J2735 message
	 */
	template <typename DecType>
	static MsgType decode_j2735_message(tmx::byte_stream bytes)
	{
		DecType decoder;
		typename MsgType::message_type *obj = 0;

		asn_dec_rval_t rval = decoder.decode((void **)&obj, bytes);
		if (rval.code == RC_OK)
		{
			// TODO Free the structure?

			return MsgType(obj);
		}
		else
		{
			J2735Exception err("Unable to decode " + std::string(MsgType::get_messageTag()) + " from bytes.");
			err << codecerr_info{DecType::Encoding};
			err << errmsg_info{"Failed after " + std::to_string(rval.consumed) + " bytes."};
			BOOST_THROW_EXCEPTION(err);
		}
	}

	/**
	 * Encode the given message to a byte stream using a specific encoder type, which must
	 * implement the encode() function.
	 * @param message The J2735 message to encode
	 * @returns The encoded byte stream
	 */
	template <typename EncType>
	static tmx::byte_stream encode_j2735_message(MsgType &message)
	{
		EncType encoder;
		tmx::byte_stream bytes(4000);
		asn_enc_rval_t rval = encoder.encode(message.get_j2735_data(), bytes);

		if (rval.encoded <= 0)
		{
			J2735Exception err("Unable to encod " + std::string(MsgType::get_messageTag()) + " to bytes.");
			err << codecerr_info{EncType::Encoding};
			err << errmsg_info{rval.failed_type->name};
			BOOST_THROW_EXCEPTION(err);
		}
		else
		{
			bytes.resize(rval.encoded);
			bytes.shrink_to_fit();
			return bytes;
		}
	}

	/**
	 * Decode the J2735 message from the data attribute using the default encoding type specified in
	 * the encoding attribute.
	 * @return The decoded J2735 message
	 */
	MsgType decode_j2735_message()
	{
		typedef tmx::messages::codec::uper<MsgType> UperCodec;
		typedef tmx::messages::codec::der<MsgType> DerCodec;
		typedef tmx::messages::codec::uperframe<MsgType> FrameCodec;

		std::string enc = this->get_encoding();

		if (enc == UperCodec::Encoding)
		{
			return TmxJ2735EncodedMessage<MsgType>::decode_j2735_message<UperCodec>(this->get_data());
		}
		else if (enc == DerCodec::Encoding)
		{
			if (DerCodec::decode_msgId(this->get_data()) >= (int)api::uperFrame_D)
				// This is an UPER frame encoded with DER
				return TmxJ2735EncodedMessage<MsgType>::decode_j2735_message<FrameCodec>(this->get_data());
			else
				return TmxJ2735EncodedMessage<MsgType>::decode_j2735_message<DerCodec>(this->get_data());
		}
		else
		{
			J2735Exception err("Unknown encoding.");
			err << codecerr_info{enc};
			BOOST_THROW_EXCEPTION(err);
			throw;	// Just to suppress the warning for non-return value
		}
	}

	/**
	 * Encode the given J2735 message to the data attribute using the default encoding type specified
	 * in the encoding attribute.
	 * @param message The J2735 message to encode
	 */
	void encode_j2735_message(MsgType &message)
	{
		typedef tmx::messages::codec::uper<MsgType> UperCodec;
		typedef tmx::messages::codec::der<MsgType> DerCodec;
		typedef tmx::messages::codec::uperframe<MsgType> FrameCodec;

		std::string enc = this->get_encoding();

		if (enc == UperCodec::Encoding)
		{
			this->set_data(TmxJ2735EncodedMessage<MsgType>::encode_j2735_message<UperCodec>(message));
		}
		else if (enc == DerCodec::Encoding)
		{
			if (message.get_msgId() > api::uperFrame_D)
				this->set_data(TmxJ2735EncodedMessage<MsgType>::encode_j2735_message<FrameCodec>(message));
			else
				this->set_data(TmxJ2735EncodedMessage<MsgType>::encode_j2735_message<DerCodec>(message));
		}
		else
		{
			J2735Exception err("Unkowning encoding");
			err << codecerr_info{enc};
			BOOST_THROW_EXCEPTION(err);
		}
	}

	/**
	 * Override of the template function in tmx::routeable_message to ensure the correct message type
	 * is decoded and extracted.  A compiler error should occur if trying to extract an incompatible type.
	 * @return The decoded J2735 message
	 */
	template <typename OtherMsgType>
	OtherMsgType get_payload()
	{
		if (this->get_encoding().find_last_of("hexstring") > 0)
			return decode_j2735_message();
		else
			return tmx::routeable_message::get_payload<OtherMsgType>();
	}

	tmx::xml_message get_payload()
	{
		return this->get_payload<tmx::xml_message>();
	}

	/**
	 * @return The message identifier for the encoded type
	 */
	int get_msgId()
	{
		// Probably need to decode
		return get_payload<MsgType>().get_msgId();
	}

	/**
	 * Initialize the message data
	 * @param payload The J2735 message payload
	 * @param source The source of the message
	 * @param sourceId The source ID of the message
	 * @param flags Flags for the TMX router
	 */
	virtual void initialize(MsgType &payload, std::string source = "", unsigned int sourceId = 0, unsigned int flags = 0)
	{
		tmx::routeable_message::initialize(MsgType::MessageType, MsgType::MessageSubType,
				source, sourceId, flags);
		this->encode_j2735_message(payload);
	}
};

typedef TmxJ2735EncodedMessage<UperFrameMessage> UperFrameEncodedMessage;

template <>
inline int UperFrameEncodedMessage::get_msgId()
{
	return get_payload<UperFrameMessage>().get_j2735_data()->contentID;
}

} /* End namespace messages */
} /* End namespace tmx */


#endif /* TMX_MESSAGES_TMXJ2735CODEC_HPP_ */
