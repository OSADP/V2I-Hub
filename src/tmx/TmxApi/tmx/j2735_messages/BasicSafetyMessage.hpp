/*
 * @file BsmMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_BASICSAFETYMESSAGE_HPP_
#define TMX_J2735_MESSAGES_BASICSAFETYMESSAGE_HPP_

#include <BasicSafetyMessage.h>
#include <tmx/messages/TmxJ2735.hpp>
#include <tmx/j2735_messages/J2735MessageTemplate.hpp>

#if SAEJ2735_SPEC < 63
TMX_J2735_DECLARE(Bsm, BasicSafetyMessage, api::basicSafetyMessage_D, api::MSGSUBTYPE_BASICSAFETYMESSAGE_STRING)
#else
TMX_J2735_DECLARE(Bsm, BasicSafetyMessage, api::basicSafetyMessage, api::MSGSUBTYPE_BASICSAFETYMESSAGE_STRING)
#endif

#endif /* TMX_J2735_MESSAGES_BASICSAFETYMESSAGE_HPP_ */
