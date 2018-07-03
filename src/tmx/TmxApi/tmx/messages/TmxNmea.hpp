/*
 * TmxError.hpp
 *
 *  Created on: Apr 26, 2016
 *      Author: ivp
 */

#ifndef TMX_TMXNMEA_HPP_
#define TMX_TMXNMEA_HPP_

#include <cstring>
#include <tmx/messages/routeable_message.hpp>
#include "IvpNmea.h"

namespace tmx {
namespace messages {

class TmxNmeaMessage: public tmx::routeable_message
{
public:
	TmxNmeaMessage(): routeable_message()
	{
		this->set_type(IVPMSG_TYPE_NMEA);
		this->set_encoding(IVP_ENCODING_STRING);
	}

	TmxNmeaMessage(const std::string sentence): TmxNmeaMessage()
	{
		this->set_sentence(sentence);
	}

	std::string get_sentence()
	{
		return get_payload_str();
	}

	void set_sentence(const std::string sentence)
	{
		this->set_payload(sentence);

		if(::strstr(sentence.c_str(), "$GPGGA") != NULL)
			this->set_subtype("GGA");
		if(::strstr(sentence.c_str(), "$GPGSA") != NULL)
			this->set_subtype("GSA");
		if(::strstr(sentence.c_str(), "$GPRMC") != NULL)
			this->set_subtype("RMC");
		if(::strstr(sentence.c_str(), "$GPGSV") != NULL)
			this->set_subtype("GSV");
		if(::strstr(sentence.c_str(), "$GPVTG") != NULL)
			this->set_subtype("VTG");
		if(::strstr(sentence.c_str(), "$GPVTG") != NULL)
			this->set_subtype("GLL");

	}
};

} /* End namespace messages */
} /* End namespace tmx */


#endif /* TMX_TMXNMEA_HPP_ */
