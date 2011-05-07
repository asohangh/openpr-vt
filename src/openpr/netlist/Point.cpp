/*
 * Point.cpp
 *
 *  Created on: Mar 8, 2010
 *      Author: jacouch
 */
// This program is free software: you can redistribute it and/or modify it under the terms of the
// GNU General Public License as published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
// the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program.  If
// not, see <http://www.gnu.org/licenses/>.
#ifndef POINT_HPP_
#define POINT_HPP_
#include "openpr/netlist/Point.h"
namespace openpr {
namespace netlist {
//necessary place holders
Point::~Point() {

}
/**
 * This is the normal constructor
 * @param tile  This is the tile name for a source destination pair.
 * @param sourceDestination  This is either the source or destination location for a point
 * @param source  if it is a source it is true if it is a destination it is false
 */
Point::Point(std::string tile, std::string sourceDestination, bool source,
		torc::architecture::DDB* mDB) {
	this->tile = tile;
	this->sourceDestination = sourceDestination;
	this->source = source;
	if (source && mDB != NULL) {
		this->segmentIndex = mDB->getSegments().getTilewireSegment(mDB->lookupTilewire(tile, sourceDestination)).getCompactSegmentIndex();
	} else if (mDB != NULL) {
		this->segmentIndex = mDB->getSegments().getTilewireSegment(mDB->lookupTilewire(tile, sourceDestination)).getCompactSegmentIndex();
	} else {
		this->segmentIndex = torc::architecture::xilinx::CompactSegmentIndex(0);
	}
}
/**
 * @param other other point to be compared to this
 * This is the equality operator it compares all three  of tile, sourcedestination and (bool)source
 */
bool Point::operator==(const Point &other) const {
	if ((this->source == other.source) && (this->sourceDestination
			== other.sourceDestination) && (this->tile == other.tile))
	//&& (this->segmentIndex && other.segmentIndex))
	{
		return true;
	}
	return false;
}
/**
 * @param other other point to be comparied to this
 * This is the equality operator it compares all three paramaters of tile, sourcedestination and (bool)source
 */
bool Point::operator==(const Point* other) const {
	if ((this->source == other->source) && (this->sourceDestination
			== other->sourceDestination) && (this->tile == other->tile))
	//&& (this->segmentIndex == other->segmentIndex))
	{
		return true;
	}
	return false;
}
/**
 * @param x point to be investigated
 * This function evaluates the hash of a point.  If is effectively a static function
 */
size_t Point::operator()(const Point& x) const {
	std::size_t seed = 0;
	boost::hash_combine(seed, x.source);
	boost::hash_combine(seed, x.sourceDestination);
	boost::hash_combine(seed, x.tile);
	//boost::hash_combine(seed, x.segmentIndex);
	return seed;
}
/**
 * @param x point to be investigated
 * This function evaluates the hash of a point.  If is effectively a static function
 */
size_t Point::operator()(const Point* x) const {
	std::size_t seed = 0;
	boost::hash_combine(seed, x->source);
	boost::hash_combine(seed, x->sourceDestination);
	boost::hash_combine(seed, x->tile);
	//boost::hash_combine(seed, x->segmentIndex);
	return seed;
}
/**
 * @param other this is the other point to be compared against this
 * Boolean operator overload
 * Only compares tile and the sourceDestination.  (bool)source is not evaluated
 */
bool Point::operator>(const Point &other) const {
	if (this->tile > other.tile) {
		return true;
	}
	if (this->tile == other.tile) {
		if (this->sourceDestination > other.sourceDestination) {
			return true;
		}
		return false;
	}
	return false;

}
/**
 * @param other this is the other point to be compared against this
 * Boolean operator overload
 * Only compares tile and the sourceDestination.  (bool)source is not evaluated
 */
bool Point::operator>(const Point* other) const {
	if (this->tile > other->tile) {
		return true;
	}
	if (this->tile == other->tile) {
		if (this->sourceDestination > other->sourceDestination) {
			return true;
		}
		return false;
	}
	return false;
}
/**
 * @param other this is the other point to be compared against this
 * Boolean operator overload
 * Only compares tile and the sourceDestination.  (bool)source is not evaluated
 */
bool Point::operator<(const Point &other) const {
	if (this->tile < other.tile) {
		return true;
	}
	if (this->tile == other.tile) {
		if (this->sourceDestination < other.sourceDestination) {
			return true;
		}
		return false;
	}
	return false;

}
/**
 * @param other this is the other point to be compared against this
 * Boolean operator overload
 * Only compares tile and the sourceDestination.  (bool)source is not evaluated
 */
bool Point::operator<(const Point* other) const {
	if (this->tile < other->tile) {
		return true;
	}
	if (this->tile == other->tile) {
		if (this->sourceDestination < other->sourceDestination) {
			return true;
		}
		return false;
	}
	return false;

}
/**
 * @param other this is the other point to be compared against this
 * Boolean operator overload
 * Only compares tile and the sourceDestination.  (bool)source is not evaluated
 */
bool Point::operator!=(const Point &other) const {
	if (this->tile != other.tile) {
		return false;
	}
	if (this->sourceDestination != other.sourceDestination) {
		return false;
	}
	return true;
}
/**
 * @param other this is the other point to be compared against this
 * Boolean operator overload
 * Only compares tile and the sourceDestination.  (bool)source is not evaluated
 */
bool Point::operator!=(const Point* other) const {
	if (this->tile != other->tile) {
		return false;
	}
	if (this->sourceDestination != other->sourceDestination) {
		return false;
	}
	return true;
}
/**
 * @return returns a const of the bool source  true=source false=destination
 */
bool Point::getSource() const {
	return this->source;
}
/**
 * @return returns a const of the source destination field.  This is the point within the tile
 */
std::string Point::getSourceDestination() const {
	return this->sourceDestination;
}
void Point::setIndices(torc::architecture::xilinx::TileIndex tileIndex,
		torc::architecture::xilinx::CompactSegmentIndex segmentIndex) {
	this->segmentIndex = segmentIndex;
}
torc::architecture::xilinx::CompactSegmentIndex Point::getSegmentIndex() const {
	return this->segmentIndex;
}
}
}
#endif /* POINT_H_ */
