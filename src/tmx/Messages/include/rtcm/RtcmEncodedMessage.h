/*
 * RtcmEncodedMessage.h
 *
 *  Created on: May 11, 2018
 *      @author: Gregory M. Baumgardner
 */

#ifndef INCLUDE_RTCM_RTCMENCODEDMESSAGE_H_
#define INCLUDE_RTCM_RTCMENCODEDMESSAGE_H_

#include <memory>
#include <tmx/TmxException.hpp>
#include <tmx/messages/routeable_message.hpp>

#include "RtcmMessage.h"
#include "RtcmMessageFactory.h"

namespace tmx {
namespace messages {

class TmxRtcmEncodedMessage: public tmx::routeable_message {
public:
	TmxRtcmEncodedMessage(): tmx::routeable_message() { }
	TmxRtcmEncodedMessage(const tmx::routeable_message &other):
		tmx::routeable_message(other) { this->reinit();	}
	virtual ~TmxRtcmEncodedMessage() { }

	std::shared_ptr<TmxRtcmMessage> get_rtcm_payload() {
		if (!_decoded) {
			_decoded.reset(factory.create(this->get_payload_bytes(), this->get_subtype()));

			// Try to build the correct type if one is known
			if (_decoded && _decoded->get_Version() == rtcm::UNKNOWN) {
				_decoded.reset();
				// The message pointer must be initialized and have valid message
				int v = (int)rtcm::RTCM_EOF - 1;
				for (; v >= 0 && (!_decoded || !_decoded->is_Valid()); v--) {
					_decoded.reset(factory.create(this->get_payload_bytes(), (rtcm::RTCM_VERSION)v));
					std::cout << "Created " << rtcm::RtcmVersionName(_decoded->get_Version()) << " message of type " << _decoded->get_MessageType() << std::endl;
					std::cout << *_decoded << std::endl;
					std::cout << (_decoded->is_Valid() ? "It is valid" : "It is not valid") << std::endl;
				}
			}

			if (_decoded && _decoded->get_MessageType() != 0) {
				_decoded.reset(factory.create(_decoded.get()));
			}

			if (_decoded)
				this->set_subtype(rtcm::RtcmVersionName(_decoded->get_Version()));
		}

		return _decoded;
	}

	template <rtcm::RTCM_VERSION Version>
	void set_rtcm_payload(RTCMMessage<Version> &msg) {
		set_rtcm_payload(&msg);

		if (!this->get_payload_str().empty())
			_decoded.reset(new RTCMMessageType<Version, 0>(msg));
	}

	template <rtcm::RTCM_VERSION Version, rtcm::msgtype_type Type>
	void set_rtcm_payload(RTCMMessageType<Version, Type> &msg) {
		set_rtcm_payload(&msg);

		if (!this->get_payload_str().empty())
			_decoded.reset(new RTCMMessageType<Version, Type>(msg));
	}

	template <rtcm::RTCM_VERSION Version>
	void initialize(RTCMMessage<Version> &msg, const std::string source = "", unsigned int sourceId = 0, unsigned int flags = 0) {
		tmx::routeable_message::initialize(RTCMMessage<Version>::MessageType, msg.get_VersionName(), source, sourceId, flags);
		set_rtcm_payload(msg);
	}

	template <rtcm::RTCM_VERSION Version, rtcm::msgtype_type Type>
	void initialize(RTCMMessageType<Version, Type> &msg, const std::string source = "", unsigned int sourceId = 0, unsigned int flags = 0) {
		tmx::routeable_message::initialize(RTCMMessageType<Version, Type>::MessageType, msg.get_VersionName(), source, sourceId, flags);
		set_rtcm_payload(msg);
	}

	void initialize(TmxRtcmMessage &msg, const std::string source = "", unsigned int sourceId = 0, unsigned int flags = 0) {
		tmx::routeable_message::initialize(TmxRtcmMessage::MessageType, msg.get_VersionName(), source, sourceId, flags);
		set_rtcm_payload(&msg);
	}
private:
	rtcm::RtcmMessageFactory factory;

	void set_rtcm_payload(TmxRtcmMessage *msg) {
		if (msg) this->set_payload_bytes(msg->get_contents());
		else this->set_payload("");
	}

	std::shared_ptr<TmxRtcmMessage> _decoded;
};


} /* End namespace messages */

template <> template <>
inline messages::TmxRtcmMessage routeable_message::get_payload<messages::TmxRtcmMessage>()
{
	messages::TmxRtcmEncodedMessage encMsg(*this);
	auto ptr = encMsg.get_rtcm_payload();
	if (ptr) return *ptr;
	return messages::TmxRtcmMessage();
}

} /* End namespace tmx */



#endif /* INCLUDE_RTCM_RTCMENCODEDMESSAGE_H_ */
