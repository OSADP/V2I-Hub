/*
 * @file ProbeVehicleDataMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_PROBEVEHICLEDATAMESSAGE_HPP_
#define TMX_J2735_MESSAGES_PROBEVEHICLEDATAMESSAGE_HPP_

#include <ProbeVehicleData.h>
#include <tmx/j2735_messages/J2735MessageTemplate.hpp>
#include <tmx/messages/TmxJ2735.hpp>

#if SAEJ2735_SPEC < 63
TMX_J2735_DECLARE(Pvd, ProbeVehicleData, api::probeVehicleData_D, api::MSGSUBTYPE_PROBEVEHICLEDATA_STRING)
#else
TMX_J2735_DECLARE(Pvd, ProbeVehicleData, api::probeVehicleData, api::MSGSUBTYPE_PROBEVEHICLEDATA_STRING)
#endif

#endif /* TMX_J2735_MESSAGES_PROBEVEHICLEDATAMESSAGE_HPP_ */
