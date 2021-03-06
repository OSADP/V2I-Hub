/*
 * Generated by asn1c-0.9.27 (http://lionet.info/asn1c)
 * From ASN.1 module "DSRC"
 * 	found in "../J2735_R41_Source_mod.ASN"
 * 	`asn1c -gen-PER -fcompound-names -fincludes-quoted`
 */

#ifndef	_IntersectionCollision_H_
#define	_IntersectionCollision_H_


#include "asn_application.h"

/* Including external dependencies */
#include "DSRCmsgID.h"
#include "MsgCount.h"
#include "TemporaryID.h"
#include "DSecond.h"
#include "PathHistory.h"
#include "IntersectionID.h"
#include "LaneNumber.h"
#include "EventFlags.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* IntersectionCollision */
typedef struct IntersectionCollision {
	DSRCmsgID_t	 msgID;
	MsgCount_t	 msgCnt;
	TemporaryID_t	 id;
	DSecond_t	*secMark	/* OPTIONAL */;
	PathHistory_t	 path;
	IntersectionID_t	 intersetionID;
	LaneNumber_t	 laneNumber;
	EventFlags_t	 eventFlag;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} IntersectionCollision_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_IntersectionCollision;

#ifdef __cplusplus
}
#endif

#endif	/* _IntersectionCollision_H_ */
#include "asn_internal.h"
