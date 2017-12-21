/*
 * IntersectionList.h
 *
 *  Created on: Jun 16, 2016
 *      Author: ivp
 */

#ifndef SRC_INTERSECTIONLIST_H_
#define SRC_INTERSECTIONLIST_H_
#include <iostream>
#include <unordered_map>
#include <hashtable.h>
#include "Intersection.h"
namespace tmx {
namespace utils {
///Tmx units may be within range of two or more MAP messages. Both of these need to be
///simultaneously maintained (for efficiency) within memory.
class IntersectionList {
public:
	IntersectionList();
	virtual ~IntersectionList();

	///Wrapper for Intersection.LoadMap.  Maintains a list of all MAPped Intersections. Returns a Pair containing
	///the intersection object resulting from the MapDataMessage, and true if the map was loaded new and false if it was unchanged data.
	std::pair<Intersection*, bool> LoadMap(tmx::messages::MapDataMessage &msg);

private:

	///Holds list of [IntersectionId, intersection object] for all encountered MAP files.
	  std::unordered_map<int, Intersection> _intersections;
};

} /* namespace ivputils */
}
#endif /* SRC_INTERSECTIONLIST_H_ */
