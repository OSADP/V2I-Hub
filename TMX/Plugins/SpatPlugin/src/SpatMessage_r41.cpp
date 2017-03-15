#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <asn_j2735_r41/SPAT.h>
#include <asn_j2735_r41/UPERframe.h>

#include "SpatMessage_r41.h"
//#include "SPaTData.h"


SpatMessage_r41::SpatMessage_r41()
{
	std::cout << "SpatMessage_r41::SpatMessage_r41 Constructor Called" << std::endl;
	frame = (UPERframe *) calloc(1, sizeof(UPERframe));

	encodedBytes = 0;
	derEncodedBytes = 0;
	iterations = 0;
}

SpatMessage_r41::~SpatMessage_r41()
{

}

int SpatMessage_r41::createSpat()
{
	ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_SPAT, &spatMessage);

	// TODO: SPAT-P Not defined from ASN.1 using literal instead
	spatMessage.msgID = 0x13; // SPAT-P

	IntersectionState *intersection = (IntersectionState *)calloc(1, sizeof(IntersectionState));

	intersection->name = (DescriptiveName_t *) calloc(1, sizeof(DescriptiveName_t));

	intersection->name->size = strlen(spatData->IntersectionName);
	intersection->name->buf = (uint8_t *) calloc(1, strlen(spatData->IntersectionName));
	memcpy(intersection->name->buf, spatData->IntersectionName, strlen(spatData->IntersectionName));
	intersection->id.id = spatData->IntersectionId;
	intersection->revision = (MsgCount_t) 1;

//	intersection->status = IntersectionStatusObject_fixedTimeOperation;

	uint16_t statusIntersection = createIntersectionStatus(statusIntersection);

	intersection->status.buf = (uint8_t *)calloc(2, sizeof(uint8_t));
	intersection->status.size = 2 * sizeof(uint8_t);
	intersection->status.bits_unused = 0;
	intersection->status.buf[1] = statusIntersection;
	intersection->status.buf[0] = (statusIntersection >> 8);

	// Movement List
	for (int m = 0; m < spatData->NumPTLMRecords; m++)
	{
		MovementState *movement = (MovementState *) calloc(1, sizeof(MovementState));
		movement->signalGroup = spatData->PTLMTable[m].SignalGroup;

		MovementEvent *stateTimeSpeed = (MovementEvent *) calloc(1, sizeof(MovementEvent));
		if (spatData->PTLMTable[m].LanesType == pedLane)
		{
			if (spatData->PTLMTable[m].state == 0x03)		{ stateTimeSpeed->eventState = MovementPhaseState_permissive_Movement_Allowed; }
			else if (spatData->PTLMTable[m].state == 0x02)		{ stateTimeSpeed->eventState = MovementPhaseState_permissive_clearance; }
			else if (spatData->PTLMTable[m].state == 0x01)		{ stateTimeSpeed->eventState = MovementPhaseState_stop_And_Remain; }
		}
		else
		{
			if (spatData->PTLMTable[m].state == DarkBall)
			{
				stateTimeSpeed->eventState = MovementPhaseState_dark;
			}
			else if ((spatData->PTLMTable[m].state & GreenBall) ||
					(spatData->PTLMTable[m].state & GreenLeftArrow) ||
					(spatData->PTLMTable[m].state & GreenRightArrow)){
				if(spatData->PTLMTable[m].PhaseType == protectedPhase)
					stateTimeSpeed->eventState = MovementPhaseState_protected_Movement_Allowed;
				else
					stateTimeSpeed->eventState = MovementPhaseState_permissive_Movement_Allowed;
			}
			else if ((spatData->PTLMTable[m].state & YellowBall) ||
					(spatData->PTLMTable[m].state & YellowLeftArrow) ||
					(spatData->PTLMTable[m].state & YellowRightArrow)){
				if(spatData->PTLMTable[m].PhaseType == protectedPhase)
					stateTimeSpeed->eventState = MovementPhaseState_protected_clearance;
				else
					stateTimeSpeed->eventState = MovementPhaseState_permissive_clearance;
			}
			else if ((spatData->PTLMTable[m].state & RedBall) ||
					(spatData->PTLMTable[m].state & RedLeftArrow) ||
					(spatData->PTLMTable[m].state & RedRightArrow))
			{
				stateTimeSpeed->eventState = MovementPhaseState_stop_And_Remain;
			}
		}

		if (iterations == 20)
		{
			std::cout << "SigGrp: "<< spatData->PTLMTable[m].SignalGroup << " PTLMState: " << spatData->PTLMTable[m].state << " LightState: " << stateTimeSpeed->eventState << std::endl;
		}

		stateTimeSpeed->timing = (TimeChangeDetails * ) calloc(1, sizeof(TimeChangeDetails));
		stateTimeSpeed->timing->minEndTime =  getAdjustedTime(spatData->PTLMTable[m].minTime);
		if (spatData->PTLMTable[m].maxTime > 0)
		{
			stateTimeSpeed->timing->maxEndTime = (TimeMark_t *) calloc(1, sizeof(TimeMark_t));
			*(stateTimeSpeed->timing->maxEndTime) = getAdjustedTime(spatData->PTLMTable[m].maxTime);
		}
//		spatData->PTLMTable[m].callActive

		if (spatData->PTLMTable[m].pedestrian == pc_oneormore)
		{
			movement->maneuverAssistList = (ManeuverAssistList *) calloc(1, sizeof(ManeuverAssistList));
			ConnectionManeuverAssist *pedDetect = (ConnectionManeuverAssist *) calloc(1, sizeof(ConnectionManeuverAssist));
			pedDetect->connectionID = 0;
			pedDetect->pedBicycleDetect = (PedestrianBicycleDetect_t *) calloc(1, sizeof(PedestrianBicycleDetect_t));

			*(pedDetect->pedBicycleDetect) = 1;
			ASN_SEQUENCE_ADD(&movement->maneuverAssistList->list, pedDetect);
		}
		ASN_SEQUENCE_ADD(&movement->state_time_speed.list, stateTimeSpeed);
		ASN_SEQUENCE_ADD(&intersection->states.list, movement);
	}
	ASN_SEQUENCE_ADD(&spatMessage.intersections.list, intersection);

	asn_enc_rval_t rvalenc = uper_encode_to_buffer(&asn_DEF_SPAT, &spatMessage, encoded, 4000);
	if (rvalenc.encoded == -1) {
		fprintf(stderr, "Cannot encode %s\n", rvalenc.failed_type->name);
		return -1;
	} else {
		encodedBytes = ((rvalenc.encoded + 7) / 8);
		if (createUPERframe_DERencoded_msg())
		{

#if 0
			// Print every 2 seconds
			if (iterations == 20)
			{
				xer_fprint(stdout, &asn_DEF_SPAT, &spatMessage);

				printf("Structure successfully encoded %d\n", encodedBytes);

				int i;
				for(i = 0; i < encodedBytes; i++)
				{
					printf("%02x ", (unsigned char) encoded[i]);
				}
				printf("\n");
				std::cout << "Spat Successfully DER encoded:" << derEncodedBytes << std::endl;

				xer_fprint(stdout, &asn_DEF_UPERframe, frame);

				// Print DER Encoded Message
				printf("SPAT successfully DER encoded size: %d\nPayload: ", derEncodedBytes);
				for (i=0; i < derEncodedBytes; i++) { printf("%02x ", derEncoded[i]); }
				printf("\n");



			}
#endif
		}
		else return false;

	}


	if (iterations == 20)
	{
		printf("\n");
		iterations = 0;
	}
	iterations++;
	return true;
}

