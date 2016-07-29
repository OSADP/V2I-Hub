/*
 * ApplicationMessageTypes.h
 *
 *  Created on: Jun 7, 2016
 *      Author: ivp
 */

#ifndef INCLUDE_APPLICATIONMESSAGETYPES_H_
#define INCLUDE_APPLICATIONMESSAGETYPES_H_

namespace appmessage {

enum ApplicationId
{
	NOAPPID = 0,
	CSW,
	FCW,
	EEBL,
	INFLO,
	RESCUMECS,
	EPCW,
	EVTRW,
	TSPW,
	RCI,
	RCVW
};

enum EventCodeId
{
	NOEVENTID = 0,
	ApplicationActive=1,
	ApplicationInactive=2,
	EnteredArea=3,
	ExitedArea=4,
	DetectedArea=5,
	NearsidePed=6,
	FarsidePed=7,
	LeftsidePed=8,
	RightsidePed=9,
	OnCurbsidePed=10,
	InRoadwayPed=11,
	AheadVeh=12,
	AheadLeftVeh=13,
	AheadRightVeh=14,
	LeftVeh=15,
	RightVeh=16,
	BehindLeftVeh=17,
	BehindRightVeh=18,
	BehindVeh=19,
	EventComplete=20,
	InArea=21,
    TrackingVeh=22
};

enum SeverityId
{
	NOSEVERITY=0,
	INFORM=1,
	WARNING=2
};
}

#endif /* INCLUDE_APPLICATIONMESSAGETYPES_H_ */
