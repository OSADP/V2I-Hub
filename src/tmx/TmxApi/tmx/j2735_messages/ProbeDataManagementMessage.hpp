/*
 * @file ProbeDataManagementMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_PROBEDATAMANAGEMENTMESSAGE_HPP_
#define TMX_J2735_MESSAGES_PROBEDATAMANAGEMENTMESSAGE_HPP_

#include <ProbeDataManagement.h>
#include <tmx/j2735_messages/J2735MessageTemplate.hpp>
#include <tmx/messages/TmxJ2735.hpp>

#if SAEJ2735_SPEC < 63
TMX_J2735_DECLARE(Pdm, ProbeDataManagement, api::probeDataManagement_D, api::MSGSUBTYPE_PROBEDATAMANAGEMENT_STRING)
#else
TMX_J2735_DECLARE(Pdm, ProbeDataManagement, api::probeDataManagement, api::MSGSUBTYPE_PROBEDATAMANAGEMENT_STRING)
#endif

#endif /* TMX_J2735_MESSAGES_PROBEDATAMANAGEMENTMESSAGE_HPP_ */
