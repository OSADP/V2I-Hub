/*
 * TmxJ2735.hpp
 *
 *  Created on: Apr 27, 2016
 *      Author: ivp
 */

#ifndef TMX_MESSAGES_TMXJ2735_HPP_
#define TMX_MESSAGES_TMXJ2735_HPP_

#include <cerrno>
#include <memory>
#include <stdexcept>
#include <stdio.h>

#include <asn_j2735_r41/DSRCmsgID.h>
#include <asn_j2735_r41/UPERframe.h>
#include <tmx/tmx.h>
#include <tmx/messages/IvpJ2735.h>
#include <tmx/messages/routeable_message.hpp>

#define TMX_J2735_MESSAGE_TYPE "J2735"

namespace tmx {
namespace messages {

namespace codec {

/**
 * A structure for ASN.1 UPER encoding/decoding.  Attached to encoding type "asn.1-uper/hexstring"
 */
template <typename MsgType>
struct uper
{
	typedef tmx::messages::codec::uper<MsgType> type;

	static constexpr const char *Encoding = IVP_ENCODING_ASN1_UPER;

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

	static constexpr const char *Encoding = IVP_ENCODING_ASN1_BER;

	asn_dec_rval_t decode(void **obj, tmx::byte_stream &bytes,
			asn_TYPE_descriptor_t *typeDescriptor = MsgType::get_descriptor())
	{
		return ber_decode(0, typeDescriptor, obj, bytes.data(), bytes.size());
	}

	asn_enc_rval_t encode(typename MsgType::message_type *obj, tmx::byte_stream &bytes,
			asn_TYPE_descriptor_t *typeDescriptor = MsgType::get_descriptor())
	{
		return der_encode_to_buffer(typeDescriptor, obj, bytes.data(), bytes.max_size());
	}
};

} /* End namespace codec */

/**
 * Exception class for run-time errors that occur in encoding/decoding the J2735 Messages
 */
class J2735Exception: public std::runtime_error
{
public:
	/**
	 * Create a J2735Exception from another exception.
	 * @param cause The root cause exception
	 */
	J2735Exception(const std::runtime_error &cause): std::runtime_error(cause) {}

	/**
	 * Create a J2735Exception from a string describing what happened.
	 * @param what The reason for the exception
	 */
	J2735Exception(const std::string &what): std::runtime_error(what) {}
};

/**
 * A template class for all J2735 messages.  This class is a decoded version of the
 * specific J2735 data type structure built with the ASN.1 compiler, but represented
 * in a boost::property_tree.  This type can be used in a handler if you want the
 * decode version of the message, which would make the most sense.
 */
template <typename DataType>
class TmxJ2735Message: public tmx::xml_message
{
public:
	/// The J2735 data type
	typedef DataType message_type;
	typedef TmxJ2735Message<message_type> type;

	/**
	 * Create a J2735 Message of this type, optionally using a pointer to the J2735
	 * data type structure.  If no pointer is given, then the message is empty until
	 * contents can be loaded in some other manner.
	 * @param data Pointer to the J2735 data
	 */
	TmxJ2735Message(message_type *data = 0): tmx::xml_message()
	{
		init_attributes();
		set_j2735_data(data);
	}

	/**
	 * Destructor
	 */
	virtual ~TmxJ2735Message()
	{
		//destroy();
	}

	/**
	 * Returns a pointer to a filled in J2735 data structure, taken from an XML serialization of the property tree.
	 * @return The pointer to the structure
	 */
	message_type *get_j2735_data()
	{
		_j2735_data = 0;

		std::string myData = this->to_string();

		asn_dec_rval_t rval;

		rval = xer_decode(NULL, get_descriptor(), (void **)&_j2735_data, myData.c_str(), myData.size());
		if (rval.code != RC_OK)
		{
			std::stringstream err;
			err << "Unable to decode " << get_messageTag() << " from " << myData <<
					"\nFailed after " << rval.consumed << " bytes.";
			throw J2735Exception(err.str());
		}


		return _j2735_data;
	}

	/**
	 * Populates the property tree from an XML serialization of the supplied J2735 data structure
	 * @param data A pointer to a filled in J2735 data structure
	 */
	void set_j2735_data(message_type *data)
	{
		if (data == NULL)
			return;

		char *buffer;
		size_t bufSize;
		FILE *mStream = open_memstream(&buffer, &bufSize);

		if (mStream == NULL)
		{
			std::string errMsg(strerror(errno));
			throw J2735Exception("Unable to open stream in memory: " + errMsg);
		}

		if (xer_fprint(mStream, get_descriptor(), data) < 0)
			throw J2735Exception("Unable to stream XML contents in memory: Unknown error");

		fclose(mStream);

		std::string xml(buffer, bufSize);
		this->set_contents(xml);
	}

