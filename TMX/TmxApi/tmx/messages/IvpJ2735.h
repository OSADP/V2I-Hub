/*
 * IvpJ2735.h
 *
 *  Created on: Aug 7, 2014
 *      Author: ivp
 */

#ifndef IVPJ2735_H_
#define IVPJ2735_H_

#include "../tmx.h"
#include "../IvpMessage.h"
#include "../apimessages/IvpMessageType.h"
#include <stdio.h>

#include <asn_j2735_r41/DSRCmsgID.h>

#define IVPMSG_TYPE_J2735 "J2735"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
	void *msgStructure;
	e_DSRCmsgID msgId;
} IvpJ2735Msg;


typedef struct {
	uint8_t *msg;
	unsigned int msgLength;
	e_DSRCmsgID msgId;
} IvpJ2735EncodedMsg;

int ivpJ2735_isJ2735Msg(IvpMessage *msg);

IvpMessageTypeCollection *ivpJ2735_addMsgTypeToCollection(IvpMessageTypeCollection *collection, e_DSRCmsgID msgId);

IvpMessage *ivpJ2735_createMsg(void *msgStructure, e_DSRCmsgID msgId, IvpMsgFlags flags);
IvpMessage *ivpJ2735_createMsgFromEncoded(uint8_t *msg, unsigned int msgLength, IvpMsgFlags flags);
IvpMessage *ivpJ2735_createMsgFromEncodedwType(uint8_t *msg, unsigned int msgLength, IvpMsgFlags flags, const char * msgType);

IvpJ2735Msg *ivpJ2735_getJ2735Msg(IvpMessage *msg);
IvpJ2735EncodedMsg *ivpJ2735_getJ2735EncodedMsg(IvpMessage *msg);


void ivpJ2735_destroyJ2735Msg(IvpJ2735Msg *msg);
void ivpJ2735_destroyJ2735EncodedMsg(IvpJ2735EncodedMsg *msg);

IvpJ2735EncodedMsg *ivpJ2735_getJ2735EncodedMsg(IvpMessage *msg);

IvpJ2735Msg *ivpJ2735_decode(uint8_t *msg, unsigned int msgLength);

void ivpJ2735_fprint(FILE *stream, IvpJ2735Msg *msg);

#ifdef __cplusplus
}
#endif


#endif /* IVPJ2735_H_ */
