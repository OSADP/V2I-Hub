/*
 * PvdDocument.h
 *
 *  Created on: Nov 8, 2016
 *      Author: svj
 */

#ifndef SRC_PvdDocument_H_
#define SRC_PvdDocument_H_

#include <vector>
#include "XmlDocument.h"
#include <ProbeVehicleData.h>
#include "WGS84Point.h"

namespace tmx
{
namespace utils
{

/**
 * Creates a PVD from a base XML string for manipulation using xpath.
 *
 * Note that tmx/messages/message_document.hpp could not be used because std11 is not supported on MIPS.
 * However, std11 users can call GetBasePvdString method to obtain the base XML string for further use.
 */
class PvdDocument : public XmlDocument
{
public:
	PvdDocument();
	virtual ~PvdDocument();

	static std::string GetBasePvdString();

	void set_cntSnapshots(uint32_t value);

	void set_StartVector(WGS84Point loc, double altitude, double heading, double speed_kph, uint64_t timestamp);
	void add_Snapshot(WGS84Point loc, double altitude, double heading, double speed_kph, uint64_t timestamp);

	void set_SegNum(std::string value);

	void set_Name(std::string value);

	void set_Vin(std::string value);
	void set_OwnerCode(std::string value);

	void set_VehicleType(VehicleType_t value);
	void set_VehicleType(std::string value);

	bool is_LatLongValid();

	std::string get_Name();

private:
	void fillLocationData(pugi::xml_node &node, WGS84Point loc, double altitude, double heading, double speed);
	void set_LatLong(pugi::xml_node &node, const WGS84Point& point);

	void set_Latitude(pugi::xml_node &node, double value);

	void set_Longitude(pugi::xml_node &node, double value);

	void set_Speed_kmph(pugi::xml_node &node, double value);

	void set_Elevation(pugi::xml_node &node, double value);

	void set_Heading_deg(pugi::xml_node &node, double value);
};

} /* namespace utils */
} /* namespace tmx */

#endif /* SRC_PvdDocument_H_ */
