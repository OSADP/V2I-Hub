/*
 * @file ProbeVehicleDataMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_PROBEVEHICLEDATAMESSAGE_HPP_
#define TMX_J2735_MESSAGES_PROBEVEHICLEDATAMESSAGE_HPP_

#include <asn_j2735_r41/ProbeVehicleData.h>
#include <tmx/messages/TmxJ2735.hpp>

#define PVD_MESSAGETYPE "PVD"

namespace tmx {
namespace messages {

typedef TmxJ2735Message<ProbeVehicleData> PvdMessage;
typedef TmxJ2735EncodedMessage<PvdMessage> PvdEncodedMessage;

template <>
constexpr asn_TYPE_descriptor_t *PvdMessage::get_descriptor()
{
	return &asn_DEF_ProbeVehicleData;
}

template <>
constexpr const char *PvdMessage::get_messageType()
{
	return PVD_MESSAGETYPE;
}

template <>
constexpr int PvdMessage::get_default_msgId()
{
	return DSRCmsgID_probeVehicleData;
}

} /* End namespace messages */

// Template specializations for handling encoding and decoding the PvdMessage

template <> template<>
inline messages::PvdMessage routeable_message::get_payload<messages::PvdMessage>()
{
	messages::PvdEncodedMessage encMsg(*this);
	return encMsg.get_payload<messages::PvdMessage>();
}

} /* End namespace tmx */

#endif /* TMX_J2735_MESSAGES_PROBEVEHICLEDATAMESSAGE_HPP_ */