bool SpatMessage_r41::createUPERframe_DERencoded_msg()
{
	ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_UPERframe, frame);
	frame->msgID = 0x11;
	frame->contentID = 0x13;

	frame->msgBlob.buf = (uint8_t *) calloc(encodedBytes, sizeof(uint8_t));
	frame->msgBlob.size = encodedBytes;
	memcpy(frame->msgBlob.buf, encoded, encodedBytes);



	asn_enc_rval_t encResults = der_encode_to_buffer(&asn_DEF_UPERframe, frame, &derEncoded, 4000);
	if (encResults.encoded == 0)
	{
		std::cout << "Did not Successfully DER encode message" << std::endl;
		return false;
	}
	else
	{
		derEncodedBytes = encResults.encoded;
	}
	return true;
}

uint16_t SpatMessage_r41::createIntersectionStatus(unsigned short intStatus)
{
//	printf("Int Status:%x\n", intStatus);
	uint16_t intersectionBitString = 0;

	if (intStatus & is_normal)		{ intersectionBitString |= 1 << (15 - IntersectionStatusObject_fixedTimeOperation); }
	if (intStatus & is_manual)		{ intersectionBitString |= 1 << (15 - IntersectionStatusObject_manualControlIsEnabled); }
	if (intStatus & is_stopped)		{ intersectionBitString |= 1 << (15 - IntersectionStatusObject_stopTimeIsActivated); }
	if (intStatus & is_faultFlash)	{ intersectionBitString |= 1 << (15 - IntersectionStatusObject_manualControlIsEnabled); }
	if (intStatus & is_preempt)		{ intersectionBitString |= 1 << (15 - IntersectionStatusObject_preemptIsActive); }
	if (intStatus & is_priority)	{ intersectionBitString |= 1 << (15 - IntersectionStatusObject_transitSignalPriorityIsActive); }
	if (intStatus & is_coordination)	{ intersectionBitString |= 1 << (15 - IntersectionStatusObject_manualControlIsEnabled); }

	return intersectionBitString;
}

/*
	DarkBall = 0x00,
	GreenBall = 0x01,
	YellowBall = 0x02,
	RedBall = 0x04,
 */


uint16_t SpatMessage_r41::createEventState(unsigned short evStatus)
{
	uint16_t eventBitString = 0;
//	if (evStatus & unavailablePhaseStatus)		{ eventBitString |= 1 << (15 - MovementPhaseState_unavailable); }
	if (evStatus & DarkBall)		{ eventBitString |= 1 << (15 - MovementPhaseState_dark); }
	if (evStatus & GreenBall)		{ eventBitString |= 1 << (15 - MovementPhaseState_permissive_Movement_Allowed); }
	if (evStatus & YellowBall)		{ eventBitString |= 1 << (15 - MovementPhaseState_permissive_clearance); }
	if (evStatus & RedBall)			{ eventBitString |= 1 << (15 - MovementPhaseState_stop_And_Remain); }
	return eventBitString;
}

// TODO move to common file
void SpatMessage_r41::printUINT16(uint16_t num)
{
    uint16_t x = num;
    for (int i = 0; i < 16; i++) {
        printf("%d", (x & 0x8000) >> 15);
        x <<= 1;
    }
    printf("\n");
}

void SpatMessage_r41::setSpatData(SPaTData* sd)
{
	spatData = sd;
}

long SpatMessage_r41::getAdjustedTime(unsigned int offset)
{
	time_t now = time(0);
	time_t now_plus_offsets = now + offset;
	struct tm now_tm = *gmtime(&now_plus_offsets);
	long retTime = ((now_tm.tm_min * 60) + now_tm.tm_sec) * 10;
//	printf("Adjustment %d Current Time: %02d:%02d:%02d Return Value:%ld\n", offset, now_tm.tm_hour, now_tm.tm_min, now_tm.tm_sec, retTime);

	return retTime;
}

