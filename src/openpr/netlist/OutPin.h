/*
 * OutPin.h
 *
 *  Created on: Feb 25, 2010
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
#ifndef OUTPIN_H_
#define OUTPIN_H_

#include "openpr/netlist/Pin.h"
namespace openpr {
namespace netlist {
class OutPin: public Pin {
public:
	OutPin();
	OutPin(std::string outpin, std::string location);
	virtual ~OutPin();
	void printData(std::ofstream* outStream);
	void printData(std::ostream* outStream);
	bool operator!=(Pin &other);
	bool operator!=(Pin* other);
	bool operator==(const Pin &other) const;
	bool operator==(const Pin* other) const;
	bool operator<(Pin &other);
	bool operator<(Pin* other);
	bool operator>(Pin &other);
	bool operator>(Pin* other);
	Pin& operator=(Pin &other);
	Pin* operator=(Pin* other);
	size_t operator()(const Pin& x) const;
	size_t operator()(const Pin* x) const;
private:
	std::string outpin;
	std::string location;
};
}
}
#endif /* OUTPIN_H_ */