	/**
	 * @return The ASN.1 descriptor for the J2735 data type
	 */
	static constexpr asn_TYPE_descriptor_t *get_descriptor()
	{
		throw J2735Exception("Null ASN descriptor type discovered for " +
				battelle::attributes::type_id_name<message_type>() + ".");
	}

	/**
	 * @return The XML tag for the J2735 data type
	 */
	static constexpr const char *get_messageTag()
	{
		return get_descriptor()->xml_tag;
	}

	/**
	 * @return The J2735 message type (by default it is the same as the tag) that is used for a TMX message sub-type
	 */
	static constexpr const char* get_messageType()
	{
		return get_messageTag();
	}

	/**
	 * @return The default J2735 message id (if known)
	 */
	static constexpr int get_default_msgId()
	{
		return -1;
	}

	static constexpr const char *MessageType = TMX_J2735_MESSAGE_TYPE;
	static constexpr const char *MessageSubType = TmxJ2735Message<DataType>::get_messageType();
	static constexpr const char *DefaultCodec = tmx::messages::codec::der<type>::Encoding;
protected:
	virtual void init_attributes() { }

private:
	message_type *_j2735_data;

	static std::string msgId_path()
	{
		static bool pathInit = false;
		static std::string path(get_messageTag());
		if (!pathInit)
		{
			path += ".msgID";
			pathInit = true;
		}
		return path;
	}

	void destroy()
	{
		try
		{
			if (_j2735_data)
				get_descriptor()->free_struct(get_descriptor(), _j2735_data, 0);
		}
		catch (std::exception &ex)
		{
		}

		_j2735_data = 0;

	}
};

/**
 * A template class for an encoded J2735 message that can be routed through the TMX core.
 */
template <typename MsgType>
class TmxJ2735EncodedMessage: public tmx::routeable_message
{
public:
	/**
	 * Construct an empty message whose contents will be supplied by another means.  The
	 * default codec is used for this type.
	 */
	TmxJ2735EncodedMessage(): tmx::routeable_message(),
		init_payload_attribute(data, "")
	{
		this->set_type(MsgType::MessageType);
		this->set_subtype(MsgType::MessageSubType);
		this->set_encoding(MsgType::DefaultCodec);
	}

	/**
	 * Construct a message from a message container
	 * @param contents The contents to load
	 */
	TmxJ2735EncodedMessage(tmx::message_container_type &contents):
		tmx::routeable_message(contents),
		init_payload_attribute(data, "") { }

	/**
	 * Construct a message from a copy of another routeable message
	 * @param contents The contents to load
	 */
	TmxJ2735EncodedMessage(tmx::routeable_message &other):
		tmx::routeable_message(other),
		init_payload_attribute(data, "") { }

	/**
	 * Create a message from the (old) IVP message structure.  This is accomplished by serializing the
	 * message to a string format, and loading that string as the contents.
	 * @param other The message to copy from
	 */
	TmxJ2735EncodedMessage(IvpMessage *other):
		tmx::routeable_message(other),
		init_payload_attribute(data, "") { }


	/**
	 * The data attribute is an encoded byte stream which is serialized under the "payload" tag
	 */
	payload_attribute(this->msg, tmx::byte_stream, data, )
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
			std::stringstream err;
			err << "Unable to decode " << MsgType::get_messageTag() << " from bytes " <<
					"\nFailed after " << rval.consumed << " bytes.";
			throw J2735Exception(err.str());
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
			std::string tag(MsgType::get_messageTag());
			std::string err(rval.failed_type->name);
			throw J2735Exception("Unable to encode " + tag + ": " + err);
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

		std::string enc = this->get_encoding();

		if (enc == UperCodec::Encoding)
		{
			return TmxJ2735EncodedMessage<MsgType>::decode_j2735_message<UperCodec>(this->get_data());
		}
		else if (enc == DerCodec::Encoding)
		{
			return TmxJ2735EncodedMessage<MsgType>::decode_j2735_message<DerCodec>(this->get_data());
		}
		else
		{
			throw J2735Exception("Do not know how to decode " + enc + " encoding.");
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

		std::string enc = this->get_encoding();

		if (enc == UperCodec::Encoding)
		{
			this->set_data(TmxJ2735EncodedMessage<MsgType>::encode_j2735_message<UperCodec>(message));
		}
		else if (enc == DerCodec::Encoding)
		{
			this->set_data(TmxJ2735EncodedMessage<MsgType>::encode_j2735_message<DerCodec>(message));
		}
		else
		{
			throw J2735Exception("Do not know how to encode " + enc);
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

} /* End namespace messages */
} /* End namespace tmx */

#endif /* TMX_MESSAGES_TMXJ2735_HPP_ */
