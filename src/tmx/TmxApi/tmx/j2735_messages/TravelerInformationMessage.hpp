/*
 * @file TravelerInformationMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_TRAVELERINFORMATIONMESSAGE_HPP_
#define TMX_J2735_MESSAGES_TRAVELERINFORMATIONMESSAGE_HPP_

#include <TravelerInformation.h>
#include <tmx/j2735_messages/J2735MessageTemplate.hpp>
#include <tmx/messages/TmxJ2735.hpp>

#if SAEJ2735_SPEC < 63
TMX_J2735_DECLARE(Tim, TravelerInformation, api::travelerInformation_D, api::MSGSUBTYPE_TRAVELERINFORMATION_STRING)
#else
TMX_J2735_DECLARE(Tim, TravelerInformation, api::travelerInformation, api::MSGSUBTYPE_TRAVELERINFORMATION_STRING)
#endif

#endif /* TMX_J2735_MESSAGES_TRAVELERINFORMATIONMESSAGE_HPP_ */
