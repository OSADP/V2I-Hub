/*
 * LocationMessage.h
 *
 *  Created on: Apr 21, 2016
 *      Author: ivp
 */

#ifndef INCLUDE_LOCATIONMESSAGE_H_
#define INCLUDE_LOCATIONMESSAGE_H_

#include <tmx/messages/message.hpp>

namespace tmx {
namespace messages {

enum class FixQualities
{
	 Invalid =0,
	 GPS_fix=1,
	DGPS_fix=2
};

enum class SignalQualityTypes {
	Invalid=0,
    GPS,
    DGPS,
    PPS,
    RealTimeKinematic,
    FloatRTK,
    DeadReckoning,
    ManualInputMode,
    SimulationMode
};

/**
 * LocationMessage is the message type used to send information messages about plugin status/activities.
 * It defines the message type and sub type and all data members.
 */
class LocationMessage : public tmx::message
{
public:
	LocationMessage() {}
	LocationMessage(const tmx::message_container_type &contents): tmx::message(contents) {}

	//These need to be in a place where they can be used everywhere
	static constexpr const char* MsgType_Decoded = "Decoded";
	static constexpr const char* MsgSubType_Location = "LocationMessage";

	/// Message type for routing this message through TMX core.
	static constexpr const char* MessageType = MsgType_Decoded;

	/// Message sub type for routing this message through TMX core.
	static constexpr const char* MessageSubType = MsgSubType_Location;

	std_attribute(this->msg, std::string, Id, "", )
	std_attribute(this->msg, SignalQualityTypes, SignalQuality, SignalQualityTypes::Invalid, )
	/**
		 * $GPGGA Global Positioning System Fix Data.Time, position and fix related data for a GPS receiver.
		 */
	std_attribute(this->msg, std::string, SentenceIdentifier, "", )
		/**
		 * hhmmss.ss = UTC of position. (ex: 170834	        is  17:08:34 Z)
		 */
	std_attribute(this->msg,std::string, Time, "", )
		/**
		 * llll.ll = latitude of position (ex: 4124.8963, N        is 	41d 24.8963' N or 41d 24' 54" N)
		 * 	a = N or S
		 */
	std_attribute(this->msg,double ,Latitude, 0, )
		/**
		 * 	yyyyy.yy = Longitude of position (ex: 08151.6838, W        is 81d 51.6838' W or 81d 51' 41" W)
		 * a = E or W
		 */
	std_attribute(this->msg,double, Longitude, 0, )
		/**
		 * 	x = GPS Quality indicator (0=no fix, 1=GPS fix, 2=Dif. GPS fix)
		 */
	std_attribute(this->msg,FixQualities, FixQuality, FixQualities::Invalid, )
		/**
		 * 	xx = number of satellites in use (ex: 	05	is 5 Satellites are in view)
		 */
	std_attribute(this->msg,int, NumSatellites, 0, )
		/**
		 * 	x.x = horizontal dilution of precision (ex: 1.5	is Relative accuracy of horizontal position)
		 */
	std_attribute(this->msg,double, HorizontalDOP, 0, )
		/**
		 * 	x.x = Antenna altitude above mean-sea-level (ex: 280.2, M	is   280.2 meters above mean sea level)
	M = units of antenna altitude, meters
		 */
	std_attribute(this->msg,double, Altitude, 0, )
		/**
		 * x.x = Geoidal separation  - Height of geoid above WGS84 ellipsoid.  (ex: -34.0, M	is   -34.0 meters)
	M = units of geoidal separation, meters
		 */
		//std_attribute(this->msg,double, GeoidalSeparation, 0, )
		/**
		 * Time since last DGPS update.
		 * x.x = Age of Differential GPS data (seconds)
		 */
		//std_attribute(this->msg,int, SecSinceLastUpdate, 0, )
		/**
		 * DGPS reference station id
		 * xxxx = Differential reference station ID
		 */
		//std_attribute(this->msg,std:string, RefStationId, 0, )
		/**
		 * Checksum. Used by program to check for transmission errors.
		 */
		//std_attribute(this->msg,int, Checksum, 0, )

		/**
		 * x.x,K = Speed, Km/hr
		 *  (ex: 010.2,K      Ground speed, Kilometers per hour)
		 */
	std_attribute(this->msg,double ,Speed, 0, )
		/**
		 * Heading in degrees.
		 */
	std_attribute(this->msg,double ,Heading, 0, )


		//eg2. $--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx

};

} /* namespace messages */
} /* namespace tmx */

#endif /* INCLUDE_LOCATIONMESSAGE_H_ */
