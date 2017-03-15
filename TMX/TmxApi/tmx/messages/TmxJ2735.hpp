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

#include <asn_j2735_r41/asn_application.h>
#include <tmx/TmxApiMessages.h>
#include <tmx/TmxException.hpp>
#include <tmx/messages/routeable_message.hpp>

namespace tmx {
namespace messages {

/**
 * Exception class for run-time errors that occur in encoding/decoding the J2735 Messages
 */
class J2735Exception: public tmx::TmxException
{
public:
	/**
	 * Create a J2735Exception from another exception.
	 * @param cause The root cause exception
	 */
	J2735Exception(const std::exception &cause): tmx::TmxException(cause) {}

	/**
	 * Create a J2735Exception from a string describing what happened.
	 * @param what The reason for the exception
	 */
	J2735Exception(const std::string &what): tmx::TmxException(what) {}
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
	TmxJ2735Message(message_type *data = 0): tmx::xml_message(), _j2735_data(0)
	{
		init_attributes();
		set_j2735_data(data);
	}

	/*
	 * Copy constructor
	 */
	TmxJ2735Message(const TmxJ2735Message<DataType>& msg): tmx::xml_message(msg), _j2735_data(0)
	{
		init_attributes();
	}

	/**
	 * Destructor
	 */
	virtual ~TmxJ2735Message()
	{
		destroy();
	}

	/*
	 * Define assignment operator to keep compiler from creating a default which
	 * will copy the _j2735_data pointer which can cause exceptions
	 */
	TmxJ2735Message<DataType>& operator=(const TmxJ2735Message<DataType>& msg)
	{
		if (this != &msg)
		{
			init_attributes();
			tmx::xml_message::operator=(msg);
			destroy();
		}
		return *this;
	}

	/**
	 * @return The message indentifier for the message type
	 */
	int get_msgId()
	{
		boost::optional<message_tree_type &> tree = this->as_tree(this->msgId_path());
		if (tree)
		{
			boost::optional<int> optRet = tree.get().get_value_optional<int>();
			if (optRet)
				return optRet.get();
		}

		return get_default_msgId();
	}

	/**
	 * Returns a pointer to a filled in J2735 data structure, taken from an XML serialization of the property tree.
	 * @return The pointer to the structure
	 */
	message_type *get_j2735_data()
	{
		destroy();

		std::string myData = this->to_string();

		asn_dec_rval_t rval;

		rval = xer_decode(NULL, get_descriptor(), (void **)&_j2735_data, myData.c_str(), myData.size());
		if (rval.code != RC_OK)
		{
			std::stringstream err;
			err << "Unable to decode " << get_messageTag() << " from " << myData <<
					"\nFailed after " << rval.consumed << " bytes.";
			BOOST_THROW_EXCEPTION(J2735Exception(err.str()));
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
			BOOST_THROW_EXCEPTION(J2735Exception("Unable to open stream in memory: " + errMsg));
		}

		if (xer_fprint(mStream, get_descriptor(), data) < 0)
			BOOST_THROW_EXCEPTION(J2735Exception("Unable to stream XML contents in memory: Unknown error"));

		fclose(mStream);

		std::string xml(buffer, bufSize);
		this->set_contents(xml);
	}

	/**
	 * @return The ASN.1 descriptor for the J2735 data type
	 */
	static constexpr asn_TYPE_descriptor_t *get_descriptor()
	{
		BOOST_THROW_EXCEPTION(J2735Exception("Null ASN descriptor type discovered for " +
				battelle::attributes::type_id_name<message_type>() + "."));
		return 0;
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

	static constexpr const char *MessageType = tmx::messages::api::MSGSUBTYPE_J2735_STRING;
	static constexpr const char *MessageSubType = TmxJ2735Message<DataType>::get_messageType();
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

} /* End namespace messages */
} /* End namespace tmx */

// Automatically include the encoded and decoded messages
#include <tmx/messages/TmxJ2735Codec.hpp>

#endif /* TMX_MESSAGES_TMXJ2735_HPP_ */
