/*
 * IntersectionListTest.cpp
 *
 *  Created on: Jun 21, 2016
 *      Author: ivp
 */

#include <gtest/gtest.h>
#include <Intersection.h>
#include <IntersectionList.h>
#include <ParsedMap.h>
#include <tmx/j2735_messages/SpatMessage.hpp>
using namespace std;
using namespace tmx::utils;
using namespace tmx::messages;
using namespace pugi;
using namespace std;
using namespace tmx::utils;

namespace unit_test {

class IntersectionListTest: public testing::Test {
protected:
	IntersectionListTest() {

	}

	virtual ~IntersectionListTest() {
	}

	IntersectionList _intersection;
};

TEST_F(IntersectionListTest, LoadMap) {
	tmx::messages::MapDataMessage mapData;
	mapData.set_contents(
			"<MapData><msgID>18</msgID><msgIssueRevision>0</msgIssueRevision><layerType><intersectionData/></layerType><intersections><IntersectionGeometry><id><id>1929</id></id><revision>1</revision><refPoint><lat>399876814</lat><long>-30207827</long></refPoint><laneWidth>221</laneWidth><laneSet><GenericLane><laneID>11</laneID><laneAttributes><directionalUse>10</directionalUse><sharedWith>0000000000</sharedWith><laneType><vehicle>0000000000000000</vehicle></laneType></laneAttributes><maneuvers>000001110100</maneuvers><nodeList><nodes><Node><delta><node-XY2><x>50</x><y>870</y></node-XY2></delta><attributes><dWidth>138</dWidth></attributes></Node><Node><delta><node-XY2><x>20</x><y>680</y></node-XY2></delta><attributes><dWidth>138</dWidth></attributes></Node></nodes></nodeList><connectsTo><Connection><connectingLane><lane>23</lane><maneuver>000000000010</maneuver></connectingLane><signalGroup>4</signalGroup></Connection></connectsTo></GenericLane><GenericLane><laneID>12</laneID><laneAttributes><directionalUse>10</directionalUse><sharedWith>0000000000</sharedWith><laneType><vehicle>0000000000000000</vehicle></laneType></laneAttributes><maneuvers>000000111010</maneuvers><nodeList><computed><referenceLaneId>11</referenceLaneId><offsetXaxis><large>-360</large></offsetXaxis><offsetYaxis><large>0</large></offsetYaxis></computed></nodeList><connectsTo><Connection><connectingLane><lane>33</lane><maneuver>000000000001</maneuver></connectingLane><signalGroup>1</signalGroup></Connection><Connection><connectingLane><lane>43</lane><maneuver>000000000100</maneuver></connectingLane><signalGroup>1</signalGroup></Connection></connectsTo></GenericLane><GenericLane><laneID>13</laneID><laneAttributes><directionalUse>01</directionalUse><sharedWith>0000000000</sharedWith><laneType><vehicle>0000000000000000</vehicle></laneType></laneAttributes><maneuvers>000000000000</maneuvers><nodeList><computed><referenceLaneId>11</referenceLaneId><offsetXaxis><large>450</large></offsetXaxis><offsetYaxis><large>0</large></offsetYaxis></computed></nodeList></GenericLane></laneSet></IntersectionGeometry></intersections></MapData>");

	std::pair<Intersection*, bool> r = _intersection.LoadMap(mapData);

	bool loadedNewMap = r.second;
	Intersection inter = *(r.first);
	EXPECT_TRUE(loadedNewMap);
	//Confirm our data. lane id 11 should be found and 1.
	int laneid = inter.GetRegionForVehicleLaneId(11);
	ASSERT_EQ(1, laneid);
	//lane 18 should be not found.
	ASSERT_EQ(-1, inter.GetRegionForVehicleLaneId(18));
	tmx::messages::MapDataMessage mapData2;
	//differs from mapData above by id, lane id 11 changed to 18.
	mapData2.set_contents(
			"<MapData><msgID>18</msgID><msgIssueRevision>0</msgIssueRevision><layerType><intersectionData/></layerType><intersections><IntersectionGeometry><id><id>1979</id></id><revision>1</revision><refPoint><lat>399876814</lat><long>-30207827</long></refPoint><laneWidth>221</laneWidth><laneSet><GenericLane><laneID>18</laneID><laneAttributes><directionalUse>10</directionalUse><sharedWith>0000000000</sharedWith><laneType><vehicle>0000000000000000</vehicle></laneType></laneAttributes><maneuvers>000001110100</maneuvers><nodeList><nodes><Node><delta><node-XY2><x>50</x><y>870</y></node-XY2></delta><attributes><dWidth>138</dWidth></attributes></Node><Node><delta><node-XY2><x>20</x><y>680</y></node-XY2></delta><attributes><dWidth>138</dWidth></attributes></Node></nodes></nodeList><connectsTo><Connection><connectingLane><lane>23</lane><maneuver>000000000010</maneuver></connectingLane><signalGroup>4</signalGroup></Connection></connectsTo></GenericLane><GenericLane><laneID>12</laneID><laneAttributes><directionalUse>10</directionalUse><sharedWith>0000000000</sharedWith><laneType><vehicle>0000000000000000</vehicle></laneType></laneAttributes><maneuvers>000000111010</maneuvers><nodeList><computed><referenceLaneId>11</referenceLaneId><offsetXaxis><large>-360</large></offsetXaxis><offsetYaxis><large>0</large></offsetYaxis></computed></nodeList><connectsTo><Connection><connectingLane><lane>33</lane><maneuver>000000000001</maneuver></connectingLane><signalGroup>1</signalGroup></Connection><Connection><connectingLane><lane>43</lane><maneuver>000000000100</maneuver></connectingLane><signalGroup>1</signalGroup></Connection></connectsTo></GenericLane><GenericLane><laneID>13</laneID><laneAttributes><directionalUse>01</directionalUse><sharedWith>0000000000</sharedWith><laneType><vehicle>0000000000000000</vehicle></laneType></laneAttributes><maneuvers>000000000000</maneuvers><nodeList><computed><referenceLaneId>11</referenceLaneId><offsetXaxis><large>450</large></offsetXaxis><offsetYaxis><large>0</large></offsetYaxis></computed></nodeList></GenericLane></laneSet></IntersectionGeometry></intersections></MapData>");

	std::pair<Intersection*, bool> r2 = _intersection.LoadMap(mapData2);

	bool loadedNewMap2 = r2.second;
	Intersection inter2 = *(r2.first);
	EXPECT_TRUE(loadedNewMap2);
	//Confirm our data. lane id 11 should be not found.
	ASSERT_EQ(-1, inter2.GetRegionForVehicleLaneId(11));
	//lane 18 should be  found.
	ASSERT_EQ(1, inter2.GetRegionForVehicleLaneId(18));

	//Load same maps in again, should be false.
	std::pair<Intersection*, bool> ra = _intersection.LoadMap(mapData);
	bool loadedNewMapa = ra.second;
	Intersection intera = *(ra.first);
	EXPECT_FALSE(loadedNewMapa);
	//Confirm our data. lane id 11 should be  found.
	ASSERT_EQ(1, intera.GetRegionForVehicleLaneId(11));
	//lane 18 should be not found.
	ASSERT_EQ(-1, intera.GetRegionForVehicleLaneId(18));

	std::pair<Intersection*, bool> r2a = _intersection.LoadMap(mapData2);
	bool loadedNewMap2a = r2a.second;
	Intersection inter2a = *(r2a.first);
	EXPECT_FALSE(loadedNewMap2a);
	//Confirm our data. lane id 11 should be not found.
	ASSERT_EQ(-1, inter2a.GetRegionForVehicleLaneId(11));
	//lane 18 should be  found.
	ASSERT_EQ(1, inter2a.GetRegionForVehicleLaneId(18));

	//change the revision and should be true
	tmx::messages::MapDataMessage mapData2b;
	mapData2b.set_contents(
			"<MapData><msgID>18</msgID><msgIssueRevision>1</msgIssueRevision><layerType><intersectionData/></layerType><intersections><IntersectionGeometry><id><id>1979</id></id><revision>1</revision><refPoint><lat>399876814</lat><long>-30207827</long></refPoint><laneWidth>221</laneWidth><laneSet><GenericLane><laneID>18</laneID><laneAttributes><directionalUse>10</directionalUse><sharedWith>0000000000</sharedWith><laneType><vehicle>0000000000000000</vehicle></laneType></laneAttributes><maneuvers>000001110100</maneuvers><nodeList><nodes><Node><delta><node-XY2><x>50</x><y>870</y></node-XY2></delta><attributes><dWidth>138</dWidth></attributes></Node><Node><delta><node-XY2><x>20</x><y>680</y></node-XY2></delta><attributes><dWidth>138</dWidth></attributes></Node></nodes></nodeList><connectsTo><Connection><connectingLane><lane>23</lane><maneuver>000000000010</maneuver></connectingLane><signalGroup>4</signalGroup></Connection></connectsTo></GenericLane><GenericLane><laneID>12</laneID><laneAttributes><directionalUse>10</directionalUse><sharedWith>0000000000</sharedWith><laneType><vehicle>0000000000000000</vehicle></laneType></laneAttributes><maneuvers>000000111010</maneuvers><nodeList><computed><referenceLaneId>11</referenceLaneId><offsetXaxis><large>-360</large></offsetXaxis><offsetYaxis><large>0</large></offsetYaxis></computed></nodeList><connectsTo><Connection><connectingLane><lane>33</lane><maneuver>000000000001</maneuver></connectingLane><signalGroup>1</signalGroup></Connection><Connection><connectingLane><lane>43</lane><maneuver>000000000100</maneuver></connectingLane><signalGroup>1</signalGroup></Connection></connectsTo></GenericLane><GenericLane><laneID>13</laneID><laneAttributes><directionalUse>01</directionalUse><sharedWith>0000000000</sharedWith><laneType><vehicle>0000000000000000</vehicle></laneType></laneAttributes><maneuvers>000000000000</maneuvers><nodeList><computed><referenceLaneId>11</referenceLaneId><offsetXaxis><large>450</large></offsetXaxis><offsetYaxis><large>0</large></offsetYaxis></computed></nodeList></GenericLane></laneSet></IntersectionGeometry></intersections></MapData>");

	std::pair<Intersection*, bool> r2b = _intersection.LoadMap(mapData2b);
	bool loadedNewMap2b = r2b.second;
	Intersection inter2b = *(r2b.first);
	EXPECT_TRUE(loadedNewMap2b);
	//Confirm our data. lane id 11 should be not found.
	ASSERT_EQ(-1, inter2b.GetRegionForVehicleLaneId(11));
	//lane 18 should be  found.
	ASSERT_EQ(1, inter2b.GetRegionForVehicleLaneId(18));
}
}  // namespace

