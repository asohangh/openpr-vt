/*
 * Pin.cpp
 *
 *  Created on: Feb 24, 2010
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

#ifndef PIN_HPP_
#define PIN_HPP_
#include "openpr/netlist/Pin.h"
namespace openpr {
namespace netlist {
Pin::Pin() {
	// TODO Auto-generated constructor stub
}

Pin::~Pin() {
	// TODO Auto-generated destructor stub
}
std::size_t hash_value(Pin & pin) {
	std::size_t seed = pin.operator ()(&pin);
	return seed;
}
std::size_t hash_value(Pin* pin) {
	std::size_t seed = pin->operator ()(pin);
	return seed;
}
}
}
#endif /* PIN_HPP_ */

