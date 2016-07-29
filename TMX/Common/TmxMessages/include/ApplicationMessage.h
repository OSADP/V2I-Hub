/*
 * ApplicationMessage.h
 *
 *  Created on: Jun 7, 2016
 *      Author: ivp
 */

#ifndef INCLUDE_APPLICATIONMESSAGE_H_
#define INCLUDE_APPLICATIONMESSAGE_H_

#include <tmx/messages/message.hpp>
#include "ApplicationMessageTypes.h"

namespace tmx {
namespace messages {

/**
 * ApplicationMessage is the message type used to send information messages about various application statuses.
 * It defines the message type and sub type and all data members.
 */
class ApplicationMessage : public tmx::message
{
public:
	ApplicationMessage() {}
	ApplicationMessage(const tmx::message_container_type &contents): tmx::message(contents) {}
	ApplicationMessage(std::string id, appmessage::ApplicationId appId, std::string eventID,
			std::string timestamp, std::string displayDuration, appmessage::SeverityId severity,
			appmessage::EventCodeId eventCode, std::string interactionId, std::string customText ) {
		set_Id(id);
		set_AppId(appId);
		set_EventID(eventID);
		set_Timestamp(timestamp);
		set_DisplayDuration(displayDuration);
		set_Severity(severity);
		set_EventCode(eventCode);
		set_InteractionId(interactionId);
		set_CustomText(customText);
	}

	/// Message type for routing this message through TMX core.
	static constexpr const char* MessageType = "Application";

	/// Message sub type for routing this message through TMX core.
	static constexpr const char* MessageSubType = "Basic";

	//unique guid
	std_attribute(this->msg, std::string, Id, "", )

	//Id of the application unique string
	std_attribute(this->msg, appmessage::ApplicationId, AppId, appmessage::ApplicationId::NOAPPID, )

	//Unique message identifier for repeated notification
	std_attribute(this->msg, std::string, EventID, "", )

	//Timestamp of the event
	std_attribute(this->msg, std::string, Timestamp, "", )

	//How long to display in milliseconds
	std_attribute(this->msg, std::string, DisplayDuration, "", )

	//Info or InformAlert or WarnAler
	std_attribute(this->msg, appmessage::SeverityId, Severity, appmessage::SeverityId::NOSEVERITY, )

	//Code from master list of possible events
	std_attribute(this->msg, appmessage::EventCodeId, EventCode, appmessage::EventCodeId::NOEVENTID, )

	//null/not present if not currently interacting with thing of interest.
	std_attribute(this->msg, std::string, InteractionId, "", )

	//Open text field for specific messages related to this event
	std_attribute(this->msg, std::string, CustomText, "", )

};

} /* namespace messages */
} /* namespace tmx */



#endif /* INCLUDE_APPLICATIONMESSAGE_H_ */
