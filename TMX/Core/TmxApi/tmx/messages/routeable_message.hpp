/*
 * routeable_message.hpp
 *
 *  Created on: Apr 7, 2016
 *      Author: ivp
 */

#ifndef TMX_MESSAGES_ROUTEABLE_MESSAGE_HPP_
#define TMX_MESSAGES_ROUTEABLE_MESSAGE_HPP_

#include <sys/time.h>
#include <tmx/IvpMessage.h>
#include <tmx/tmx.h>
#include <tmx/messages/byte_stream.hpp>
#include <tmx/messages/message.hpp>

#define ATTR_HEADER "header"
#define DSRC_HEADER "dsrcMetadata"
#define ATTR_PAYLOAD "payload"

// For an attribute that is stored exclusively in the payload
#define payload_attribute(C, X, Y, L) \
private: \
	battelle::attributes::typed_attribute<X> attr_field_name(Y); \
public: \
	typesafe_getter_builder(C, X, Y, get_) \
	typesafe_setter_builder(C, X, Y, set_, L)
#define init_payload_attribute(Y, D) \
	attr_field_name(Y)(battelle::attributes::attribute_traits(\
			quoted_attribute_name(Y), ATTR_PAYLOAD, \
			battelle::attributes::attribute_lexical_cast<\
			message_container_type::value_type>(D)))

namespace tmx
{

/**
 * A class to represent a specific type of message with attributes that are used to route through
 * the TMX core.  There is a header portion of the message which contains attributes that can be used
 * to identify the type of the message and the source, and a payload portion that contains the actual message
 * data.  Most message types are just other tmx::message types in the same format, but can also be a string
 * containing the data.  The type and subtype attributes in the header are used to determine information about
 * the payload, and the encoding attribute may be used to determine how the data is encoded.  Typically, this will
 * be using the Format encoding, whose default is JSON.  Other Formats and other encodings must be serialized as
 * a string payload.
 */
template <typename Format = TMX_DEFAULT_MESSAGE_FORMAT>
class tmx_routeable_message: public tmx_message<Format>
{
public:
	/**
	 * Create an empty message.  Initializes the header.
	 */
	tmx_routeable_message()
	{
		init_attributes();
	}

	/**
	 * Create a message with a copy of the message container contents.  Nothing is initialized.
	 * @param contents The already filled message container
	 */
	tmx_routeable_message(const message_container_type &contents):
		tmx_message<Format>(contents) { }

	/**
	 * Create a message that has a copy of the contents of the other message.
	 * @see tmx::message(const message<OtherFormat> &, message_converter *)
	 * @param other The message to copy from
	 * @param converter A specific converter to use, or NULL for the default one
	 */
	template <typename OtherFormat>
	tmx_routeable_message(const tmx_routeable_message<OtherFormat> &other, message_converter *converter = 0):
		tmx_message<Format>(other, converter) { }

	/**
	 * Create a message from a string representation of the contents.
	 * @see tmx::message(const std::string &)
	 * @param contents A string representation of the contents.
	 */
	tmx_routeable_message(const std::string &contents): tmx_message<Format>(contents) { }

	/**
	 * Create a message from the (old) IVP message structure.  This is accomplished by serializing the
	 * message to a string format, and loading that string as the contents.
	 * @param other The message to copy from
	 */
	tmx_routeable_message(IvpMessage *other):
		tmx_message<Format>(std::string(
				ivpMsg_createJsonString(other, IvpMsg_FormatOptions_none))) { }

	/**
	 * Destructor
	 */
	virtual ~tmx_routeable_message()
	{
	}
public:
	/**
	 * Get the payload as the specified message type.  Note that the message type class must be of
	 * type tmx::message.  If the payload is a string, then the contents of the returned message are
	 * set using tmx::message::set_contents(string).  Otherwise, the contents are set using
	 * tmx::message::set_contents(message_tree_type &) with the sub-tree.
	 * @return A new message of the specified type constructed with the payload contents
	 */
	template <typename MessageType>
	MessageType get_payload()
	{
		MessageType newMsg;

		// Some older messages are actually routeable messages in themselves.  If that is the case, then
		// this needs to return just a copy of the whole message in that new type
		static constexpr bool is_routeable =
				std::is_base_of<tmx_routeable_message<Format>, MessageType>::value;

		if (is_routeable)
		{
			newMsg.set_contents(this->msg);
		}
		else
		{
			boost::optional<message_tree_type &> payload = this->as_tree(ATTR_PAYLOAD);
			if (payload)
			{
				// We have something here.  Is it a standard string?
				if (payload.get().empty())
					newMsg.set_contents(payload.get().get_value<std::string>());
				else
					newMsg.set_contents(payload.get());
			}
		}

		return newMsg;
	}

