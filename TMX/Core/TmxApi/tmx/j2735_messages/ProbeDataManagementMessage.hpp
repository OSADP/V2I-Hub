/*
 * @file ProbeDataManagementMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_PROBEDATAMANAGEMENTMESSAGE_HPP_
#define TMX_J2735_MESSAGES_PROBEDATAMANAGEMENTMESSAGE_HPP_

#include <asn_j2735_r41/ProbeDataManagement.h>
#include <tmx/messages/TmxJ2735.hpp>

#define PDM_MESSAGETYPE "PDM"

namespace tmx {
namespace messages {

typedef TmxJ2735Message<ProbeDataManagement> PdmMessage;
typedef TmxJ2735EncodedMessage<PdmMessage> PdmEncodedMessage;

template <>
constexpr asn_TYPE_descriptor_t *PdmMessage::get_descriptor()
{
	return &asn_DEF_ProbeDataManagement;
}

template <>
constexpr const char *PdmMessage::get_messageType()
{
	return PDM_MESSAGETYPE;
}

template <>
constexpr int PdmMessage::get_default_msgId()
{
	return DSRCmsgID_probeDataManagement;
}

} /* End namespace message */

// Template specializations for handling encoding and decoding the message

template <> template <>
inline messages::PdmMessage routeable_message::get_payload<messages::PdmMessage>()
{
	messages::PdmEncodedMessage encMsg(*this);
	return encMsg.get_payload<messages::PdmMessage>();
}

} /* End namespace tmx */

#endif /* TMX_J2735_MESSAGES_PROBEDATAMANAGEMENTMESSAGE_HPP_ */
