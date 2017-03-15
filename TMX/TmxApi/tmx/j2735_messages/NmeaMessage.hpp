/*
 * @file NmeaMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_NMEAMESSAGE_HPP_
#define TMX_J2735_MESSAGES_NMEAMESSAGE_HPP_

#include <asn_j2735_r41/NMEA-Corrections.h>
#include <tmx/messages/TmxJ2735.hpp>

namespace tmx {
namespace messages {

typedef TmxJ2735Message<NMEA_Corrections> NmeaMessage;
typedef TmxJ2735EncodedMessage<NmeaMessage> NmeaEncodedMessage;

template <>
constexpr asn_TYPE_descriptor_t *NmeaMessage::get_descriptor()
{
	return &asn_DEF_NMEA_Corrections;
}

template <>
constexpr const char *NmeaMessage::get_messageType()
{
	return api::MSGSUBTYPE_NMEACORRECTIONS_STRING;
}

template <>
constexpr int NmeaMessage::get_default_msgId()
{
	return (int)api::nmeaCorrections_D; // DSRCmsgID_nmeaCorrections;
}

} /* End namespace message */

// Template specializations for handling encoding and decoding the message

template <> template <>
inline messages::NmeaMessage routeable_message::get_payload<messages::NmeaMessage>()
{
	messages::NmeaEncodedMessage encMsg(*this);
	return encMsg.get_payload<messages::NmeaMessage>();
}

} /* End namespace tmx */

#endif /* TMX_J2735_MESSAGES_NMEAMESSAGE_HPP_ */