	/**
	 * @return A string representation of the payload
	 */
	std::string get_payload_str()
	{
		boost::optional<message_tree_type &> payload = this->as_tree(ATTR_PAYLOAD);
		if (payload)
		{
			// We have something here.  Is it a simple string?
			if (payload.get().empty())
				return payload.get().get_value<std::string>();
			// Or a wrapped message type
			else
				return get_payload<tmx_message<Format> >().to_string();
		}
		else
		{
			return "";
		}
	}

	/**
	 * @see get_payload_str()
	 * @return A byte stream representation of the payload string
	 */
	byte_stream get_payload_bytes()
	{
		return battelle::attributes::attribute_lexical_cast<byte_stream>(this->get_payload_str());
	}

	/**
	 * Set the payload with the given message.  Note that the payload should be in the same format.
	 * @param payload The message payload
	 */
	void set_payload(tmx_message<Format> &payload)
	{
		message_tree_type copy(this->as_tree(payload.get_container()).get());
		this->as_tree().get().put_child(ATTR_PAYLOAD, copy);
	}

	/**
	 * Set the payload with the given message of another format.  Note that a string representation
	 * of the contents will be used and the encoding type will be "<format>string", such as
	 * "jsonstring" or "xmlstring".
	 * @param payload The message payload
	 */
	template <typename OtherFormat>
	void set_payload(tmx_message<OtherFormat> &payload)
	{
		this->set_payload(payload.to_string());

		// Set the encoding based on the format.  In lowercase.
		std::string format = payload.format();
		std::transform(format.begin(), format.end(), format.begin(), ::tolower);
		format.append(IVP_ENCODING_STRING);

		this->set_encoding(format);
	}

	/**
	 * Set the payload with the given string.  Note that the encoding type will be "string".
	 * @param payload The payload string
	 */
	void set_payload(std::string payload)
	{
		this->set_encoding(IVP_ENCODING_STRING);
		this->as_tree().get().put(ATTR_PAYLOAD, payload);
	}

	/**
	 * Set the payload with the given byte stream.  Note that the encoding type will be "bytearray/hexstring"
	 * @param bytes The payload bytes
	 */
	void set_payload_bytes(const byte_stream bytes)
	{
		set_payload(battelle::attributes::attribute_lexical_cast<std::string>(bytes));
		this->set_encoding(IVP_ENCODING_BYTEARRAY);
	}

	/**
	 * Initialize the message contents, given a payload message
	 * @param payload The message payload
	 * @param source The source of the message
	 * @param sourceId The source ID of the message
	 * @param flags Flags for the TMX router
	 */
	template <typename MessageType>
	void initialize(MessageType &payload, const std::string source = "", unsigned int sourceId = 0, unsigned int flags = 0)
	{
		this->initialize(MessageType::MessageType, MessageType::MessageSubType, source, sourceId, flags);
		this->set_payload(payload);
	}

	/**
	 * Initialize the message contents, given only the payload type and subtype
	 * @param messageType The payload message type
	 * @param messageSubType The payload message subtype
	 * @param source The source of the message
	 * @param sourceId The source ID of the message
	 * @param flags Flags for the TMX router
	 */
	virtual void initialize(std::string messageType, std::string messageSubType,
			const std::string source = "", unsigned int sourceId = 0, unsigned int flags = 0)
	{
		this->set_type(messageType);
		this->set_subtype(messageSubType);
		this->refresh_timestamp();
		if (source.length() > 0) this->set_source(source);
		if (sourceId > 0) this->set_sourceId(sourceId);
		if (flags > 0) this->set_flags(flags);
	}
private:
    struct IvpMessageHeaderCreator
	{
		typedef message_path_type path_type;

