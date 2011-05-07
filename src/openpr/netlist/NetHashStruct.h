/*
 * NetHashStruct.h
 *
 *  Created on: Mar 10, 2010
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
#ifndef NETHASHSTRUCT_H_
#define NETHASHSTRUCT_H_
namespace openpr {
namespace netlist {
struct hash_net {
	size_t operator()(const Net* t) const {
		return t->operator ()(t);
	}
};
struct eq_net {
	bool operator()(const Net* t1, const Net* t2) const {
		return t1->operator ==(t2);
	}
};
}
}

#endif /* NETHASHSTRUCT_H_ */
