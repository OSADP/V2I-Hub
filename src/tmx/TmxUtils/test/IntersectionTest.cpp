//============================================================================
// Name        : TestIntersection.cpp
// Description : Unit tests for Intersection.
//============================================================================

#include <gtest/gtest.h>
#include <Intersection.h>
#include <ParsedMap.h>
#include <tmx/j2735_messages/SpatMessage.hpp>
using namespace std;
using namespace tmx::utils;
using namespace tmx::messages;
using namespace pugi;
namespace tmx {
namespace utils {
class Intersection;
}
}
namespace unit_test {

class IntersectionTest: public testing::Test {
protected:
	IntersectionTest() {

	}

	virtual ~IntersectionTest() {
	}

	Intersection _intersection;
};

TEST_F(IntersectionTest,UpdateRegions) {

	//Test UpdateRegionForLaneId
	int reg = 0;
	reg = _intersection.UpdateRegionForLaneId(10);
	ASSERT_EQ(1, reg);
	ASSERT_EQ(1, _intersection.Regions[1].VehicleLanes.size());
	reg = _intersection.UpdateRegionForLaneId(19);
	ASSERT_EQ(1, reg);
	ASSERT_EQ(2, _intersection.Regions[1].VehicleLanes.size());
	reg = _intersection.UpdateRegionForLaneId(20);
	ASSERT_EQ(2, reg);
	ASSERT_EQ(1, _intersection.Regions[2].VehicleLanes.size());
	reg = _intersection.UpdateRegionForLaneId(29);
	ASSERT_EQ(2, reg);
	ASSERT_EQ(2, _intersection.Regions[2].VehicleLanes.size());
	reg = _intersection.UpdateRegionForLaneId(30);
	ASSERT_EQ(3, reg);
	ASSERT_EQ(1, _intersection.Regions[3].VehicleLanes.size());
	reg = _intersection.UpdateRegionForLaneId(39);
	ASSERT_EQ(3, reg);
	ASSERT_EQ(2, _intersection.Regions[3].VehicleLanes.size());
	reg = _intersection.UpdateRegionForLaneId(40);
	ASSERT_EQ(4, reg);
	ASSERT_EQ(1, _intersection.Regions[4].VehicleLanes.size());
	reg = _intersection.UpdateRegionForLaneId(49);
	ASSERT_EQ(4, reg);
	ASSERT_EQ(2, _intersection.Regions[4].VehicleLanes.size());

	reg = _intersection.UpdateRegionForLaneId(1);
	ASSERT_EQ(1, reg);
	ASSERT_EQ(1, _intersection.Regions[1].Crosswalks.size());
	reg = _intersection.UpdateRegionForLaneId(2);
	ASSERT_EQ(2, reg);
	ASSERT_EQ(1, _intersection.Regions[2].Crosswalks.size());
	reg = _intersection.UpdateRegionForLaneId(3);
	ASSERT_EQ(3, reg);
	ASSERT_EQ(1, _intersection.Regions[3].Crosswalks.size());
	reg = _intersection.UpdateRegionForLaneId(4);
	ASSERT_EQ(4, reg);
	ASSERT_EQ(1, _intersection.Regions[4].Crosswalks.size());

	reg = _intersection.UpdateRegionForLaneId(110);
	ASSERT_EQ(1, reg);
	ASSERT_EQ(1, _intersection.Regions[1].Curbs.size());
	reg = _intersection.UpdateRegionForLaneId(119);
	ASSERT_EQ(1, reg);
	ASSERT_EQ(2, _intersection.Regions[1].Curbs.size());
	reg = _intersection.UpdateRegionForLaneId(120);
	ASSERT_EQ(2, reg);
	ASSERT_EQ(1, _intersection.Regions[2].Curbs.size());
	reg = _intersection.UpdateRegionForLaneId(129);
	ASSERT_EQ(2, reg);
	ASSERT_EQ(2, _intersection.Regions[2].Curbs.size());
	reg = _intersection.UpdateRegionForLaneId(130);
	ASSERT_EQ(3, reg);
	ASSERT_EQ(1, _intersection.Regions[3].Curbs.size());
	reg = _intersection.UpdateRegionForLaneId(139);
	ASSERT_EQ(3, reg);
	ASSERT_EQ(2, _intersection.Regions[3].Curbs.size());
	reg = _intersection.UpdateRegionForLaneId(140);
	ASSERT_EQ(4, reg);
	ASSERT_EQ(1, _intersection.Regions[4].Curbs.size());
	reg = _intersection.UpdateRegionForLaneId(149);
	ASSERT_EQ(4, reg);
	ASSERT_EQ(2, _intersection.Regions[4].Curbs.size());

	MapLane lane31;
	lane31.LaneNumber = 31;
	_intersection.Map.Lanes.push_back(lane31); //add lane to map
	MapLane lane32;
	lane32.LaneNumber = 32;
	_intersection.Map.Lanes.push_back(lane32); //add lane to map
	ParsedMap map;
	//add lane to map
	map.Lanes.push_back(lane31);
	map.Lanes.push_back(lane32);
	_intersection.Map = map;
	_intersection.UpdateRegions();

	ASSERT_EQ(2, _intersection.Regions[3].VehicleLanes.size());

}
TEST_F(IntersectionTest, CheckRegionForVehicleLaneId) {

	Region one(1);

	one.VehicleLanes.push_back(11);
	one.VehicleLanes.push_back(12);
	one.VehicleLanes.push_back(13);

	int r = _intersection.CheckRegionForVehicleLaneId(one,11);
	ASSERT_EQ(1, r);
	r = _intersection.CheckRegionForVehicleLaneId(one,20);
	ASSERT_EQ(-1, r);
}
TEST_F(IntersectionTest, GetRegionForVehicleLaneId) {

	Region one(1);
	Region two(2);
	Region three(3);
	Region four(4);
	one.VehicleLanes.push_back(11);
	one.VehicleLanes.push_back(12);
	one.VehicleLanes.push_back(13);
	two.VehicleLanes.push_back(21);
	two.VehicleLanes.push_back(22);
	two.VehicleLanes.push_back(23);
	three.VehicleLanes.push_back(31);
	three.VehicleLanes.push_back(32);
	three.VehicleLanes.push_back(33);
	four.VehicleLanes.push_back(41);
	four.VehicleLanes.push_back(42);
	four.VehicleLanes.push_back(43);
	_intersection.Regions.push_back(one);
	_intersection.Regions.push_back(two);
	_intersection.Regions.push_back(three);
	_intersection.Regions.push_back(four);

	int reg;
	reg = _intersection.GetRegionForVehicleLaneId(11);
	ASSERT_EQ(1, reg);
	reg = _intersection.GetRegionForVehicleLaneId(12);
	ASSERT_EQ(1, reg);
	reg = _intersection.GetRegionForVehicleLaneId(13);
	ASSERT_EQ(1, reg);
	reg = _intersection.GetRegionForVehicleLaneId(21);
	ASSERT_EQ(2, reg);
	reg = _intersection.GetRegionForVehicleLaneId(22);
	ASSERT_EQ(2, reg);
	reg = _intersection.GetRegionForVehicleLaneId(23);
	ASSERT_EQ(2, reg);
	reg = _intersection.GetRegionForVehicleLaneId(31);
	ASSERT_EQ(3, reg);
	reg = _intersection.GetRegionForVehicleLaneId(32);
	ASSERT_EQ(3, reg);
	reg = _intersection.GetRegionForVehicleLaneId(33);
	ASSERT_EQ(3, reg);
	reg = _intersection.GetRegionForVehicleLaneId(41);
	ASSERT_EQ(4, reg);
	reg = _intersection.GetRegionForVehicleLaneId(42);
	ASSERT_EQ(4, reg);
	reg = _intersection.GetRegionForVehicleLaneId(43);
	ASSERT_EQ(4, reg);

	reg = _intersection.GetRegionForVehicleLaneId(26);//check one that doesn't exist.
	ASSERT_EQ(-1, reg);

}
TEST_F(IntersectionTest, GetRegionForCrosswalkLaneId) {

	Region one(1);
	Region two(2);
	Region three(3);
	Region four(4);
	one.Crosswalks.push_back(1);

	two.Crosswalks.push_back(2);

	three.Crosswalks.push_back(3);

	four.Crosswalks.push_back(4);

	_intersection.Regions.push_back(one);
	_intersection.Regions.push_back(two);
	_intersection.Regions.push_back(three);
	_intersection.Regions.push_back(four);

	int reg;
	reg = _intersection.GetRegionForCrosswalkLaneId(1);
	ASSERT_EQ(1, reg);
	reg = _intersection.GetRegionForCrosswalkLaneId(2);
	ASSERT_EQ(2, reg);
	reg = _intersection.GetRegionForCrosswalkLaneId(3);
	ASSERT_EQ(3, reg);
	reg = _intersection.GetRegionForCrosswalkLaneId(4);
	ASSERT_EQ(4, reg);


	reg = _intersection.GetRegionForCrosswalkLaneId(6);//check one that doesn't exist.
	ASSERT_EQ(-1, reg);

}
TEST_F(IntersectionTest, CheckRegionForCurbLaneId) {

	Region one(1);
	Region two(2);
	Region three(3);
	Region four(4);
	one.Curbs.push_back(111);
	one.Curbs.push_back(112);
	one.Curbs.push_back(113);
	two.Curbs.push_back(121);
	two.Curbs.push_back(122);
	two.Curbs.push_back(123);
	three.Curbs.push_back(131);
	three.Curbs.push_back(132);
	three.Curbs.push_back(133);
	four.Curbs.push_back(141);
	four.Curbs.push_back(142);
	four.Curbs.push_back(143);
	_intersection.Regions.push_back(one);
	_intersection.Regions.push_back(two);
	_intersection.Regions.push_back(three);
	_intersection.Regions.push_back(four);

	int reg;
	reg = _intersection.GetRegionForCurbLaneId(111);
	ASSERT_EQ(1, reg);
	reg = _intersection.GetRegionForCurbLaneId(112);
	ASSERT_EQ(1, reg);
	reg = _intersection.GetRegionForCurbLaneId(113);
	ASSERT_EQ(1, reg);
	reg = _intersection.GetRegionForCurbLaneId(121);
	ASSERT_EQ(2, reg);
	reg = _intersection.GetRegionForCurbLaneId(122);
	ASSERT_EQ(2, reg);
	reg = _intersection.GetRegionForCurbLaneId(123);
	ASSERT_EQ(2, reg);
	reg = _intersection.GetRegionForCurbLaneId(131);
	ASSERT_EQ(3, reg);
	reg = _intersection.GetRegionForCurbLaneId(132);
	ASSERT_EQ(3, reg);
	reg = _intersection.GetRegionForCurbLaneId(133);
	ASSERT_EQ(3, reg);
	reg = _intersection.GetRegionForCurbLaneId(141);
	ASSERT_EQ(4, reg);
	reg = _intersection.GetRegionForCurbLaneId(142);
	ASSERT_EQ(4, reg);
	reg = _intersection.GetRegionForCurbLaneId(143);
	ASSERT_EQ(4, reg);

	reg = _intersection.GetRegionForCurbLaneId(126);//check one that doesn't exist.
	ASSERT_EQ(-1, reg);

}
TEST_F(IntersectionTest, FindEgressRegion) {
	//Test FindEgressRegion
	int eRegion = 0;
	eRegion = _intersection.FindEgressRegion(1, Direction::Dir_Straight);
	ASSERT_EQ(3, eRegion);
	eRegion = _intersection.FindEgressRegion(1, Direction::Dir_Left);
	ASSERT_EQ(2, eRegion);
	eRegion = _intersection.FindEgressRegion(1, Direction::Dir_Right);
	ASSERT_EQ(4, eRegion);

	eRegion = _intersection.FindEgressRegion(2, Direction::Dir_Straight);
	ASSERT_EQ(4, eRegion);
	eRegion = _intersection.FindEgressRegion(2, Direction::Dir_Left);
	ASSERT_EQ(3, eRegion);
	eRegion = _intersection.FindEgressRegion(2, Direction::Dir_Right);
	ASSERT_EQ(1, eRegion);

	eRegion = _intersection.FindEgressRegion(3, Direction::Dir_Straight);
	ASSERT_EQ(1, eRegion);
	eRegion = _intersection.FindEgressRegion(3, Direction::Dir_Left);
	ASSERT_EQ(4, eRegion);
	eRegion = _intersection.FindEgressRegion(3, Direction::Dir_Right);
	ASSERT_EQ(2, eRegion);

	eRegion = _intersection.FindEgressRegion(4, Direction::Dir_Straight);
	ASSERT_EQ(2, eRegion);
	eRegion = _intersection.FindEgressRegion(4, Direction::Dir_Left);
	ASSERT_EQ(1, eRegion);
	eRegion = _intersection.FindEgressRegion(4, Direction::Dir_Right);
	ASSERT_EQ(3, eRegion);

}
TEST_F(IntersectionTest,GetPointFromNodeOffset){
	WGS84Point referencePoint = {39.9876814,-83.0207827};
	WGS84Point offset = {-70,58};
	WGS84Point point = _intersection.GetPointFromNodeOffset(referencePoint, offset);
//Latitude = 39.987686627756325, Longitude = -83.020790907190801}
	ASSERT_FLOAT_EQ( 39.9876866, point.Latitude);
	ASSERT_FLOAT_EQ(-83.0207909, point.Longitude);
}
TEST_F(IntersectionTest, IsSignalForGroupRedLight) {
	SpatMessage spat;
	spat.set_contents("<SPAT><msgID>19</msgID><intersections><IntersectionState><name>5th and Tisdale</name>   <id><id>1929</id> </id><revision>1</revision> <status> 0000000000000000</status><states><MovementState><signalGroup>1</signalGroup><state-time-speed><MovementEvent><eventState><stop-And-Remain/></eventState><timing> <minEndTime>32850</minEndTime><maxEndTime>32850</maxEndTime> </timing> </MovementEvent></state-time-speed>     	</MovementState></states></IntersectionState></intersections></SPAT>");
	string outputfield;
	bool isRed = _intersection.IsSignalForGroupRedLight(1, spat,outputfield);
	EXPECT_TRUE(isRed);
	isRed = _intersection.IsSignalForGroupRedLight(2, spat,outputfield);
	EXPECT_FALSE(isRed);
}
TEST_F(IntersectionTest, IsPedestrianReportedForLane) {
	SpatMessage spat;
	spat.set_contents("<SPAT><intersections><IntersectionState> <maneuverAssistList><ConnectionManeuverAssist><connectionID>4</connectionID><pedBicycleDetect><true/></pedBicycleDetect></ConnectionManeuverAssist></maneuverAssistList></IntersectionState></intersections></SPAT>");

	bool isPed = _intersection.IsPedestrianReportedForLane(4, spat);
	EXPECT_TRUE(isPed);
	isPed = _intersection.IsPedestrianReportedForLane(2, spat);
	EXPECT_FALSE(isPed);
}

TEST_F(IntersectionTest, LoadMap)
{
	tmx::messages::MapDataMessage mapData;
	mapData.set_contents("<MapData><msgID>18</msgID><msgIssueRevision>0</msgIssueRevision><layerType><intersectionData/></layerType><intersections><IntersectionGeometry><id><id>1929</id></id><revision>1</revision><refPoint><lat>399876814</lat><long>-30207827</long></refPoint><laneWidth>221</laneWidth><laneSet><GenericLane><laneID>11</laneID><laneAttributes><directionalUse>10</directionalUse><sharedWith>0000000000</sharedWith><laneType><vehicle>0000000000000000</vehicle></laneType></laneAttributes><maneuvers>000001110100</maneuvers><nodeList><nodes><Node><delta><node-XY2><x>50</x><y>870</y></node-XY2></delta><attributes><dWidth>138</dWidth></attributes></Node><Node><delta><node-XY2><x>20</x><y>680</y></node-XY2></delta><attributes><dWidth>138</dWidth></attributes></Node></nodes></nodeList><connectsTo><Connection><connectingLane><lane>23</lane><maneuver>000000000010</maneuver></connectingLane><signalGroup>4</signalGroup></Connection></connectsTo></GenericLane><GenericLane><laneID>12</laneID><laneAttributes><directionalUse>10</directionalUse><sharedWith>0000000000</sharedWith><laneType><vehicle>0000000000000000</vehicle></laneType></laneAttributes><maneuvers>000000111010</maneuvers><nodeList><computed><referenceLaneId>11</referenceLaneId><offsetXaxis><large>-360</large></offsetXaxis><offsetYaxis><large>0</large></offsetYaxis></computed></nodeList><connectsTo><Connection><connectingLane><lane>33</lane><maneuver>000000000001</maneuver></connectingLane><signalGroup>1</signalGroup></Connection><Connection><connectingLane><lane>43</lane><maneuver>000000000100</maneuver></connectingLane><signalGroup>1</signalGroup></Connection></connectsTo></GenericLane><GenericLane><laneID>13</laneID><laneAttributes><directionalUse>01</directionalUse><sharedWith>0000000000</sharedWith><laneType><vehicle>0000000000000000</vehicle></laneType></laneAttributes><maneuvers>000000000000</maneuvers><nodeList><computed><referenceLaneId>11</referenceLaneId><offsetXaxis><large>450</large></offsetXaxis><offsetYaxis><large>0</large></offsetYaxis></computed></nodeList></GenericLane></laneSet></IntersectionGeometry></intersections></MapData>");

	int mapRevision = _intersection.GetMapRevision(mapData);
	EXPECT_LT(-1,mapRevision);

	WGS84Point rederencePoint = _intersection.GetMapRReferencePoint(mapData);
	EXPECT_GT(rederencePoint.Latitude, 0);
	EXPECT_LT(rederencePoint.Longitude, 0);

	MapLane isVehicleMapLane = _intersection.GetMapLane(mapData, 11);
	EXPECT_TRUE(isVehicleMapLane.LaneNumber == 11);

	MapLane isComputedMapLane = _intersection.GetMapLane(mapData, 12);
	EXPECT_TRUE(isComputedMapLane.LaneNumber == 12);

	MapLane isEgressMapLane = _intersection.GetMapLane(mapData, 13);
	EXPECT_TRUE(isEgressMapLane.LaneNumber == 13);

	std::list<LaneNode> laneNodes = _intersection.GetLaneNodes(mapData, 11, 0, 0);
	EXPECT_TRUE(laneNodes.size() == 2);

}

//TEST_F(IntersectionTest, LoadMapmaxMinBox)
//{
//	tmx::messages::MapDataMessage mapData;
//	mapData.set_contents("<MapData><msgID>18</msgID><msgIssueRevision>0</msgIssueRevision><layerType><intersectionData/></layerType><intersections><IntersectionGeometry><id><id>1929</id></id><revision>1</revision><refPoint><lat>399876814</lat><long>-30207827</long></refPoint><laneWidth>221</laneWidth><laneSet><GenericLane><laneID>11</laneID><laneAttributes><directionalUse>10</directionalUse><sharedWith>0000000000</sharedWith><laneType><vehicle>0000000000000000</vehicle></laneType></laneAttributes><maneuvers>000001110100</maneuvers><nodeList><nodes><Node><delta><node-XY2><x>50</x><y>870</y></node-XY2></delta><attributes><dWidth>138</dWidth></attributes></Node><Node><delta><node-XY2><x>20</x><y>680</y></node-XY2></delta><attributes><dWidth>138</dWidth></attributes></Node></nodes></nodeList><connectsTo><Connection><connectingLane><lane>23</lane><maneuver>000000000010</maneuver></connectingLane><signalGroup>4</signalGroup></Connection></connectsTo></GenericLane><GenericLane><laneID>12</laneID><laneAttributes><directionalUse>10</directionalUse><sharedWith>0000000000</sharedWith><laneType><vehicle>0000000000000000</vehicle></laneType></laneAttributes><maneuvers>000000111010</maneuvers><nodeList><computed><referenceLaneId>11</referenceLaneId><offsetXaxis><large>-360</large></offsetXaxis><offsetYaxis><large>0</large></offsetYaxis></computed></nodeList><connectsTo><Connection><connectingLane><lane>33</lane><maneuver>000000000001</maneuver></connectingLane><signalGroup>1</signalGroup></Connection><Connection><connectingLane><lane>43</lane><maneuver>000000000100</maneuver></connectingLane><signalGroup>1</signalGroup></Connection></connectsTo></GenericLane><GenericLane><laneID>13</laneID><laneAttributes><directionalUse>01</directionalUse><sharedWith>0000000000</sharedWith><laneType><vehicle>0000000000000000</vehicle></laneType></laneAttributes><maneuvers>000000000000</maneuvers><nodeList><computed><referenceLaneId>11</referenceLaneId><offsetXaxis><large>450</large></offsetXaxis><offsetYaxis><large>0</large></offsetYaxis></computed></nodeList></GenericLane></laneSet></IntersectionGeometry></intersections></MapData>");
//
//
//	_intersection.LoadMap(mapData);
//
//}
}  // namespace