		path_type operator()(path_type attr_path)
		{
			path_type p(ATTR_HEADER);
			std::string tmp(attr_path.dump());
			if (attr_path.dump().compare(0, 4, "dsrc") == 0)
			{
				// DSRC attributes go under a separate sub-object
				p /= DSRC_HEADER;
				tmp.replace(0, 4, "");
				tmp[0] = tolower(tmp[0]);
				p /= tmp;
			}
			else
			{
				p /= attr_path;
			}
			return p;
		}
	};

    template <typename T>
    using ivp_header_attribute =
    		battelle::attributes::standard_2level_attribute<T, message_container_type::storage_type, IvpMessageHeaderCreator>;

 	rw_attribute(this->msg, ivp_header_attribute<type>, std::string, type, get_, "Unknown", set_, )
    rw_attribute(this->msg, ivp_header_attribute<subtype>, std::string, subtype, get_, "Unknown", set_, )
	rw_attribute(this->msg, ivp_header_attribute<source>, std::string, source, get_, "", set_, )
	rw_attribute(this->msg, ivp_header_attribute<sourceId>, unsigned int, sourceId, get_, 0, set_, )
	rw_attribute(this->msg, ivp_header_attribute<encoding>, std::string, encoding, get_, "", set_, )
	rw_attribute(this->msg, ivp_header_attribute<timestamp>, uint64_t, timestamp, get_, 0, set_, )
	rw_attribute(this->msg, ivp_header_attribute<flags>, unsigned int, flags, get_, 0, set_, )
	rw_attribute(this->msg, ivp_header_attribute<dsrcChannel>, int, dsrcChannel, get_, -1, set_, )
	rw_attribute(this->msg, ivp_header_attribute<dsrcPsid>, int, dsrcPsid, get_, -1, set_, )
public:
	// Some other helpful routines

	/**
	 * Note that the resulting pointer has memory allocated that must be freed
	 * with ivpMsg_destroy.
	 *
	 * @return A filled in (old) IVP message structure from the message contents.
	 */
	IvpMessage* get_message()
	{
		IvpMessage *message = ivpMsg_parse(const_cast<char*>(this->to_string().c_str()));

		// The numeric values are not converted from strings, so we have to do this manually
		try_set(message->sourceId, _sourceId.traits().path);
		try_set(message->timestamp, _timestamp.traits().path);
		try_set(message->flags, _flags.traits().path);
		if (message->dsrcMetadata != NULL)
		{
			try_set(message->dsrcMetadata->channel, _dsrcChannel.traits().path);
			try_set(message->dsrcMetadata->psid, _dsrcPsid.traits().path);
		}

		return message;
	}

	/**
	 * Reset the timestamp attribute to the current time
	 */
	void refresh_timestamp()
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);
		double millisecondsSinceEpoch =
		    (double)(tv.tv_sec) * 1000 +
		    (double)(tv.tv_usec) / 1000;

		this->set_timestamp(millisecondsSinceEpoch);
	}

	/**
	 * Add DSRC metadata attributes
	 * @param channel The DSRC channel
	 * @param psid The DSRC psid
	 */
	void addDsrcMetadata(int channel, int psid)
	{
		this->set_dsrcChannel(channel);
		this->set_dsrcPsid(psid);
	}

	/**
	 * Clear the message container and reset the header attributes
	 */
	void clear()
	{
		tmx_message<Format>::clear();
		init_attributes();
	}
private:
	void init_attributes()
	{
		// Initialize type, sub-type, encoding, timestamp and flags in that order to ensure they get set
		this->get_type();
		this->get_subtype();

		// Set the encoding based on the format.  In lowercase.
		std::string format = this->format();
		std::transform(format.begin(), format.end(), format.begin(), ::tolower);
		this->set_encoding(format);

		this->get_timestamp();
		this->get_flags();
	}

	template <typename DataType>
	void try_set(DataType &field, message_path_type path) {
		std::string val = battelle::attributes::get_value_at(this->msg, path, "");
		if (!val.empty())
			field = battelle::attributes::attribute_lexical_cast<DataType>(val);
	}
};

/// A TMX routeable message in JSON format
typedef tmx_routeable_message<JSON> json_routeable_message;
/// A TMX routeable message in XML format
typedef tmx_routeable_message<XML> xml_routeable_message;
/// A TMX routeable message in the default format
typedef tmx_routeable_message<TMX_DEFAULT_MESSAGE_FORMAT> routeable_message;

} /* End namespace tmx */

#endif /* TMX_MESSAGES_ROUTEABLE_MESSAGE_HPP_ */
