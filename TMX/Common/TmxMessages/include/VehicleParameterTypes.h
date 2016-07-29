/*
 * VehicleParamaterTypes.h
 *
 *  Created on: Jun 10, 2016
 *      Author: ivp
 */

#ifndef INCLUDE_VEHICLEPARAMETERTYPES_H_
#define INCLUDE_VEHICLEPARAMETERTYPES_H_

namespace vehicleparam
{
///For sending the gear state value within the vehicle basic message.
enum GearState
{
	GearUnknown = 0,
	Park=1,
	Reverse=2,
	Drive=3,
	Other=4
};

///For sending turn signal value within the vehicle basic message.
enum TurnSignalState
{
	SignalUnknown=0,
	Off=1,
	Left=2,
	Right=3
};

///Grouping category for VehicleStateChange message.
	enum VehicleParameter
	{
		ParameterNA=0,
		GearPosition=1,
		TurnSignal=2,
		Brake=3
	};
	///Detail category for VehicleStateChange message.
	enum VehicleParameterState
	{
		StateNA=0,
		Gear_Park=1,
		Gear_Reverse=2,
		Gear_Drive=3,
		Gear_Other=4,
		Turn_Off=5,
		Turn_Left=6,
		Turn_Right=7,
		Brake_On=8,
		Brake_Off=9
	};

}

#endif /* INCLUDE_VEHICLEPARAMETERTYPES_H_ */
