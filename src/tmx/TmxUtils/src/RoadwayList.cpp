/*
 * RoadwayList.cpp
 *
 *  Created on: Jun 16, 2016
 *      Author: ivp
 */

#include "RoadwayList.h"

namespace tmx {
namespace utils {

RoadwayList::RoadwayList() {

}

RoadwayList::~RoadwayList() {

}

std::pair<Roadway*, bool> RoadwayList::LoadMap(
		tmx::messages::MapDataMessage &msg) {
	int roadwayId = msg.get<int>(QUERYROADSEGMENTMAPID, -1);

	//See if roadway id is already in our list

	Roadway i;
	auto a = _roadways.emplace(std::make_pair(roadwayId, i));
	//Return value: Returns a pair consisting of an iterator to the inserted element, or
	//the already-existing element if no insertion happened, and a bool denoting whether
	//the insertion took place. True for Insertion, False for No Insertion.
	if (a.second) {
		//return value of true means that the roadway id was new and was added.
		//TBD, check if our list is getting too long and if old one should be removed.
		//New items are added at position 0. so index 0 is roadway id and index 1 is the roadway object.
		//The second added MAP would be at indexes 2 and 3.
	} else {
		//roadway not added, value not new.
	}

	//a.first IS _Hashtable<int, pair<const int, Roadway>        a.second IS bool of 'if added'
	//               |
	//               |->_Hashtable{e.g. a.first}.first IS roadwayID   _Hashtable{e.g. a.first}.second IS Roadway object

	//Use the iterator of the inserted (or found) element returned by emplace to access the Roadway object stored in
	//the map. Update the map data.
	bool loadedNewMap = a.first->second.LoadMap(msg);

	//Returns a pair containing the Roadway of the MAP just loaded, and the bool indicating if the data triggered a refresh
	//or if it was unchanged data.

	std::pair<Roadway*, bool> ret(&(a.first->second), loadedNewMap);
	return ret;
}
}
} /* namespace ivputils */
