/*
 * Point.h
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
#ifndef POINT_H_
#define POINT_H_
#include "openpr/netlist/Pin.h"
#include "torc/architecture/DDB.hpp"
namespace openpr {
namespace netlist {
class Point {
public:
	Point();
	Point(std::string tile, std::string sourceDestination, bool source,
			torc::architecture::DDB* mDB);
	bool operator>(const Point &other) const;
	bool operator>(const Point* other) const;
	bool operator<(const Point &other) const;
	bool operator<(const Point* other) const;
	bool operator!=(const Point &other) const;
	bool operator!=(const Point* other) const;
	bool operator==(const Point &other) const;
	bool operator==(const Point* other) const;
	size_t operator()(const Point& x) const;
	size_t operator()(const Point* x) const;
	bool getSource() const;
	std::string getSourceDestination() const;
	virtual ~Point();

	torc::architecture::xilinx::CompactSegmentIndex getSegmentIndex() const;

private:
	bool source;
	std::string tile;
	std::string sourceDestination;
	torc::architecture::xilinx::CompactSegmentIndex segmentIndex;
	void setIndices(torc::architecture::xilinx::TileIndex tileIndex,
			torc::architecture::xilinx::CompactSegmentIndex segmentIndex);
};
}
}
#endif /* POINT_H_ */
