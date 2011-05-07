/*
 * HashStructs.h
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

#ifndef HASHSTRUCTS_H_
#define HASHSTRUCTS_H_
#include "torc/architecture/Segments.hpp"
namespace openpr {
namespace netlist {
struct hash_pip {
	size_t operator()(const Pin* t) const {
		return t->operator ()(t);
	}
};

struct eq_pip {
	bool operator()(const Pin* t1, const Pin* t2) const {
		if (t1 == NULL || t2 == NULL) {
			return false;
		}
		return *t1 == *t2;
	}
};

struct hash_point {
	size_t operator()(const Point* t) const {
		return t->operator ()(t);
	}
};
struct eq_point {
	bool operator()(const Point* t1, const Point* t2) const {
		return t1->operator ==(t2);
	}
};
struct hash_segment {
	size_t operator()(const torc::architecture::xilinx::CompactSegmentIndex t) const {
		return t;
	}
};
struct eq_segment {

	bool operator()(const torc::architecture::xilinx::CompactSegmentIndex t1, const torc::architecture::xilinx::CompactSegmentIndex t2) const {

		return t1 == t2;
	}
};
}
}

#endif /* HASHSTRUCTS_H_ */
