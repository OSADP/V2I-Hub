/*
 * IntersectionList.cpp
 *
 *  Created on: Jun 16, 2016
 *      Author: ivp
 */

#include "IntersectionList.h"

namespace tmx {
namespace utils {

IntersectionList::IntersectionList() {

}

IntersectionList::~IntersectionList() {

}

std::pair<Intersection*, bool> IntersectionList::LoadMap(
		tmx::messages::MapDataMessage &msg) {
	int intersectionId = msg.get<int>(
			"MapData.intersections.IntersectionGeometry.id.id", -1);

	//See if intersection id is already in our list

	Intersection i;
	auto a = _intersections.emplace(std::make_pair(intersectionId, i));
	//Return value: Returns a pair consisting of an iterator to the inserted element, or
	//the already-existing element if no insertion happened, and a bool denoting whether
	//the insertion took place. True for Insertion, False for No Insertion.
	if (a.second) {
		//return value of true means that the intersection id was new and was added.
		//TBD, check if our list is getting too long and if old one should be removed.
		//New items are added at position 0. so index 0 is intersection id and index 1 is the intersection object.
		//The second added MAP would be at indexes 2 and 3.
	} else {
		//intersection not added, value not new.
	}

	//a.first IS _Hashtable<int, pair<const int, Intersection>        a.second IS bool of 'if added'
	//               |
	//               |->_Hashtable{e.g. a.first}.first IS intersectionID   _Hashtable{e.g. a.first}.second IS Intersection object

	//Use the iterator of the inserted (or found) element returned by emplace to access the Intersection object stored in
	//the map. Update the map data.
	bool loadedNewMap = a.first->second.LoadMap(msg);

	//Returns a pair containing the Intersection of the MAP just loaded, and the bool indicating if the data triggered a refresh
	//or if it was unchanged data.

	std::pair<Intersection*, bool> ret(&(a.first->second), loadedNewMap);
	return ret;
}
}
} /* namespace ivputils */
