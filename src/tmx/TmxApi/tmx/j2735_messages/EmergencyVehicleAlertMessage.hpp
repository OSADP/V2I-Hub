/*
 * @file EmergencyVehicleAlertMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_EMERGENCYVEHICLEALERTMESSAGE_HPP_
#define TMX_J2735_MESSAGES_EMERGENCYVEHICLEALERTMESSAGE_HPP_

#include <EmergencyVehicleAlert.h>
#include <tmx/j2735_messages/J2735MessageTemplate.hpp>
#include <tmx/messages/TmxJ2735.hpp>

#if SAEJ2735_SPEC < 63
TMX_J2735_DECLARE(Eva, EmergencyVehicleAlert, api::emergencyVehicleAlert_D, api::MSGSUBTYPE_EMERGENCYVEHICLEALERT_STRING)
#else
TMX_J2735_DECLARE(Eva, EmergencyVehicleAlert, api::emergencyVehicleAlert, api::MSGSUBTYPE_EMERGENCYVEHICLEALERT_STRING)
#endif

#endif /* TMX_J2735_MESSAGES_EMERGENCYVEHICLEALERTMESSAGE_HPP_ */
