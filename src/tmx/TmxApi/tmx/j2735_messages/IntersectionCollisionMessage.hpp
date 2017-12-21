/*
 * @file IntersectionCollisionMessage.hpp
 *
 *  Created on: May 8, 2016
 *      Author: Gregory M. Baumgardner
 */

#ifndef TMX_J2735_MESSAGES_INTERSECTIONCOLLISIONMESSAGE_HPP_
#define TMX_J2735_MESSAGES_INTERSECTIONCOLLISIONMESSAGE_HPP_

#include <IntersectionCollision.h>
#include <tmx/j2735_messages/J2735MessageTemplate.hpp>
#include <tmx/messages/TmxJ2735.hpp>

#if SAEJ2735_SPEC < 63
TMX_J2735_DECLARE(IntersectionCollision, IntersectionCollision, api::intersectionCollision_D, api::MSGSUBTYPE_INTERSECTIONCOLLISION_STRING)
#else
TMX_J2735_DECLARE(IntersectionCollision, IntersectionCollision, api::intersectionCollision, api::MSGSUBTYPE_INTERSECTIONCOLLISION_STRING)
#endif

#endif /* TMX_J2735_MESSAGES_INTERSECTIONCOLLISIONMESSAGE_HPP_ */
