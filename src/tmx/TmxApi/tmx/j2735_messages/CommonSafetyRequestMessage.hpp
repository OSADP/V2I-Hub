/*
 * @file CommonSafetyRequestMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_COMMONSAFETYREQUESTMESSAGE_HPP_
#define TMX_J2735_MESSAGES_COMMONSAFETYREQUESTMESSAGE_HPP_

#include <CommonSafetyRequest.h>
#include <tmx/j2735_messages/J2735MessageTemplate.hpp>
#include <tmx/messages/TmxJ2735.hpp>

#if SAEJ2735_SPEC < 63
TMX_J2735_DECLARE(Csr, CommonSafetyRequest, api::commonSafetyRequest_D, api::MSGSUBTYPE_COMMONSAFETYREQUEST_STRING)
#else
TMX_J2735_DECLARE(Csr, CommonSafetyRequest, api::commonSafetyRequest, api::MSGSUBTYPE_COMMONSAFETYREQUEST_STRING)
#endif

#endif /* TMX_J2735_MESSAGES_COMMONSAFETYREQUESTMESSAGE_HPP_ */
