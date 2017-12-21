/*
 * PsmDocument.h
 *
 *  Created on: Nov 2, 2016
 *      Author: ivp
 */

#ifndef SRC_PSMDOCUMENT_H_
#define SRC_PSMDOCUMENT_H_

#include <list>
#include <PersonalSafetyMessage.h>
#include "WGS84Point.h"
#include "XmlDocument.h"

namespace tmx
{
namespace utils
{

struct HistoryPoint
{
	uint64_t Time_ms;
	int Latitude; // 1/10th microdegree.
	int Longitude; // 1/10th microdegree.
	int Elevation; // LSB decimeter.
};

/**
 * Creates a PSM from a base XML string for manipulation using xpath.
 *
 * Note that tmx/messages/message_document.hpp could not be used because std11 is not supported on MIPS.
 */
class PsmDocument : public XmlDocument
{
public:
	PsmDocument();
	virtual ~PsmDocument();
	static std::string GetBasePsmString();

	void set_BasicType(PersonalDeviceUserType_t value);

	void set_ClusterRadius(int value);

	void set_ClusterSize(NumberOfParticipantsInCluster_t value);

	void set_ClusterSizeFromNumberOfParticipants(int numberOfParticipants);

	void set_Id(uint32_t value);

	void set_LatLong(const WGS84Point& point);

	void set_Latitude(double value);

	void set_Longitude(double value);

	void set_Elevation(double value);

	void set_MsgCnt();

	void set_Speed_kmph(double value);

	void set_Heading_deg(double value);

	void update_PathHistory(bool setInitialPosition);

	void set_PathPrediction(int radiusOfCurve, int confidence);

	void set_PathPredictionToStraight(int confidence);

private:
	uint GetNextMsgCnt();

	/// Ever incrementing number to provide unique message ID for broadcasted PSMs.
	/// Rolls over after 127.
	uint _msgCnt;

	int _lastLatitude;
	int _lastLongitude;
	int _lastElevation;

	std::list<HistoryPoint> _historyList;
};

} /* namespace utils */
} /* namespace tmx */

#endif /* SRC_PSMDOCUMENT_H_ */
