/*
 * TmxError.hpp
 *
 *  Created on: Apr 26, 2016
 *      Author: ivp
 */

#ifndef TMX_TMXNMEA_HPP_
#define TMX_TMXNMEA_HPP_

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
		std::string sentence = this->get_subtype();
		if (sentence.length() > 0)
			sentence.insert(0, "$GP");

		return sentence;
	}

	void set_sentence(const std::string sentence)
	{
		if(sentence == "$GPGGA")
			this->set_subtype("GGA");
		if(sentence == "$GPGSA")
			this->set_subtype("GSA");
		if(sentence == "$GPRMC")
			this->set_subtype("RMC");
		if(sentence == "$GPGSV")
			this->set_subtype("GSV");
		if(sentence == "$GPVTG")
			this->set_subtype("VTG");
		if(sentence == "$GPGLL")
			this->set_subtype("GLL");
	}

	std::string get_nmeaString()
	{
		return get_payload_str();
	}

	void set_nmeaString(const std::string &nmea)
	{
		set_payload(nmea);
	}
};

} /* End namespace messages */
} /* End namespace tmx */


#endif /* TMX_TMXNMEA_HPP_ */
